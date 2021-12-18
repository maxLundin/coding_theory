cmake_minimum_required(VERSION 3.16.3)


set(CMAKE_CXX_STANDARD 20)


add_library(huffman_coder
        encoder/encoder.cpp encoder/encoder.h
        decoder/decoder.cpp decoder/decoder.h
        bit_vector.cpp bit_vector.h
        huffman_tree.cpp huffman_tree.h
        preprocess.cpp preprocess.h
        utils.cpp utils.h
        entropy.cpp entropy.h)
