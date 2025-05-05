cmake_minimum_required(VERSION 3.10)
project(autoshop_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим необходимые пакеты
find_package(SQLite3 REQUIRED)

# Специальная настройка для GTest на macOS
if(APPLE)
    set(GTEST_ROOT "/opt/homebrew/")
    set(GTEST_INCLUDE_DIR "/opt/homebrew/include")
    set(GTEST_LIBRARY "/opt/homebrew/lib/libgtest.dylib")
    set(GTEST_MAIN_LIBRARY "/opt/homebrew/lib/libgtest_main.dylib")
endif()

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Добавляем директорию с заголовочными файлами
include_directories(${CMAKE_SOURCE_DIR}/includes)

# Основной исполняемый файл
add_executable(autoshop_app 
    src/main.cpp
    src/db_utils.cpp
    src/image_utils.cpp
    src/auth.cpp
    src/main_utils.cpp
)

# Тестовый исполняемый файл
add_executable(autoshop_tests
    tests/test_db_utils.cpp
    tests/test_image_utils.cpp
    tests/test_main.cpp
    src/db_utils.cpp
    src/image_utils.cpp
    src/auth.cpp
    src/main_utils.cpp
)

# Линкуем библиотеки для основного приложения
target_link_libraries(autoshop_app
    PRIVATE
    SQLite::SQLite3
)

# Линкуем библиотеки для тестов
target_link_libraries(autoshop_tests
    PRIVATE
    SQLite::SQLite3
    GTest::GTest
    GTest::Main
    Threads::Threads
)

# Включаем тестирование
enable_testing()
add_test(NAME autoshop_tests COMMAND autoshop_tests)

# Устанавливаем выходные директории
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Добавляем флаги компиляции для тестов
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(autoshop_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Добавляем флаги для покрытия кода тестами
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(autoshop_tests PRIVATE --coverage)
        target_link_options(autoshop_tests PRIVATE --coverage)
    endif()
endif()
