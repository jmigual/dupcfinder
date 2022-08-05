execute_process(COMMAND git rev-parse --abbrev-ref HEAD
                OUTPUT_VARIABLE GIT_BRANCH
                RESULT_VARIABLE GIT_OUTPUT
                ERROR_QUIET)

if (GIT_OUTPUT EQUAL 0)
execute_process(COMMAND git rev-list HEAD --count
                OUTPUT_VARIABLE GIT_COUNT
                ERROR_QUIET)

execute_process(COMMAND git log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)
execute_process(COMMAND git log -1 --format=%cd
                OUTPUT_VARIABLE GIT_DATE
                ERROR_QUIET)

string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
string(STRIP "${GIT_COUNT}" GIT_COUNT)
string(STRIP "${GIT_REV}" GIT_REV)
string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
string(STRIP "${GIT_DATE}" GIT_DATE)

set(VERSION 
"#include \"versioning.hpp\"

const char* const VERSION = \"${GIT_BRANCH}-${GIT_COUNT}-${GIT_REV} Commited at ${GIT_DATE}\";
")
else()
message("Version set from project version")
set(VERSION 
"#include \"versioning.hpp\"

const char* const VERSION = \"${PROJECT_VERSION}\";
")
endif()

if(EXISTS ${VERSION_FILE_NAME})
    file(READ ${VERSION_FILE_NAME} VERSION_)
else()
    set(VERSION_ "")
endif()

if (NOT "${VERSION}" STREQUAL "${VERSION_}")
    file(WRITE ${VERSION_FILE_NAME} "${VERSION}")
endif()

