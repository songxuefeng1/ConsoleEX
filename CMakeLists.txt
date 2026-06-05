cmake_minimum_required(VERSION 3.15)
project(ConsoleEX VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(WIN32)
    set(PLATFORM_SPECIFIC_LIBS ws2_32)
else()
    set(PLATFORM_SPECIFIC_LIBS)
endif()

# === EISS DLL Library ===
set(EISS_SOURCES
    src/io/export_import.cpp
)

set(EISS_HEADERS
    include/io/export_import.hpp
)

add_library(EISS_api SHARED ${EISS_SOURCES} ${EISS_HEADERS})

target_include_directories(EISS_api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(EISS_api PRIVATE /W4 /WX)
    target_compile_definitions(EISS_api PRIVATE BUILD_DLL)
else()
    target_compile_options(EISS_api PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_definitions(EISS_api PRIVATE BUILD_DLL)
    target_link_options(EISS_api PRIVATE -static-libgcc -static-libstdc++)
endif()

# === Main ConsoleEX Executable ===
set(CONSOLE_SOURCES
    src/main.cpp
    src/core/exception_manager.cpp
    src/core/action_manager.cpp
    src/core/memory_manager.cpp
    src/io/editor.cpp
)

set(CONSOLE_HEADERS
    include/core/exception_manager.hpp
    include/core/action_manager.hpp
    include/core/memory_manager.hpp
    include/io/editor.hpp
)

add_executable(ConsoleEX ${CONSOLE_SOURCES} ${CONSOLE_HEADERS})

target_include_directories(ConsoleEX PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ConsoleEX PRIVATE EISS_api ${PLATFORM_SPECIFIC_LIBS})

if(MSVC)
    target_compile_options(ConsoleEX PRIVATE /W4 /WX)
else()
    target_compile_options(ConsoleEX PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# === Output directories ===
set_target_properties(ConsoleEX PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(EISS_api PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
