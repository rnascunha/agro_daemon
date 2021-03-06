cmake_minimum_required (VERSION 3.10)
project (csv_export)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /std:c++17 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++17")	#-fmax-errors=5 
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(DIR_SRC		./)
set(MAIN_SRC	${DIR_SRC}/writer.cpp ${DIR_SRC}/fwriter.cpp ${DIR_SRC}/swriter.cpp)

include_directories(${DIR_SRC}/include)

add_library(${PROJECT_NAME}	${MAIN_SRC})
if(DEFINED WITH_EXAMPLES)
	if(${WITH_EXAMPLES} EQUAL 1)
		set(EXAMPLE_DIR	examples)
		set(EXAMPLE_LIST	main_csv file_csv string_csv)
		foreach(example ${EXAMPLE_LIST})
			add_executable(${example} ${EXAMPLE_DIR}/${example}.cpp)
			target_link_libraries(${example} ${PROJECT_NAME})
		endforeach()
	endif()
endif()
