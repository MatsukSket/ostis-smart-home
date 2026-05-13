file(GLOB SOURCES CONFIGURE_DEPENDS
    "units/*.cpp"
    "units/*.hpp"
    "utils/*.cpp"
    "utils/*.hpp"
)

add_executable(smart-home-module-tests ${SOURCES})

target_link_libraries(smart-home-module-tests
    LINK_PRIVATE GTest::gtest_main
    LINK_PRIVATE smart-home-module
    LINK_PRIVATE sc-machine::sc-builder-lib
)
target_include_directories(smart-home-module-tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/units
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_compile_definitions(smart-home-module-tests
    PRIVATE TEST_FILES_DIR_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-structures/"
)
gtest_discover_tests(
    smart-home-module-tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/units
    DISCOVERY_TIMEOUT 30
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(smart-home-module-tests)
endif()
