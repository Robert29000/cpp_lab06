cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_COVERAGE "Build coverage" OFF)

set(
  HUNTER_CACHE_SERVERS
  "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
  CACHE STRING "Default cache server"
)

include("tools/gate/cmake/HunterGate.cmake")

HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.278.tar.gz"
  SHA1 "35fa55bc1dfcf9f4d7264f8bb26cd1759ce1ae07"
)

# TODO: rename project and delete this comment
project(find_hash)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

hunter_add_package(GTest)
hunter_add_package(nlohmann_json)
hunter_add_package(Boost COMPONENTS log thread filesystem date_time)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Boost CONFIG REQUIRED log thread filesystem date_time)


# TODO: change template word to project name and delete this comment
add_library(hash_checker STATIC
  # enum your files and delete this comment
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/hash_checker.cpp
)

add_executable(solve_hash
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/main.cpp
        )

add_executable(tests
  # TODO: enum your files and delete this comment
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
)

# TODO: change template word to project name and delete this comment
target_include_directories(hash_checker
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(tests
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(solve_hash
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
# TODO: change template word to project name and delete this comment
target_link_libraries(tests GTest::gtest_main hash_checker)
target_link_libraries(solve_hash nlohmann_json::nlohmann_json
        Boost::log Boost::thread Boost::filesystem Boost::date_time hash_checker
        )

enable_testing()
add_test(NAME unit_tests COMMAND tests)

if(BUILD_COVERAGE)
  function(ENABLE_PROJECT_COVERAGE PROJECT)
    target_compile_options(${PROJECT} PRIVATE --coverage)
    target_link_libraries(${PROJECT} --coverage)
    add_coverage(${PROJECT})
  endfunction()

  set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  find_package(codecov)
  # TODO: change template word to project name and delete this comment
  enable_project_coverage(hash_checker)
  enable_project_coverage(tests)
  list(APPEND LCOV_REMOVE_PATTERNS "'${PROJECT_SOURCE_DIR}/tests/*'")
  coverage_evaluate()
endif()
