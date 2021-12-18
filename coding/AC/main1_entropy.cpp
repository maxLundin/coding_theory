//
// Created by mlundin on 11/10/21.
//

#include <bits/stdc++.h>

using namespace std;

int main(int argv, char **args) {
    std::ifstream in(args[1], std::ios::in | std::ios::binary);

    std::vector<size_t> arr(256, 0);
    std::vector<std::vector<size_t>> arr1(256, arr);
    std::map<std::pair<size_t, size_t>, std::vector<size_t>> arr2;

    size_t size = 0;
    uint8_t ch1 = 0;
    uint8_t ch2 = 0;
    while (true) {
        auto ch = in.get();
        if (ch == std::char_traits<char>::eof()) {
            break;
        }
        arr[(uint8_t) ch]++;
        arr1[(uint8_t) ch1][(uint8_t) ch]++;
        auto &aaa = arr2[{(uint8_t) ch2,(uint8_t) ch1}];
        if (aaa.empty()) {
            aaa.resize(256);
        }

        aaa[ch]++;

        ch2 = ch1;
        ch1 = ch;
        size++;
    }
    {
        double x = 0;
        for (size_t i : arr) {
            if (i == 0) {
                continue;
            }
            double p = (1. * i) / size;
            x += -p * log2(p);
        }
        std::cout << x << " ";
    }

    {
        double x = 0;
        for (auto &a : arr1) {
            size_t sum_a = std::accumulate(a.begin(), a.end(), 0ull);
            for (size_t i : a) {
                if (i == 0) {
                    continue;
                }
                double p = (1. * i) / sum_a;
                x += -(1. * sum_a) / size * p * log2(p);
            }
        }
        std::cout << x << " ";
    }

    {
        double x = 0;
        for (auto &[pair, a] : arr2) {
            size_t sum_a = std::accumulate(a.begin(), a.end(), 0ull);
            for (size_t i : a) {
                if (i == 0) {
                    continue;
                }
                double p = (1. * i) / sum_a;
                x += -(1. * sum_a) / size * p * log2(p);
            }
        }
        std::cout << x << " ";
    }
    std::cout << std::endl;
}