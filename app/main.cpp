
#include <fmt/format.h>
#include <table.hpp>
#include <utils/command_line.hpp>

int main(int argc, char *argv[]) {

    auto args = CommandLine::getArgs(argc, argv);

    dupcfinder::DuplicateTable table(args.directories);
    table.find();
    auto duplicates = table.getDuplicates();

    for (const auto &[hash, path] : duplicates) {
        fmt::print("{},{}\n", hash, path.string());
    }
}