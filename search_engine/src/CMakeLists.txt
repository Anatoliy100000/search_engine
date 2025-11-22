# src/CMakeLists.txt

# Path to headers
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(search_engine_lib
    ConverterJSON.cpp
    InvertedIndex.cpp
    SearchServer.cpp
)

target_include_directories(search_engine_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(search_engine_app
    main.cpp
)

target_link_libraries(search_engine_app
    PRIVATE
        search_engine_lib
)
