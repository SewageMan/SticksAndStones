cmake_minimum_required(VERSION 3.10)
project(MyGDExtension)

# Устанавливаем стандарт C++ (для IntelliSense)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем include-пути (твои папки + godot-cpp)
include_directories(
    src
    godot-cpp/include
    godot-cpp/gen/include
)

# Фиктивная цель, чтобы CMake не ругался, что нет исходников
# Можно оставить пустым, но некоторые версии VS требуют хотя бы один файл
add_executable(dummy dummy.cpp)