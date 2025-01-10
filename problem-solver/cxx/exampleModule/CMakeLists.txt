file(GLOB SOURCES CONFIGURE_DEPENDS
        "*.cpp" "*.hpp"
        "agents/*.cpp" "agents/*.hpp"
        "keynodes/*.hpp"
        "structures/*.cpp" "structures/*.hpp"
        "searcher/*.cpp" "searcher/*.hpp"
        "utils/*.cpp" "utils/*.hpp")

add_library(example-module SHARED ${SOURCES})
target_link_libraries(example-module
    LINK_PRIVATE sc-machine::sc-memory
    LINK_PRIVATE sc-machine::sc-agents-common
)
target_include_directories(example-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(example-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()
