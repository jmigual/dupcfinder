
#### DEPENDENCIES ####
set(CPM_USE_LOCAL_PACKAGES ON)
CPMAddPackage(
    GITHUB_REPOSITORY jarro2783/cxxopts
    VERSION 3.0.0
    OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES"
    EXCLUDE_FROM_ALL
)
CPMAddPackage("gh:fmtlib/fmt#8.1.1")
CPMAddPackage("gh:nlohmann/json@3.11.1")
CPMAddPackage(
    NAME OpenSSL # <- use OpenSSL here
    GITHUB_REPOSITORY janbar/openssl-cmake
    GIT_TAG 1.1.1n-20220327
    OPTIONS "WITH_APPS OFF"
    EXCLUDE_FROM_ALL
)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
# remove_definitions("-DOPENSSL_NO_ASM")

# Create version file
set(VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/versioning.cpp")
add_custom_target(Version ALL
    COMMAND "${CMAKE_COMMAND}" -DVERSION_FILE_NAME="${VERSION_FILE}" -P "${PROJECT_SOURCE_DIR}/config/revision.cmake"
    BYPRODUCTS "${VERSION_FILE}"
)

# Find source files to compile
file(GLOB_RECURSE LIB_COMMON_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE LIB_COMMON_SOURCES CONFIGURE_DEPENDS "*.cpp")

# Define common library
add_library(common STATIC EXCLUDE_FROM_ALL ${LIB_COMMON_SOURCES} ${VERSION_FILE} ${LIB_COMMON_HEADERS})

target_include_directories(common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(common cxxopts fmt nlohmann_json::nlohmann_json OpenSSL::Crypto)
target_compile_features(common PUBLIC cxx_std_20)
set_target_properties(common PROPERTIES 
    INTERPROCEDURAL_OPTIMIZATION $<CONFIG:Release,RelWithDebInfo> # Enable LTO for release builds
)

# IDEs should put the headers in a nice place
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${LIB_COMMON_HEADERS}
)
