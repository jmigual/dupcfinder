#include "sha.hpp"

#include <fstream>
#include <vector>

constexpr std::size_t kMaxReadSize = 1024 * 1024;

dupcfinder::Hash dupcfinder::hash(const void *input, std::size_t size) {
    SHA512_CTX context;

    if (SHA512_Init(&context) != 1) {
        throw std::runtime_error("SHA512_Init failed");
    }

    if (SHA512_Update(&context, input, size) != 1) {
        throw std::runtime_error("SHA512_Update failed");
    }

    dupcfinder::Hash hash;
    if (SHA512_Final(hash.data(), &context) != 1) {
        throw std::runtime_error("SHA512_Final failed");
    }

    return hash;
}

dupcfinder::Hash dupcfinder::hash(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    SHA512_CTX context;

    if (SHA512_Init(&context) != 1) {
        throw std::runtime_error("SHA512_Init failed");
    }

    std::vector<char> buffer(kMaxReadSize);

    while (file.good() && file.read(buffer.data(), kMaxReadSize).good()) {
        if (SHA512_Update(&context, buffer.data(), file.gcount()) != 1) {
            throw std::runtime_error("SHA512_Update failed");
        }
    }

    dupcfinder::Hash hash;
    if (SHA512_Final(hash.data(), &context) != 1) {
        throw std::runtime_error("SHA512_Final failed");
    }

    return hash;
}
