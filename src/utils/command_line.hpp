#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

#include <filesystem>
#include <vector>

class CommandLine {
public:
    struct Args {
        std::vector<std::filesystem::path> directories;
    };

    static Args getArgs(int argc, char **argv) noexcept;
};

#endif // COMMAND_LINE_HPP