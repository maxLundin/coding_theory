#include <gtest/gtest.h>
#include "coding.h"
#include <filesystem>
#include <random>

using namespace Compress;

namespace {
    std::filesystem::path get_path(std::string_view subpath) {
        auto dataset_path = std::filesystem::current_path().append("dataset");

        while (!std::filesystem::exists(dataset_path) && dataset_path != dataset_path.root_directory()) {
            dataset_path = dataset_path.parent_path().parent_path().append("dataset");
        }

        return dataset_path / subpath;
    }

    template<class Coder>
    void run_test(std::string_view path) {
        const auto dir = get_path(path);
        EXPECT_TRUE(std::filesystem::exists(dir));
        for (const auto &file: std::filesystem::recursive_directory_iterator(dir)) {
            std::ifstream in(file.path(), std::ios::binary | std::ios::in | std::ios::ate);
            const size_t in_size = in.tellg();
            in.seekg(0, std::ios::beg);
            in.clear();

            bytes input(in_size);
            in.read((char *) input.data(), input.size());
            const auto encoded = Coder::encode(input);
            const auto decoded = Coder::decode(encoded);
            EXPECT_EQ(decoded, input) << " on file: " << file;
        }
    }
}

TEST(RANS_ADAPTIVE, base) {
    run_test<RANS_ADAPTIVE>("base");
}

TEST(RANS_ADAPTIVE, calgarycorpus) {
    run_test<RANS_ADAPTIVE>("calgarycorpus");
}

TEST(RANS_ADAPTIVE, repeats) {
    run_test<RANS_ADAPTIVE>("repeats");
}

TEST(RANS_ADAPTIVE, jpeg30) {
    run_test<RANS_ADAPTIVE>("jpeg30");
}

TEST(RANS_ADAPTIVE, jpeg80) {
    run_test<RANS_ADAPTIVE>("jpeg80");
}

TEST(RANS_ADAPTIVE, fuzz) {
    bytes input(1ull << 17);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<byte> distrib;

    for (size_t _ = 0 ; _ < 100; ++_) {
        for (auto &ch : input) {
            ch = distrib(gen);
        }

        const auto encoded = RANS_ADAPTIVE::encode(input);
        const auto decoded = RANS_ADAPTIVE::decode(encoded);
        if (decoded != input) {
            std::ofstream out("in_fuzzer", std::ios::binary | std::ios::out);
            out.write(reinterpret_cast<const char *>(input.data()), input.size());
        }
        EXPECT_EQ(decoded, input) << " on file: " << "in_fuzzer";
    }

}