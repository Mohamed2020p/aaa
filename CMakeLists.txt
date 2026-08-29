cmake_minimum_required(VERSION 3.16)
project(AlienXMenu CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(alienx WIN32
    src/main.cpp
    src/app.cpp
    src/login.cpp
    src/pages.cpp
    src/ui.cpp
    src/theme.cpp
)

target_compile_definitions(alienx PRIVATE UNICODE _UNICODE NOMINMAX)

# gdiplus = the GDI+ drawing API, ole32 = COM (required by GDI+).
target_link_libraries(alienx PRIVATE gdiplus ole32)

if(MSVC)
    target_compile_options(alienx PRIVATE /W4 /utf-8)
else()
    # MinGW: link the unicode entry point (wWinMain), source is UTF-8.
    target_link_options(alienx PRIVATE -municode)
    target_compile_options(alienx PRIVATE -Wall -finput-charset=UTF-8)
endif()
