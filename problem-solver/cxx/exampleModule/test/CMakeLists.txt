file(GLOB TEST_UTILS_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/utils/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/utils/*.hpp")

add_library(example-module-test-utils STATIC ${TEST_UTILS_SOURCES})
target_link_libraries(example-module-test-utils
    example-module
    sc-machine::sc-memory
    sc-machine::sc-agents-common
    gtest)

target_include_directories(example-module-test-utils
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${EXAMPLE_MODULE_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(example-module-test-utils)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME example-module-test-starter
    DEPENDS sc-machine::sc-builder-lib example-module-test-utils
    INCLUDES ${EXAMPLE_MODULE_SRC} ${CMAKE_CURRENT_LIST_DIR})

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(example-module-test-starter)
endif()
