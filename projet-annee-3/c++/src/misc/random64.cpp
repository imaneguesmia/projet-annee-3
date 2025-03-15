#include "random64.hpp"

#include <random>

// Seed for random number generation.
static constexpr int SEED {1};

// Generate a pseudo-random uint64_t.
uint64_t random64() {
    static std::mt19937_64 generator(SEED);
    static std::uniform_int_distribution<uint64_t> distribution;

    return distribution(generator);
}