file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

add_library(smart-home-module SHARED ${SOURCES})
target_link_libraries(smart-home-module
    LINK_PUBLIC sc-machine::sc-memory
)
target_include_directories(smart-home-module
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(smart-home-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(smart-home-module)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(test)
endif()
