#pragma once

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <tuple>

#include "stats.h"

class Model {
public:
    virtual std::tuple<uint64_t, uint64_t> get(uint64_t) = 0;

    virtual uint64_t get_n() = 0;

    virtual uint64_t get_s(uint64_t aux) = 0;
};

class LameModel final : public Model {
public:
    explicit LameModel(uint64_t val) : val(val) {}

    std::tuple<uint64_t, uint64_t> get(uint64_t code) final {
        return {code, code + 1};
    }

    uint64_t get_n() final {
        return val;
    }

    uint64_t get_s(uint64_t aux) final {
        return aux;
    }

    void set_val(uint64_t mVal) {
        this->val = mVal;
    }

    uint64_t val;
};

class SuperModel final : public Model {
public:
    explicit SuperModel(Stats<256> &ostats) : stats(ostats) {
    }

    std::tuple<uint64_t, uint64_t> get(uint64_t code) final {
        for (size_t i = code + 1; i < stats.prefix.size(); ++i) {
            stats.prefix[i]--;
        }
        return {stats.prefix[code], stats.prefix[code + 1] + 1};
    }

    uint64_t get_n() final {
        return stats.prefix[stats.prefix.size() - 1];
    }

    uint64_t get_s(uint64_t aux) final {
        return std::upper_bound(stats.prefix.begin(), stats.prefix.end(), aux) - 1 - stats.prefix.begin();
    }

    Stats<256> &stats;
};

class AdaptModel final : public Model {
public:
    explicit AdaptModel(size_t N = 2) : stats(), N(N) {
        for (size_t i = 0; i < stats.prefix.size(); ++i) {
            stats.prefix[i] = i;
        }
    }

    std::tuple<uint64_t, uint64_t> get(uint64_t code) final {
        for (size_t i = code + 1; i < stats.prefix.size(); ++i) {
            stats.prefix[i] += N;
        }
        return {stats.prefix[code], stats.prefix[code + 1] - N};
    }

    uint64_t get_n() final {
        return stats.prefix[stats.prefix.size() - 1];
    }

    uint64_t get_s(uint64_t aux) final {
        return std::upper_bound(stats.prefix.begin(), stats.prefix.end(), aux) - 1 - stats.prefix.begin();
    }

    Stats<256> stats;
    size_t N;
};

class AdaptModelBin final : public Model {
public:
    AdaptModelBin() : stats() {
        for (size_t i = 0; i < stats.prefix.size(); ++i) {
            stats.prefix[i] = i;
        }
    }

    std::tuple<uint64_t, uint64_t> get(uint64_t code) final {
        for (size_t i = code + 1; i < stats.prefix.size(); ++i) {
            stats.prefix[i] += 1;
        }
        return {stats.prefix[code], stats.prefix[code + 1] - 1};
    }

    uint64_t get_n() final {
        return stats.prefix[stats.prefix.size() - 1];
    }

    uint64_t get_s(uint64_t aux) final {
        return std::upper_bound(stats.prefix.begin(), stats.prefix.end(), aux) - 1 - stats.prefix.begin();
    }

    Stats<2> stats;
};

