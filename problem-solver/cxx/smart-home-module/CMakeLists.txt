set(MODULE_NAME smart-home-module)

file(GLOB_RECURSE SOURCES
    "*.cpp"
    "agents/*.cpp"
    "keynodes/*.cpp"
)

add_library(${MODULE_NAME} SHARED ${SOURCES})

target_link_libraries(${MODULE_NAME}
    PRIVATE
        sc-machine::sc-memory
)

target_include_directories(${MODULE_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
