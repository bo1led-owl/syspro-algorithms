struct Node<T> {
    pub value: T,
    pub next: Option<Box<Node<T>>>,
}

pub struct LinkedList<T> {
    head: Option<Box<Node<T>>>,
    len: usize,
}

impl<T> Default for LinkedList<T> {
    fn default() -> Self {
        Self { head: None, len: 0 }
    }
}

impl<T> LinkedList<T> {
    pub fn new() -> LinkedList<T> {
        Self::default()
    }

    pub fn len(&self) -> usize {
        self.len
    }

    pub fn push_front(&mut self, x: T) {
        self.len += 1;
        self.head = Some(Box::new(Node {
            value: x,
            next: std::mem::replace(&mut self.head, None),
        }));
    }

    pub fn pop_front(&mut self) -> Option<T> {
        match std::mem::replace(&mut self.head, None) {
            Some(mut n) => {
                self.len -= 1;
                self.head = std::mem::replace(&mut n.next, None);
                Some(n.value)
            }
            None => None,
        }
    }

    pub fn remove_matching<F: FnMut(&mut T) -> bool>(&mut self, mut f: F) -> Option<T> {
        if self.head.is_none() {
            return None;
        }

        if f(&mut self.head.as_mut().unwrap().as_mut().value) {
            self.len -= 1;
            return self.pop_front();
        }

        let mut prev: &mut Node<T> = self.head.as_deref_mut().unwrap();
        while let Some(Node { value, next }) = prev.next.as_deref_mut() {
            if f(value) {
                self.len -= 1;
                let new_next = std::mem::replace(next, None);
                let cur = std::mem::replace(&mut prev.next, new_next).take().unwrap();
                return Some(cur.value);
            } else {
                prev = prev.next.as_deref_mut().unwrap();
            }
        }

        return None;
    }

    pub fn iter(&self) -> LinkedListIterator<T> {
        LinkedListIterator {
            next: self.head.as_deref(),
        }
    }
    pub fn iter_mut(&mut self) -> LinkedListMutIterator<T> {
        LinkedListMutIterator {
            next: self.head.as_deref_mut(),
        }
    }
}

pub struct LinkedListIterator<'a, T> {
    next: Option<&'a Node<T>>,
}

pub struct LinkedListMutIterator<'a, T> {
    next: Option<&'a mut Node<T>>,
}

impl<'a, T> Iterator for LinkedListIterator<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        self.next.map(|node| {
            self.next = node.next.as_deref();
            &node.value
        })
    }
}

impl<'a, T> Iterator for LinkedListMutIterator<'a, T> {
    type Item = &'a mut T;

    fn next(&mut self) -> Option<Self::Item> {
        self.next.take().map(|node| {
            self.next = node.next.as_deref_mut();
            &mut node.value
        })
    }
}

impl<T> Drop for LinkedList<T> {
    fn drop(&mut self) {
        while self.pop_front().is_some() {}
    }
}
