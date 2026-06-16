//
// Created by blomq on 2026-06-16.
//

#ifndef MINESWEEPER_RNG_H
#define MINESWEEPER_RNG_H

#include <algorithm>
#include <random>
#include <vector>

class RNG {
public:
    using Engine = std::mt19937;

    explicit RNG()
        : generator_(makeSeededEngine()) {
    }

    Engine &generator() {
        return generator_;
    }

    [[nodiscard]] const Engine &generator() const {
        return generator_;
    }

private:
    static Engine makeSeededEngine() {
        std::random_device rd;

        std::vector<std::seed_seq::result_type> seedData(Engine::state_size);
        std::ranges::generate(seedData, [&rd] {
            return rd();
        });

        std::seed_seq seedSequence(seedData.begin(), seedData.end());
        return Engine(seedSequence);
    }

    Engine generator_;
};

#endif //MINESWEEPER_RNG_H
