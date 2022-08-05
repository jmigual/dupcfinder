#include <array>
#include <filesystem>
#include <openssl/sha.h>

namespace dupcfinder {
using Hash = std::array<std::uint8_t, SHA512_DIGEST_LENGTH>;
} // namespace dupcfinder

template <> struct std::hash<dupcfinder::Hash> {
    std::size_t operator()(const dupcfinder::Hash &data) const noexcept {
        std::size_t seed = 0;
        for (const auto &byte : data) {
            seed ^= byte + 0x9e3779b9 + (seed << 6) + (seed >> 2); // NOLINT
        }
        return seed;
    }
};

namespace dupcfinder {

/// @brief Hash some data
Hash hash(const void *input, std::size_t size);

inline Hash hash(const std::string &input) { return hash(input.data(), input.size()); }

/// @brief Hash a file
Hash hash(const std::filesystem::path &path);

} // namespace dupcfinder
