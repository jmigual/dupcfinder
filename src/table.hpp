#ifndef TABLE_HPP
#define TABLE_HPP

#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace dupcfinder {

/**
 * @brief Table containing all the duplicates and also finding them.
 *
 */
class DuplicateTable {
public:
    /// @brief Files with the same size
    std::unordered_map<std::size_t, std::vector<std::filesystem::path>> duplicatesSize;

    /// @brief Files with the same first 1024 bytes
    std::unordered_map<std::string, std::vector<std::filesystem::path>> duplicatesFirst1024;

    /// @brief Files with the same hash
    std::unordered_map<std::string, std::vector<std::filesystem::path>> duplicatesHash;

    /// @brief Directories that will be searched
    std::vector<std::filesystem::path> searchPaths;

    explicit DuplicateTable(std::vector<std::filesystem::path> searchPaths) :
        searchPaths(std::move(searchPaths)) {}

    void find();
    [[nodiscard]] std::vector<std::filesystem::path> getDuplicates() const;

private:
    void findDuplicatesSize();
    void findDuplicatesFirst1024();
    void findDuplicatesHash();
};

} // namespace dupcfinder

#endif // TABLE_HPP