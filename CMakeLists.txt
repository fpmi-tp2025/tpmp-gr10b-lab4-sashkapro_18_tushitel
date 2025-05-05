cmake_minimum_required(VERSION 3.10)
project(autoshop_app)

set(CMAKE_CXX_STANDARD 17)

# Включаем поддержку файловой системы только для Linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
endif()

include_directories(includes)
file(GLOB SOURCES "src/*.cpp")

add_executable(autoshop_app ${SOURCES})

# Подключаем SQLite
find_package(SQLite3 REQUIRED)
target_link_libraries(autoshop_app SQLite::SQLite3)

# Создаем директории для сборки
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/images)

# Копируем базу данных в директорию сборки
configure_file(${CMAKE_SOURCE_DIR}/autoshops.db ${CMAKE_BINARY_DIR}/bin/autoshops.db COPYONLY)
