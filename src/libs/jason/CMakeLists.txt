cmake_minimum_required (VERSION 3.10)
project (jason VERSION 1.0)

option(WITH_EXAMPLES "compile jason examples" OFF)
option(WITH_EXAMPLES "run jason tests" OFF)

set(RAPIDJSON_ROOT 		../../../third/rapidjson/include)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include_directories(${RAPIDJSON_ROOT})

add_library(${PROJECT_NAME} jason.cpp types.cpp array.cpp object.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
 

if(WITH_EXAMPLES)
	set(EXAMPLES api_test basic_types array_parse object_doc array_doc)
	
	foreach(example ${EXAMPLES})
		add_executable(${example} examples/${example}.cpp)
		target_link_libraries(${example} PUBLIC ${PROJECT_NAME})
		set_target_properties(${example} PROPERTIES
		    CXX_STANDARD 17
		    CXX_STANDARD_REQUIRED ON
		    CXX_EXTENSIONS OFF)
	endforeach()
endif()

if(WITH_TESTS)
	add_subdirectory(tests)
endif()
