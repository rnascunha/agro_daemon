cmake_minimum_required (VERSION 3.10)
project (jason VERSION 1.0)

set(RAPIDJSON_ROOT 		../../../third/rapidjson/include)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include_directories(${RAPIDJSON_ROOT})

add_library(${PROJECT_NAME} jason.cpp types.cpp array.cpp object.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

if(DEFINED WITH_EXAMPLES)
	if(${WITH_EXAMPLES} EQUAL 1)
		add_executable(api_test examples/api_test.cpp)
		target_link_libraries(api_test PUBLIC ${PROJECT_NAME})
		
		add_executable(array_doc examples/array_doc.cpp)
		target_link_libraries(array_doc PUBLIC ${PROJECT_NAME})
	endif()
endif()
