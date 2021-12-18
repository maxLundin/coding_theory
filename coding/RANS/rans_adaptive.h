#pragma once

#include <array>
#include <cassert>

#include "util.h"
#include "util/bit_stream.h"

namespace Compress {

    struct RANS_ADAPTIVE {

        static bytes encode(const bytes &in) {

            bytes ans;

            const size_type total_size = in.size();
            Stats stats;

            for (const auto ch: in) {
                stats.inc(ch);
            }

            for (size_t i = 0; i < stats.stats.size(); ++i) {
                stats.inc(i);
            }

            auto out_iter = std::back_insert_iterator(ans);

            out_iter = std::copy_n((byte *) (&total_size), sizeof(total_size), out_iter);

            uint64_t x = 1ull << 16;
            const uint32_t D = 32 - N;
            const uint32_t msk = (1ull << 16) - 1;

            const auto flush = [&]() {
                const uint16_t val = x & msk;
                out_iter = std::copy_n((byte *) (&val), sizeof(val), out_iter);
                x >>= 16;
            };

            for (size_t pos = in.size(); pos--;) {
                const auto ch = in[pos];
                stats.dec(ch);
                const auto stat = stats.statsByN(ch);
                if (x >= (stat << D)) {
                    flush();
                }
                x = ((x / stat) << N) + (x % stat) + stats.prefixSumArray[ch];
                assert(std::upper_bound(stats.prefixSumArray.begin(), stats.prefixSumArray.end(),
                                        x & ((1ull << N) - 1)) - 1 - stats.prefixSumArray.begin() == ch);
            }

            flush();
            flush();

            return ans;
        }

        static bytes decode(const bytes &in) {
            bytes ans;

            size_type total_size;

            auto in_pos = in.data();
            std::copy_n(in_pos, sizeof(total_size), (byte *) (&total_size));
            in_pos += sizeof(total_size);

            Stats stats;

            for (size_t i = 0; i < stats.stats.size(); ++i) {
                stats.inc(i);
            }

            uint64_t x = 0;
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
                const auto pos1 =
                        std::upper_bound(stats.prefixSumArray.begin(), stats.prefixSumArray.end(), x & msk) - 1 -
                        stats.prefixSumArray.begin();
                const byte ch = pos1;
                ans.push_back(ch);
                x = stats.statsByN(ch) * (x >> N) + (x & msk) - stats.prefixSumArray[ch];
                if (x < (1ull << 16)) {
                    get();
                }
                stats.inc(ch);
            }

            return ans;
        }

    private:
        static constexpr size_t N = 12;
        static constexpr size_t STATS_SIZE = 256;

        using size_type = uint32_t;

        struct Stats {

            std::array<uint64_t, STATS_SIZE> stats{};
            std::array<uint64_t, STATS_SIZE + 1> prefixSumArray{};

            size_t total_size{0};

            uint64_t statsByN(byte ch) {
                const auto val = stats[ch] * (1ull << N) / (total_size + 1);
                return (val > 0 ? val : stats[ch] ? 1 : 0);
            }

            void rebuild() {
                prefixSumArray[0] = 0;
                for (size_t i = 1; i < prefixSumArray.size(); ++i) {
                    prefixSumArray[i] = prefixSumArray[i - 1] + statsByN(i - 1);
                }
            }

            void dec(byte ch) {
                stats[ch]--;
                total_size--;
                rebuild();
            }

            void inc(byte ch) {
                stats[ch]++;
                total_size++;
                rebuild();
            }
        };

    };
}