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

find_package(SQLite3)
if(NOT SQLite3_FOUND)
# Didn't find SQLite3 packet. 
# We are going to have compile ourselves...

	message(STATUS "SQLite3 packet NOT found! Compiling...")
	
	set(SQLite3_ROOT third/)
	set(SQLite3_LIBRARIES sqlite3)
	
	add_library(${SQLite3_LIBRARIES} ${SQLite3_ROOT}/sqlite3.c)
	target_link_libraries(${SQLite3_LIBRARIES} PUBLIC 
											${CMAKE_DL_LIBS} 
											${CMAKE_THREAD_LIBS_INIT})
	
	set(SQLite3_INCLUDE_DIRS ${SQLite3_ROOT})
endif()

#include_directories(${SQLite3_INCLUDE_DIRS})
add_library(${PROJECT_NAME} sqlite3.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON)

target_link_libraries(${PROJECT_NAME} PUBLIC ${SQLite3_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC include ${SQLite3_INCLUDE_DIRS})


