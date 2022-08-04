#include "command_line.hpp"

#include "versioning.hpp"

#include <cxxopts.hpp>
#include <fmt/ostream.h>
#include <iostream>

namespace {
/* print the usage of the tool */
void printUsage(const cxxopts::Options &options) {
    fmt::print(std::cout, "{}\nVersion information: {}\n", options.help(), VERSION);
}
} // namespace

CommandLine::Args CommandLine::getArgs(int argc, char **argv) noexcept {

    cxxopts::Options options("dupcfinder", "A fast duplicate files finder");

    // clang-format off
    options.add_options()
        ("h,help", "Show this help")
        ("directories", "The directories to search for duplicates", 
            cxxopts::value<std::vector<std::string>>());
    // clang-format on
    options.parse_positional({"directories"});

    Args args;

    try {
        auto result = options.parse(argc, argv);

        if (result.count("help") > 0) {
            printUsage(options);
            std::exit(EXIT_SUCCESS);
        }

        if (result.count("directories") <= 0) {
            fmt::print(std::cerr, "Error: directories are required\n");
            printUsage(options);
            std::exit(EXIT_FAILURE);
        }

        for (const auto &directory : result["directories"].as<std::vector<std::string>>()) {
            args.directories.emplace_back(directory);
        }
    } catch (const cxxopts::OptionException &e) {
        std::cerr << "Error parsing arguments:" << std::endl
                  << e.what() << std::endl
                  << "Submitted parameters:" << std::endl;

        for (size_t i = 0; i < argc; ++i) {
            std::cerr << "'" << argv[i] << "' ";
        }
        std::cerr << std::endl;

        printUsage(options);
        std::exit(EXIT_FAILURE);
    }
    return args;
}
