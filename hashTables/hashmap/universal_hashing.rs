use crate::hashmap::UpdatableBuildHasher;
use rand::prelude::*;
use std::hash::{BuildHasher, Hasher};

pub struct UniversalHasher {
    initial_value: u64,
    a: u64,
    cur: u64,
}

impl Hasher for UniversalHasher {
    fn finish(&self) -> u64 {
        self.cur
    }

    fn write(&mut self, bytes: &[u8]) {
        let mut h = self.initial_value;
        for x in bytes {
            h = h.wrapping_mul(self.a).wrapping_add(*x as u64);
        }
        self.cur = h;
    }
}

pub struct UniversalHasherBuilder {
    rng: ThreadRng,
    a: u64,
    initial_value: u64,
}

impl BuildHasher for UniversalHasherBuilder {
    type Hasher = UniversalHasher;

    fn build_hasher(&self) -> Self::Hasher {
        UniversalHasher {
            initial_value: self.initial_value,
            a: self.a,
            cur: 0,
        }
    }
}

impl Default for UniversalHasherBuilder {
    fn default() -> Self {
        let mut rng = rand::rng();
        Self {
            a: rng.next_u64(),
            initial_value: rng.next_u64(),
            rng,
        }
    }
}

impl UniversalHasherBuilder {
    pub fn new() -> Self {
        Self::default()
    }
}

impl UpdatableBuildHasher for UniversalHasherBuilder {
    fn update(&mut self) {
        self.a = self.rng.next_u64();
        self.initial_value = self.rng.next_u64();
    }
}
