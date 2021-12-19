#pragma once

#include <algorithm>
#include <bitset>
#include <concepts>
#include <cstring>
#include <iostream>
#include <list>
#include <numeric>
#include <set>
#include <ranges>
#include <vector>

#include "util.h"

#include "AC/encoder.h"

#include "BlockHuffman/encoder/encoder.h"
#include "BlockHuffman/decoder/decoder.h"

#include "RANS/rans.h"
#include "RANS/rans_adaptive.h"

namespace Compress {

    struct BWT {
        using index = int32_t;

        static bytes encode(const bytes &in) {
            bytes cycled = in;
            cycled.reserve(2 * cycled.size());
            std::copy(begin(in), end(in), std::back_insert_iterator<decltype(cycled)>(cycled));

            std::vector<index> indexes;
            indexes.reserve(in.size());
            for (size_t i = 0; i < in.size(); i++) {
                indexes.emplace_back(i);
            }

            std::ranges::sort(indexes, [&cycled](const auto &a, const auto &b) {
                return std::lexicographical_compare(&cycled[a], &cycled[a] + cycled.size() / 2, &cycled[b],
                                                    &cycled[b] + cycled.size() / 2);
            });

            const index answer_index = static_cast<index>(std::find(begin(indexes), end(indexes), 0) - begin(indexes));

            bytes ans_vector;
            ans_vector.reserve(in.size());
            for (size_t i = 0; i < in.size(); i++) {
                ans_vector.push_back(cycled[indexes[i] + in.size() - 1]);
            }

            byte tmp[sizeof(answer_index)];
            memcpy(&tmp[0], &answer_index, sizeof(answer_index));

            for (const auto &i: tmp) {
                ans_vector.push_back(i);
            }

            return ans_vector;
        }

        static bytes decode(const bytes &in) {
            index cur;

            std::vector<index> indexes(in.size() - sizeof(cur));
            for (size_t i = 0; i < indexes.size(); i++) {
                indexes[i] = i;
            }

            std::ranges::stable_sort(indexes,
                                     [&in](const auto &a, const auto &b) { return in[a] < in[b]; });

            byte tmp[sizeof(cur)];
            for (size_t i = 0; i < sizeof(cur); ++i) {
                tmp[i] = in[in.size() - sizeof(cur) + i];
            }

            memcpy(&cur, &tmp[0], sizeof(cur));
            auto new_size = in.size() - sizeof(cur);

            bytes ans_vector(new_size);
            for (size_t i = 0; i < new_size; i++) {
                ans_vector[i] = in[indexes[cur]];
                cur = indexes[cur];
            }

            return ans_vector;
        }
    };

    struct MTF {
        static bytes encode(const bytes &in) {
            std::list<byte> queue;
            for (size_t i = 0; i < 256; i++) {
                queue.push_front(i);
            }

            bytes ans;
            ans.reserve(in.size());

            for (const auto elem: in) {
                byte index = 0;
                auto it = queue.begin();
                for (; *it != elem; it++) {
                    index++;
                }

                if (it != queue.begin()) {
                    queue.splice(queue.begin(), queue, it, std::next(it));
                }

                ans.push_back(index);
            }

            return ans;
        }

        static bytes decode(const bytes &in) {
            std::list<byte> queue;
            for (size_t i = 0; i < 256; i++) {
                queue.push_front(i);
            }

            bytes ans;
            ans.reserve(in.size());

            for (const auto elem: in) {
                auto it = queue.begin();
                std::advance(it, elem);
                const byte symbol = *it;

                if (it != queue.begin()) {
                    queue.splice(queue.begin(), queue, it, std::next(it));
                }

                ans.push_back(symbol);
            }

            return ans;
        }
    };

    struct RLE {

        static bytes decode(const bytes &in) {
            bytes ans;

            int32_t prev = -1;

            for (size_t i = 0; i < in.size(); i++) {
                auto ch = in[i];
                ans.push_back(ch);
                if (ch == prev) {
                    auto seq_size = in[i++];
                    for (size_t j = 0; j < seq_size; j++) {
                        ans.push_back(prev);
                    }
                }
                prev = ch;
            }
            return ans;
        }

        static bytes encode(const bytes &in) {
            constexpr size_t MAX_SIZE = 255 + 2;

            bytes ans;

            size_t cur_seq_size = 0;

            int32_t prev = -1;

            auto flush = [&]() {
                if (cur_seq_size < 2)
                    return;
                auto repeatedByte = prev;
                size_t seq_size = (cur_seq_size - 2);
                ans.push_back(repeatedByte);
                ans.push_back(seq_size);
                cur_seq_size = 1;
            };

            for (const auto ch: in) {
                if (ch == prev) {
                    cur_seq_size++;
                } else {
                    flush();
                    ans.push_back(ch);
                }
                if (cur_seq_size == MAX_SIZE) {
                    flush();
                }
                prev = ch;
            }
            flush();
            return ans;
        }
    };

    struct HUFF {
        static bytes decode(const bytes &in) {
            std::string str(in.begin(), in.end());
            auto out = HuffEncode::decode(str);
            return bytes{out.begin(), out.end()};
        }

        static bytes encode(const bytes &in) {
            std::string str(in.begin(), in.end());
            auto out = HuffEncode::encode(str);
            return bytes{out.begin(), out.end()};
        }
    };

//    struct RANS {
//        static bytes decode(const bytes &in) {
//            return {};
//        }
//
//        static bytes encode(const bytes &in) {
//            std::string str(in.begin(), in.end());
//            auto out = HuffEncode::encode(str);
//            return bytes{out.begin(), out.end()};
//        }
//    };

    template<class T>
    concept Encoder = requires {{ T::encode(std::declval<bytes>()) } -> std::same_as<bytes>; };

    template<class T>
    concept Decoder = requires {{ T::decode(std::declval<bytes>()) } -> std::same_as<bytes>; };

    template<class T>
    concept CoderConcept = (Encoder<T> && Decoder<T>);

    template<class... T>
    struct Coder;

    template<CoderConcept T>
    struct Coder<T> {
        static bytes encode(const bytes &in) {
            return T::encode(in);
        }

        static bytes decode(const bytes &in) {
            return T::decode(in);
        }
    };

    template<CoderConcept T, CoderConcept... Types>
    struct Coder<T, Types...> {
        static bytes encode(const bytes &in) {
            return Coder<Types...>::encode(T::encode(in));
        }

        static bytes decode(const bytes &in) {
            return T::decode(Coder<Types...>::decode(in));
        }
    };

    template<CoderConcept... Types>
    struct BestOfCoder {
        static bytes encode(const bytes &in) {
            auto out1 = std::vector<bytes>{Types::encode(in)...};
            auto min_size = std::min_element(begin(out1), end(out1),
                                             [](const auto &a, const auto &b) { return a.size() < b.size(); });
            min_size->push_back(min_size - begin(out1));
            return *min_size;
        }

        static bytes decode(const bytes &in) {
            auto in_cp = in;
            const auto n = in_cp.back();
            in_cp.pop_back();
            return decodeN<0, Types...>(n, in_cp);
        }

    private:
        template<size_t N>
        static bytes decodeN(size_t, const bytes &) {
            throw std::logic_error("sizeof(Types...) < n");
        }

        template<size_t N, CoderConcept SubType, CoderConcept... SubTypes>
        static bytes decodeN(size_t n, const bytes &in) {
            if (N == n) {
                return SubType::decode(in);
            }
            return decodeN<N + 1, SubTypes...>(n, in);
        }
    };

}
