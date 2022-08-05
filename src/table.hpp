#ifndef TABLE_HPP
#define TABLE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utils/sha.hpp>
#include <vector>

namespace dupcfinder {

/**
 * @brief Table containing all the duplicates and also finding them.
 *
 */
class DuplicateTable {
public:
    /// @brief Files with the same size
    std::unordered_map<std::size_t, std::vector<std::filesystem::path>> m_duplicatesSize;

    /// @brief Files with the same first 1024 bytes
    std::unordered_map<Hash, std::vector<std::filesystem::path>> m_duplicatesFirst1024;

    /// @brief Files with the same hash
    std::unordered_map<Hash, std::vector<std::filesystem::path>> m_duplicatesHash;

    /// @brief Directories that will be searched
    std::vector<std::filesystem::path> m_searchPaths;

    explicit DuplicateTable(std::vector<std::filesystem::path> searchPaths) :
        m_searchPaths(std::move(searchPaths)) {}

    /// @brief Start the search for duplicates
    void find();

    /// @brief Get the duplicates found
    [[nodiscard]] std::vector<std::tuple<std::string, std::filesystem::path>> getDuplicates() const;

private:
    /// @brief Mutex for parallel execution
    std::mutex m_mutex;

    void findDuplicatesSize();
    void findDuplicatesFirst1024();
    void findDuplicatesHash();
};

} // namespace dupcfinder

#endif // TABLE_HPP