#pragma once

#include <fstream>
#include <cstring>
#include "util.h"

namespace Compress {

    struct ibit_stream {

        explicit ibit_stream(const byte *in) : pos(0), val(0), in(in) {
            memcpy(reinterpret_cast<char *>(&val), in, sizeof(val));
            this->in += sizeof(val);
        }

        bool get_bit() {
            if (pos == 8 * sizeof(val)) {
                memcpy(reinterpret_cast<char *>(&val), in, sizeof(val));
                in += sizeof(val);
                pos = 0;
            }
            auto out = (val >> pos) & 1;
            pos++;
            return out;
        }

        uint64_t pos;
        uint64_t val;
        const byte *in;
    };

    struct obit_stream {

        explicit obit_stream(bytes &out) : pos(0), val(0), out(out) {}

        void out_bit(uint64_t bit) {
            if (pos == 8 * sizeof(val)) {
                std::copy_n((byte *) (&val), sizeof(val), out);
                pos = 0;
                val = 0;
            }
            val |= (bit << pos);
            pos++;
        }

        ~obit_stream() {
            std::copy_n((byte *) (&val), (pos + 7) / 8, out);
        }

        uint64_t pos;
        uint64_t val;
        std::back_insert_iterator<bytes> out;
    };

}
