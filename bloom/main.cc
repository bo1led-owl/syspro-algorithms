#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <thread>

namespace {
class IpAddress {
    uint32_t repr_;

public:
    IpAddress() : repr_{0} {}
    explicit IpAddress(uint32_t val) : repr_{val} {}
    IpAddress(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4)
        : repr_{static_cast<uint32_t>(o1) << 24 | static_cast<uint32_t>(o2) << 16 |
                static_cast<uint32_t>(o3) << 8 | static_cast<uint32_t>(o4)} {}

    uint32_t repr() const noexcept { return repr_; }

    using Octets = std::array<uint8_t, 4>;

    Octets octets() const noexcept {
        return std::array<uint8_t, 4>{
            static_cast<uint8_t>((repr_ >> 24) & 0xFF),
            static_cast<uint8_t>((repr_ >> 16) & 0xFF),
            static_cast<uint8_t>((repr_ >> 8) & 0xFF),
            static_cast<uint8_t>(repr_ & 0xFF),
        };
    }
};

std::ostream& operator<<(std::ostream& os, IpAddress addr) {
    auto octets = addr.octets();
    return os << +octets[0] << '.' << +octets[1] << '.' << +octets[2] << '.' << +octets[3];
}

using HashFn = std::move_only_function<size_t(const IpAddress&)>;

template <std::uniform_random_bit_generator G>
HashFn newHashFunction(G& gen) {
    std::uniform_int_distribution<size_t> distrib(std::numeric_limits<size_t>::min(),
                                                  std::numeric_limits<size_t>::max());
    size_t a1 = distrib(gen), a2 = distrib(gen), a3 = distrib(gen), a4 = distrib(gen);
    return [a1, a2, a3, a4](const IpAddress& addr) -> size_t {
        auto octets = addr.octets();
        return a1 * static_cast<size_t>(octets[0]) + a2 * static_cast<size_t>(octets[1]) +
               a3 * static_cast<size_t>(octets[2]) + a4 * static_cast<size_t>(octets[3]);
    };
}

class BloomFilter {
    static bool isPrime(size_t n) {
        for (size_t i = 2; i * i <= n; ++i) {
            if (n % i == 0) return false;
        }
        return true;
    }

    static size_t nextPrime(size_t n) {
        for (size_t i = n; i <= std::numeric_limits<size_t>::max(); ++i) {
            if (isPrime(i)) { return i; }
        }
        throw std::runtime_error{"couldn't find the next prime"};
    }

    std::vector<HashFn> hashes_;
    std::vector<bool> bitset_;

public:
    template <std::uniform_random_bit_generator G>
    explicit BloomFilter(G& gen, size_t requests, double epsilon) {
        double k = std::log(epsilon) / std::log(0.5);
        size_t b = static_cast<size_t>(ceil(k / std::log(2.0)));

        bitset_ = std::vector<bool>(nextPrime(b * requests));

        size_t nHashes = static_cast<size_t>(ceil(k));
        hashes_ = std::vector<HashFn>(nHashes);
        std::ranges::for_each(hashes_, [&gen](HashFn& f) { f = newHashFunction(gen); });
    }

    size_t memoryConsumed() const noexcept { return bitset_.size() / 8; }

    void insert(IpAddress addr) noexcept {
        for (auto& fn : hashes_) {
            bitset_[fn(addr) % bitset_.size()] = true;
        }
    }

    bool lookup(IpAddress addr) noexcept {
        bool res = true;
        for (auto& fn : hashes_) {
            res &= bitset_[fn(addr) % bitset_.size()];
        }
        return res;
    }
};
}  // namespace

constexpr size_t N = 1000;
constexpr double EPS = 0.0001;
constexpr size_t THREADS = 16;
constexpr size_t CHUNK = (1z << 32) / THREADS;
static_assert((1z << 32) % THREADS == 0);

std::atomic<size_t> falsePositives = 0;

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    BloomFilter bf{gen, N, EPS};
    std::cout << "filter consumed " << bf.memoryConsumed() << " instead of "
              << N * sizeof(IpAddress) << " bytes of memory (values only)\n";

    std::vector<uint32_t> addresses(N);
    {
        std::uniform_int_distribution<uint32_t> dist;
        for (uint32_t& x : addresses) {
            x = dist(gen);
            bf.insert(IpAddress(x));
        }
    }
    std::ranges::sort(addresses);

    for (uint32_t raw_addr : addresses) {
        IpAddress addr{raw_addr};
        if (!bf.lookup(addr)) { std::cerr << "FAIL: did not find address " << addr << '\n'; }
    }

    {
        std::vector<std::jthread> threads(THREADS);
        for (size_t i = 0; auto& thrd : threads) {
            size_t start = i * CHUNK;
            size_t end = static_cast<size_t>(i + 1) * CHUNK - 1;
            ++i;
            thrd = std::jthread{[&bf, &addresses, start, end]() {
                for (size_t i = start; i <= end; ++i) {
                    if (!std::ranges::binary_search(addresses, i) && bf.lookup(IpAddress(i))) {
                        // std::lock_guard l{mut};
                        falsePositives += 1;
                        // std::cerr << "false positive on address " << IpAddress(i) << '\n';
                    }
                }
            }};
        }
    }

    size_t totalLookupRequests = (1z << 32) - N;
    std::cout << falsePositives << " false positives in " << totalLookupRequests << " requests\n";
    std::cout << (static_cast<double>(falsePositives) / static_cast<double>(totalLookupRequests)) *
                     100.0
              << "% of all lookup requests gave false positives\n";
}
