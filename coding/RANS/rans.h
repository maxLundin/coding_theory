#pragma once

#include <array>
#include <cassert>

#include "util.h"
#include "util/bit_stream.h"

namespace Compress {

    struct RANS_RAW {

        static bytes encode(const bytes &in) {

            bytes ans;

            Stats stats;

            for (const auto ch: in) {
                stats.stats[ch]++;
            }

            const size_type total_size = in.size();

            stats.fill(total_size);
            stats.fill_prefix();

            auto out_iter = std::copy_n((byte *) (&stats.statsByN), sizeof(stats.statsByN), std::back_insert_iterator(ans));

            out_iter = std::copy_n((byte *) (&total_size), sizeof(total_size), out_iter);

            uint32_t x = 1ull << 16;
            const uint32_t D = 32 - N;
            const uint32_t msk = (1ull << 16) - 1;

            const auto flush = [&]() {
                const uint16_t val = x & msk;
                out_iter = std::copy_n((byte *) (&val), 2, out_iter);
                x >>= 16;
            };

            size_t pos = 0;
            for (const auto ch: in) {
                if (x >= (stats.statsByN[ch] << D)) {
                    flush();
                }
                pos++;
                x = ((x / stats.statsByN[ch]) << N) + (x % stats.statsByN[ch]) + stats.prefixSumArray[ch];
                assert(stats.sLineOver[x & ((1ull << N) - 1)] == ch);
            }

            flush();
            flush();

            return ans;
        }

        static bytes decode(const bytes &in) {
            bytes ans;

            Stats stats;

            auto in_pos = in.data();

            size_type total_size;

            std::copy_n(in_pos, sizeof(stats.statsByN), (byte *) (&stats.statsByN));
            in_pos += sizeof(stats.statsByN);

            std::copy_n(in_pos, sizeof(total_size), (byte *) (&total_size));
            in_pos += sizeof(total_size);

            stats.fill_prefix();

            uint32_t x = 0;
            const uint32_t msk = (1ull << N) - 1;

            size_t pos = in.size();

            const auto get = [&]() {
                x <<= 16;
                uint16_t val;
                pos -= 2;
                std::copy_n(in.data() + pos, 2, (byte *) (&val));
                x |= val;
            };
            get();
            get();
            while (total_size--) {
                const auto pos1 = std::upper_bound(stats.prefixSumArray.begin(), stats.prefixSumArray.end(), x & msk) - 1 - stats.prefixSumArray.begin();
                const byte ch = pos1; //stats.sLineOver[x & msk];
                ans.push_back(ch);
                x = stats.statsByN[ch] * (x >> N) + (x & msk) - stats.prefixSumArray[ch];
                if (x < (1ull << 16)) {
                    get();
                }
            }
            std::reverse(ans.begin(), ans.end());

            return ans;
        }

    private:
        static constexpr size_t N = 12;
        static constexpr size_t STATS_SIZE = 256;

        using size_type = uint32_t;

        struct Stats {
            std::array<size_type, STATS_SIZE> stats{};
            std::array<size_type, STATS_SIZE> statsByN{};
            std::array<uint64_t, STATS_SIZE + 1> prefixSumArray{};

            std::array<uint64_t, (1ull << (N + 1))> sLineOver{};

            void fill(size_type total_size) {
                for (size_t i = 0; i < stats.size(); ++i) {
                    const auto val = stats[i] * (1ull << N) / total_size;
                    statsByN[i] = (val > 0 ? val : stats[i] ? 1 : 0);
                }
            }

            void fill_prefix() {
                size_t prefix_sum = 0;

                for (size_t i = 0; i < statsByN.size(); ++i) {
                    prefixSumArray[i] = prefix_sum;
                    std::fill(sLineOver.data() + prefix_sum, sLineOver.data() + prefix_sum + statsByN[i], i);
                    prefix_sum += statsByN[i];
                }
                prefixSumArray.back() = prefix_sum;
            }
        };

    };
}