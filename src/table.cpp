#include "table.hpp"

#include <execution>
#include <fmt/compile.h>
#include <fmt/ostream.h>
#include <fstream>
#include <libs/tqdm.hpp>

static const std::size_t kMaxReadSize = 1024;

std::vector<std::tuple<std::string, std::filesystem::path>>
dupcfinder::DuplicateTable::getDuplicates() const {
    std::vector<std::tuple<std::string, std::filesystem::path>> duplicates;
    for (const auto &[hash, paths] : m_duplicatesHash) {
        for (const auto &path : paths) {
            // Convert bytes to hex string
            duplicates.emplace_back(fmt::format("{:02x}", fmt::join(hash, "")), path);
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
    for (const auto &path : m_searchPaths) {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                auto size = entry.file_size();
                m_duplicatesSize[size].emplace_back(entry.path());
            }
        }
    }

    // Filter out entries with 1 element
    for (auto it = m_duplicatesSize.begin(); it != m_duplicatesSize.end();) {
        if (it->second.size() == 1) {
            it = m_duplicatesSize.erase(it);
        } else {
            ++it;
        }
    }

    fmt::print(std::cerr, "Found {} possible duplicates\n", m_duplicatesSize.size());
}

void dupcfinder::DuplicateTable::findDuplicatesFirst1024() {
    fmt::print(std::cerr, "Finding duplicates by first 1024 bytes...\n");
    auto bar = tq::tqdm(m_duplicatesSize);
    bar.set_prefix("Filtering ");
    for (const auto &[size, paths] : bar) {
        for (const auto &path : paths) {
            std::ifstream file(path, std::ios::binary | std::ios::in);
            std::string first1024;
            first1024.resize(kMaxReadSize);
            file.read(first1024.data(), kMaxReadSize);
            m_duplicatesFirst1024[dupcfinder::hash(first1024)].emplace_back(path);
            file.close();
        }
    }

    // Remove entries with 1 element
    for (auto it = m_duplicatesFirst1024.begin(); it != m_duplicatesFirst1024.end();) {
        if (it->second.size() == 1) {
            it = m_duplicatesFirst1024.erase(it);
        } else {
            ++it;
        }
    }

    fmt::print(std::cerr, "\nFound {} possible duplicates\n", m_duplicatesFirst1024.size());
}

void dupcfinder::DuplicateTable::findDuplicatesHash() {
    fmt::print(std::cerr, "Finding duplicates by hashes...\n");

    std::vector<std::filesystem::path> pathsToDo;
    for (const auto &[first1024, paths] : m_duplicatesFirst1024) {
        for (const auto &path : paths) {
            pathsToDo.emplace_back(path);
        }
    }

    auto bar = tq::tqdm(pathsToDo);
    bar.set_prefix("Filtering ");
    auto iter = bar.begin();

    std::for_each(std::execution::par_unseq,
                  pathsToDo.begin(),
                  pathsToDo.end(),
                  [this, &iter, &bar](const auto &path) {
                      auto hash = dupcfinder::hash(path);
                      m_mutex.lock();
                      m_duplicatesHash[hash].emplace_back(path);
                      ++iter;
                      bar.update();
                      m_mutex.unlock();
                  });

    // Remove entries with 1 element
    for (auto it = m_duplicatesHash.begin(); it != m_duplicatesHash.end();) {
        if (it->second.size() == 1) {
            it = m_duplicatesHash.erase(it);
        } else {
            ++it;
        }
    }

    fmt::print(std::cerr, "\nFound {} duplicates\n", m_duplicatesHash.size());
}
