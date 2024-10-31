file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library(example-module SHARED ${SOURCES})
target_link_libraries(example-module
    LINK_PRIVATE sc-machine::sc-memory
    LINK_PRIVATE sc-machine::sc-agents-common
)
target_include_directories(example-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
