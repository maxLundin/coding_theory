#ifndef TEORCOD_BIT_VECTOR_H
#define TEORCOD_BIT_VECTOR_H

#include <vector>
#include <string>

class bit_vector {
public:
    bit_vector() = default;

    void push_code(std::string &str);

    void push(bool bit);

    void push(bit_vector &other);

    void push_back(bool bit);

    static bit_vector from_string(std::string &s, size_t len = -1);

    std::string to_string();

    std::string to_bit_string();

    bool operator[](int i);

    size_t size();

private:
    std::vector<char> content = {0};
    int pos_in_char = 0;
    int power_of_two = 1;
};

#endif //TEORCOD_BIT_VECTOR_H
