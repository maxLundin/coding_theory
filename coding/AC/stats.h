#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

template<size_t N = 256>
struct Stats
{
    constexpr static size_t count_size = N + 1;

    std::array<std::pair<size_t, uint8_t>, count_size> count{};
    std::array<size_t, count_size + 1> prefix{};
};

constexpr size_t b = 31;
