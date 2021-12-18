#include <map>
#include <cmath>

#include "entropy.h"

std::map<char, size_t> calc_letter2cnt(const std::string &text) {
    std::map<char, size_t> letter2cnt;
    for (char ch: text) {
        letter2cnt[ch]++;
    }

    return letter2cnt;
}

std::map<char, std::map<char, size_t>> calc_letter2letter2cnt(const std::string &text) {
    std::map<char, std::map<char, size_t>> letter2letter2cnt;
    bool has_prev = false;
    char prev = text[0];
    for (char ch: text) {
        if (!has_prev) {
            has_prev = true;
            continue;
        }

        letter2letter2cnt[prev][ch]++;
        prev = ch;
    }

    return letter2letter2cnt;
}

typedef std::pair<char, char> twogram;

std::map<twogram, size_t> calc_twogram2cnt(const std::string &text) {
    std::map<twogram, size_t> twogram2cnt;
    for (int i = 1; i < text.size(); i++) {
        twogram2cnt[{text[i - 1], text[i]}]++;
    }

    return twogram2cnt;
}

std::map<twogram, std::map<char, size_t>> calc_twogram2letter2cnt(const std::string &text) {
    std::map<twogram, std::map<char, size_t>> twogram2letter2cnt;
    bool has_prev = false;
    for (int i = 2; i < text.size(); i++) {
        twogram2letter2cnt[{text[i - 2], text[i - 1]}][text[i]]++;
    }

    return twogram2letter2cnt;
}

double log2(double x) {
    return std::log(x) / std::log(2);
}

double calc_entropy_hX(const std::string &text) {
    std::map<char, size_t> letter2cnt = calc_letter2cnt(text);

    double entropy = 0;
    for (auto &[l, cnt]: letter2cnt) {
        double p = (double) cnt / (double) text.size();
        entropy -= p * log2(p);
    }

    return entropy;
}

double calc_entropy_hXIX(const std::string &text) {
    // sum p(y) * (sum(p(x|y) * log(p(x|y)))
    auto letter2cnt = calc_letter2cnt(text);
    auto letter2letter2cnt = calc_letter2letter2cnt(text);

    double entropy = 0;
    for (auto &[letter1, cnt1]: letter2cnt) {
        double sum_p = 0;
        double py = (double) cnt1 / (double) text.size();
        for (auto &[letter2, cnt2]: letter2letter2cnt[letter1]) {
            double pxy = (double) cnt2 / (double) cnt1;
            sum_p += pxy * log2(pxy);
        }
        entropy -= py * sum_p;
    }

    return entropy;
}

double calc_entropy_hXIXX(const std::string &text) {
    // sum p(yz) * (sum(p(x|yz) * log(p(x|yz)))
    auto twogram2cnt = calc_twogram2cnt(text);
    auto twogram2letter2cnt = calc_twogram2letter2cnt(text);

    double entropy = 0;
    for (auto &[twogram, cnt1]: twogram2cnt) {
        double sum_p = 0;
        double py = (double) cnt1 / (double) text.size();
        for (auto &[letter, cnt2]: twogram2letter2cnt[twogram]) {
            double pxy = (double) cnt2 / (double) cnt1;
            sum_p += pxy * log2(pxy);
        }
        entropy -= py * sum_p;
    }

    return entropy;
}

double calc_entropy_hXX(const std::string &text) {
    return calc_entropy_hX(text) + calc_entropy_hXIX(text);
}
