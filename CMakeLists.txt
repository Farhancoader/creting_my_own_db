cmake_minimum_required(VERSION 3.20)

project(MiniDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(minidb
    src/main.cpp
    src/tokenizer.cpp
    src/parser.cpp
    src/executor.cpp
    src/table_manager.cpp
)

target_include_directories(minidb
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
