# tests/CMakeLists.txt

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_disable_pthreads ON)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(search_engine_tests
    tests.cpp
)

target_link_libraries(search_engine_tests
    PRIVATE
        search_engine_lib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(search_engine_tests)
