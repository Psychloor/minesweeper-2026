//
// Created by blomq on 2026-06-16.
//

#ifndef MINESWEEPER_RNG_H
#define MINESWEEPER_RNG_H

#include <algorithm>
#include <functional>
#include <random>
#include <vector>

class RNG {
public:
    explicit RNG() { // NOLINT(*-msc51-cpp)
        std::random_device rd;

        std::vector<std::seed_seq::result_type> seedData(std::mt19937::state_size);
        std::ranges::generate(seedData, std::ref(rd));

        std::seed_seq seedSequence(seedData.begin(), seedData.end());
        generator_ = std::mt19937(seedSequence);
    }

    std::mt19937 &generator() {
        return generator_;
    }

private:
    std::mt19937 generator_;
};

#endif //MINESWEEPER_RNG_H
