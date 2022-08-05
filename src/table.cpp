#include "table.hpp"

#include <fmt/ostream.h>
#include <libs/sha512.hpp>
#include <libs/tqdm.hpp>

static const std::size_t kMaxReadSize = 1024;

std::vector<std::tuple<std::string, std::filesystem::path>>
dupcfinder::DuplicateTable::getDuplicates() const {
    std::vector<std::tuple<std::string, std::filesystem::path>> duplicates;
    for (const auto &[hash, paths] : duplicatesHash) {
        for (const auto &path : paths) {
            duplicates.emplace_back(hash, path);
        }
    }
    return duplicates;
}

void dupcfinder::DuplicateTable::find() {
    findDuplicatesSize();
    findDuplicatesFirst1024();
    findDuplicatesHash();
}

void dupcfinder::DuplicateTable::findDuplicatesSize() {
    fmt::print(std::cerr, "Finding duplicates by size...\n");
    for (const auto &path : searchPaths) {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                auto size = entry.file_size();
                duplicatesSize[size].emplace_back(entry.path());
            }
        }
    }

    // Filter out entries with 1 element
    for (auto it = duplicatesSize.begin(); it != duplicatesSize.end();) {
        if (it->second.size() == 1) {
            it = duplicatesSize.erase(it);
        } else {
            ++it;
        }
    }

    fmt::print(std::cerr, "Found {} possible duplicates\n", duplicatesSize.size());
}

void dupcfinder::DuplicateTable::findDuplicatesFirst1024() {
    fmt::print(std::cerr, "Finding duplicates by first 1024 bytes...\n");
    auto bar = tq::tqdm(duplicatesSize);
    bar.set_prefix("Filtering ");
    for (const auto &[size, paths] : bar) {
        for (const auto &path : paths) {
            std::ifstream file(path, std::ios::binary | std::ios::in);
            std::string first1024;
            first1024.resize(kMaxReadSize);
            file.read(first1024.data(), kMaxReadSize);
            duplicatesFirst1024[sw::sha512::calculate(first1024)].emplace_back(path);
        }
    }

    // Remove entries with 1 element
    for (auto it = duplicatesFirst1024.begin(); it != duplicatesFirst1024.end();) {
        if (it->second.size() == 1) {
            it = duplicatesFirst1024.erase(it);
        } else {
            ++it;
        }
    }

    fmt::print(std::cerr, "Found {} possible duplicates\n", duplicatesFirst1024.size());
}

void dupcfinder::DuplicateTable::findDuplicatesHash() {
    fmt::print(std::cerr, "Finding duplicates by hash...\n");
    auto bar = tq::tqdm(duplicatesFirst1024);
    bar.set_prefix("Filtering ");
    for (const auto &[first1024, paths] : bar) {
        for (const auto &path : paths) {
            auto hash = sw::sha512::file(path.string());
            duplicatesHash[hash].emplace_back(path);
        }
    }

    // Remove entries with 1 element
    for (auto it = duplicatesHash.begin(); it != duplicatesHash.end();) {
        if (it->second.size() == 1) {
            it = duplicatesHash.erase(it);
        } else {
            ++it;
        }
    }

    fmt::print(std::cerr, "Found {} duplicates\n", duplicatesHash.size());
}
