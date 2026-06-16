cmake_minimum_required(VERSION 4.2)
project(minesweeper)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories.
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(third_party/SDL EXCLUDE_FROM_ALL)

add_executable(minesweeper WIN32
        main.cpp
        minefield.cpp
        minefield.h
        rng.h
)

target_compile_definitions(minesweeper PRIVATE NOMINMAX)

target_link_libraries(minesweeper PRIVATE SDL3::SDL3)

add_custom_command(TARGET minesweeper POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:minesweeper>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/settings.ini"
        "$<TARGET_FILE_DIR:minesweeper>/settings.ini"
)