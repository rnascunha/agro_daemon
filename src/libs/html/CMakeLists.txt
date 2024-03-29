cmake_minimum_required (VERSION 3.10)
project (html_export)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(DIR_SRC		./)
set(MAIN_SRC	${DIR_SRC}/writer.cpp 
				${DIR_SRC}/fwriter.cpp 
				${DIR_SRC}/swriter.cpp)

add_library(${PROJECT_NAME}	${MAIN_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_include_directories(${PROJECT_NAME} PUBLIC ${DIR_SRC}/include)

if(WITH_EXAMPLES)
	set(EXAMPLE_DIR		examples)
	set(EXAMPLE_LIST	html_test html_file html_str)
	foreach(example ${EXAMPLE_LIST})
		add_executable(${example} ${EXAMPLE_DIR}/${example}.cpp)
		target_link_libraries(${example} ${PROJECT_NAME})
	endforeach()
endif()
