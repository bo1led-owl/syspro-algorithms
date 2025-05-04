use crate::linked_list::LinkedList;
use std::{
    borrow::Borrow,
    collections::BTreeMap,
    hash::{BuildHasher, Hash, RandomState},
};

struct Entry<K: Eq, V> {
    pub key: K,
    pub value: V,
}

const CAPACITIES: [usize; 11] = [17, 37, 79, 163, 331, 673, 1361, 2729, 5471, 10949, 21911];
const MAX_LOAD_FACTOR: f32 = 0.75;
const DEFAULT_CHAIN_LEN_BOUNDARY: usize = 25;

fn next_prime(cur: usize) -> usize {
    'outer: for i in cur + 1..usize::MAX {
        let mut j = 2;
        while j * j <= i {
            if i % j == 0 {
                continue 'outer;
            }
            j += 1;
        }
        return j;
    }
    panic!()
}

fn next_capacity(cur: usize) -> usize {
    CAPACITIES
        .into_iter()
        .find(|i| *i > cur)
        .unwrap_or(next_prime(cur))
}

enum Chain<K: Hash + Eq + Ord, V> {
    List(LinkedList<Entry<K, V>>),
    Tree(BTreeMap<K, V>),
}

impl<K: Hash + Eq + Ord, V> Chain<K, V> {
    pub fn new() -> Self {
        Self::List(LinkedList::new())
    }

    fn list_to_tree(&mut self) {
        let list = std::mem::replace(self, Chain::Tree(BTreeMap::new()));
        match (self, list) {
            (Chain::Tree(tree), Chain::List(mut list)) => {
                while let Some(entry) = list.pop_front() {
                    tree.insert(entry.key, entry.value);
                }
            }
            _ => unreachable!(),
        }
    }

    pub fn push(&mut self, entry: Entry<K, V>, boundary: usize) {
        match self {
            Chain::List(list) => {
                if list.len() + 1 < boundary {
                    list.push_front(entry);
                } else {
                    self.list_to_tree();
                    self.push(entry, boundary);
                }
            }
            Chain::Tree(tree) => {
                tree.insert(entry.key, entry.value);
            }
        }
    }

    pub fn pop(&mut self) -> Option<Entry<K, V>> {
        match self {
            Chain::List(list) => list.pop_front(),
            Chain::Tree(tree) => tree.pop_first().map(|(k, v)| Entry { key: k, value: v }),
        }
    }

    pub fn remove_with_key<'a, Q>(&mut self, key: &'a Q) -> Option<V>
    where
        K: Borrow<Q>,
        Q: Eq + Ord + ?Sized,
    {
        match self {
            Chain::List(list) => list
                .remove_matching(|entry: &mut Entry<K, V>| entry.key.borrow() == key)
                .map(|e| e.value),
            Chain::Tree(tree) => tree.remove(key),
        }
    }

    pub fn get<'a, 'b, Q>(&'a self, key: &'b Q) -> Option<&'a V>
    where
        K: Borrow<Q>,
        Q: Hash + Eq + Ord + ?Sized,
    {
        match self {
            Chain::List(chain) => chain.iter().find_map(|e| {
                if e.key.borrow() == key {
                    Some(&e.value)
                } else {
                    None
                }
            }),
            Chain::Tree(tree) => tree.get(key),
        }
    }

    pub fn get_mut<'a, 'b, Q>(&'a mut self, key: &'b Q) -> Option<&'a mut V>
    where
        K: Borrow<Q>,
        Q: Hash + Eq + Ord + ?Sized,
    {
        match self {
            Chain::List(chain) => chain.iter_mut().find_map(|e| {
                if e.key.borrow() == key {
                    Some(&mut e.value)
                } else {
                    None
                }
            }),
            Chain::Tree(tree) => tree.get_mut(key),
        }
    }
}

pub struct HashMap<K: Hash + Eq + Ord, V, S: BuildHasher = RandomState> {
    state: S,
    chains: Option<Box<[Chain<K, V>]>>,
    len: usize,
    chain_len_boundary: usize,
}

impl<K: Hash + Eq + Ord, V, S: Default + BuildHasher> Default for HashMap<K, V, S> {
    fn default() -> Self {
        Self {
            state: S::default(),
            chains: None,
            len: 0,
            chain_len_boundary: DEFAULT_CHAIN_LEN_BOUNDARY,
        }
    }
}

impl<K: Hash + Eq + Ord, V, S: Default + BuildHasher> HashMap<K, V, S> {
    pub fn new() -> Self {
        Default::default()
    }

    pub fn new_with_chain_len_boundary(boundary: usize) -> Self {
        Self {
            state: S::default(),
            chains: None,
            len: 0,
            chain_len_boundary: boundary,
        }
    }
}

pub trait UpdatableBuildHasher: BuildHasher {
    fn update(&mut self) {}
}

impl UpdatableBuildHasher for RandomState {}

impl<K: Hash + Eq + Ord, V, S: UpdatableBuildHasher> HashMap<K, V, S> {
    pub fn with_hasher(hash_builder: S) -> Self {
        Self {
            state: hash_builder,
            chains: None,
            len: 0,
            chain_len_boundary: DEFAULT_CHAIN_LEN_BOUNDARY,
        }
    }

    pub fn with_hasher_and_chain_len_boundary(hash_builder: S, boundary: usize) -> Self {
        Self {
            state: hash_builder,
            chains: None,
            len: 0,
            chain_len_boundary: boundary,
        }
    }

    pub fn len(&self) -> usize {
        self.len
    }

    pub fn capacity(&self) -> usize {
        self.chains.as_ref().map_or(0, |b| b.len())
    }

    fn load_factor(&self) -> f32 {
        if self.capacity() == 0 {
            f32::INFINITY
        } else {
            self.len as f32 / self.capacity() as f32
        }
    }

    pub fn get<Q>(&self, k: &Q) -> Option<&V>
    where
        K: Borrow<Q>,
        Q: Hash + Eq + Ord + ?Sized,
    {
        self.chains.as_ref().and_then(|c| {
            let hash = self.state.hash_one(k) as usize % self.capacity();
            c[hash].get(k)
        })
    }

    pub fn get_mut<'a, 'b, Q>(&'a mut self, k: &'b Q) -> Option<&'a mut V>
    where
        K: Borrow<Q>,
        Q: Hash + Eq + Ord + ?Sized,
    {
        let hash = self.state.hash_one(k) as usize % self.capacity();
        self.chains.as_mut().and_then(|c| c[hash].get_mut(k))
    }

    fn rehash_if_needed(&mut self) {
        if self.load_factor() < MAX_LOAD_FACTOR {
            return;
        }

        self.state.update();

        let new_capacity = next_capacity(self.capacity());
        let mut new_chains = Vec::with_capacity(new_capacity);
        new_chains.resize_with(new_capacity, || Chain::<K, V>::new());
        let mut new_chains = new_chains.into_boxed_slice();

        if self.chains.is_none() {
            self.chains = Some(new_chains);
            return;
        }

        for chain in self.chains.as_mut().unwrap().iter_mut() {
            while let Some(entry) = chain.pop() {
                let hash = self.state.hash_one(&entry.key) as usize % new_capacity;
                new_chains[hash].push(entry, self.chain_len_boundary);
            }
        }

        self.chains = Some(new_chains);
    }

    pub fn insert(&mut self, k: K, v: V) {
        let prev = if self.capacity() != 0 {
            let hash = self.state.hash_one(&k) as usize % self.capacity();
            self.chains.as_mut().and_then(|c| c[hash].get_mut(&k))
        } else {
            None
        };

        if let Some(prev) = prev {
            *prev = v;
            return;
        }

        self.len += 1;
        self.rehash_if_needed();

        let hash = self.state.hash_one(&k) as usize % self.capacity();
        self.chains.as_mut().unwrap()[hash]
            .push(Entry { key: k, value: v }, self.chain_len_boundary);
    }

    pub fn update_or_insert<F: FnMut(&mut V)>(&mut self, k: K, v: V, mut f: F) {
        let prev = if self.capacity() != 0 {
            let hash = self.state.hash_one(&k) as usize % self.capacity();
            self.chains.as_mut().and_then(|c| c[hash].get_mut(&k))
        } else {
            None
        };

        if let Some(prev) = prev {
            f(prev);
            return;
        }

        self.len += 1;
        self.rehash_if_needed();

        let hash = self.state.hash_one(&k) as usize % self.capacity();
        self.chains.as_mut().unwrap()[hash]
            .push(Entry { key: k, value: v }, self.chain_len_boundary);
    }

    pub fn remove<Q>(&mut self, k: &Q) -> Option<V>
    where
        K: Borrow<Q>,
        Q: Hash + Eq + Ord + ?Sized,
    {
        if self.capacity() == 0 {
            return None;
        }

        let hash = self.state.hash_one(&k) as usize % self.capacity();
        self.chains
            .as_mut()
            .and_then(|c| c[hash].remove_with_key(k))
    }
}
