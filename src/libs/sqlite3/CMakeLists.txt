cmake_minimum_required (VERSION 3.10)
project (sqlite3_wrapper VERSION 1.0)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

find_package(SQLite3 REQUIRED)

add_library(${PROJECT_NAME} sqlite3.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON)

target_link_libraries(${PROJECT_NAME} PUBLIC SQLite::SQLite3)
target_include_directories(${PROJECT_NAME} PUBLIC include)


