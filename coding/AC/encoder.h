#pragma once

#include <array>
#include <string_view>

#include "util/bit_stream.h"
#include "model.h"

namespace Compress {

    struct ArithmeticEncoder {
        ArithmeticEncoder(bytes &out, Model *model) : outStream(out), model(model) {}

        void set_model(Model *pModel) {
            this->model = pModel;
        }

        void print(uint64_t bit) {
            outStream.out_bit(bit);
            for (auto i = 0ull; i < btf; ++i) {
                outStream.out_bit(!bit);
            }
            btf = 0;
        }

        void encode(uint64_t code) {
            auto denom = model->get_n();
            auto[bottom, top] = model->get(code);

            auto G = high - low + 1;

            high = low + (G * top) / denom - 1;
            low = low + (G * bottom) / denom;

            while (true) {
                if (high < (1ull << (b - 1))) {
                    print(0);
                    high = (high << 1) + 1ull;
                    low = low << 1;
                } else if (low >= (1ull << (b - 1))) {
                    print(1);
                    low = (low << 1) - (1ull << b);
                    high = (high << 1) - (1ull << b) + 1;
                } else if ((((1ull << (b - 2))) <= low && high < 3 * ((1ull << (b - 2))))) {
                    btf++;
                    low = (low << 1) - (1ull << (b - 1));
                    high = (high << 1) - ((1ull << (b - 1))) + 1;
                } else {
                    break;
                }
            }
        }

        ~ArithmeticEncoder() {
            ++btf;
            if (low < (1ull << (b - 2))) {
                print(0);
            } else {
                print(1);
            }
        }

        obit_stream outStream;
        Model *model;
        uint64_t btf = 0;
        uint64_t high = (1ull << b) - 1;
        uint64_t low = 0;
    };

    struct ArithmeticDecoder {
        ArithmeticDecoder(const bytes &in, Model *model) : inStream(in.data()), model(model) {
            for (size_t i = 0; i < b; ++i) {
                value = (value << 1) + inStream.get_bit();
            }
        }

        void set_model(Model *pModel) {
            this->model = pModel;
        }

        uint64_t decode() {
            auto denom = model->get_n();

            auto G = high - low + 1;

            auto aux = ((value - low + 1) * denom - 1) / G;
            auto symbol = model->get_s(aux);

            auto[bottom, top] = model->get(symbol);

            high = low + (G * top) / denom - 1;
            low = low + (G * bottom) / denom;

            while (true) {
                if (high < ((1ull << (b - 1)))) {
                    high = (high << 1) + 1ull;
                    low = low << 1;
                    value = (value << 1) + inStream.get_bit();
                } else if (low >= ((1ull << (b - 1)))) {
                    low = (low << 1) - (1ull << b);
                    high = (high << 1) - (1ull << b) + 1;
                    value = (value << 1) - (1ull << b) + inStream.get_bit();
                } else if ((((1ull << (b - 2))) <= low && high < 3 * ((1ull << (b - 2))))) {
                    low = (low << 1) - (1ull << (b - 1));
                    high = (high << 1) - (1ull << (b - 1)) + 1;
                    value = (value << 1) - (1ull << (b - 1)) + inStream.get_bit();
                } else {
                    break;
                }
            }
            return symbol;
        }

        ibit_stream inStream;
        Model *model;
        uint64_t value = 0;
        uint64_t high = (1ull << b) - 1;
        uint64_t low = 0;
    };

    template<size_t N = 1>
    struct ARITH {
        static bytes encode(const bytes &in) {
            bytes ans;

            AdaptModel model(N);
            ArithmeticEncoder encoder(ans, &model);

            for (const auto ch: in) {
                encoder.encode(ch);
            }
            encoder.encode(256);

            return ans;
        }

        static bytes decode(const bytes &in) {

            bytes ans;
            AdaptModel model(N);
            ArithmeticDecoder decoder(in, &model);

            while (true) {
                auto ch = decoder.decode();
                if (ch == 256) {
                    break;
                }
                ans.push_back(ch);
            }
            return ans;
        }
    };

    struct ARITH_BIN {
        static bytes encode(const bytes &in) {

            bytes ans;
            AdaptModelBin model;
            ArithmeticEncoder encoder(ans, &model);
            for (const auto ch: in) {
                encoder.encode(ch & 1);
                encoder.encode((ch >> 1) & 1);
                encoder.encode((ch >> 2) & 1);
                encoder.encode((ch >> 3) & 1);
                encoder.encode((ch >> 4) & 1);
                encoder.encode((ch >> 5) & 1);
                encoder.encode((ch >> 6) & 1);
                encoder.encode((ch >> 7) & 1);
            }
            encoder.encode(2);

            return ans;
        }

        static bytes decode(const bytes &in) {

            bytes ans;

            AdaptModelBin model;
            ArithmeticDecoder decoder(in, &model);

            while (true) {
                auto ch1 = decoder.decode();
                if (ch1 == 2) {
                    break;
                }
                auto ch2 = decoder.decode();
                auto ch3 = decoder.decode();
                auto ch4 = decoder.decode();
                auto ch5 = decoder.decode();
                auto ch6 = decoder.decode();
                auto ch7 = decoder.decode();
                auto ch8 = decoder.decode();
                auto ch = (ch1 | (ch2 << 1) | (ch3 << 2) | (ch4 << 3) | (ch5 << 4) | (ch6 << 5) | (ch7 << 6) |
                           (ch8 << 7));
                ans.push_back(ch);
            }
            return ans;
        }
    };
}