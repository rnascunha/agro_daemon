cmake_minimum_required (VERSION 3.10)
project (smtp VERSION 1.0)

if(MSVC)
	message(STATUS "MSVC build")
	set(CMAKE_CXX_FLAGS "/W4 /std:c++17 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	add_compile_definitions(BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)
else()
	message(STATUS "NO MSVC build")
	set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wno-unused-parameter")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

find_package(Threads REQUIRED)

find_package(Boost COMPONENTS REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
endif()

add_library(${PROJECT_NAME} client.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}
										OpenSSL::SSL
										${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} INTERFACE include)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

if(DEFINED WITH_EXAMPLES)
	if(${WITH_EXAMPLES} EQUAL 1)
		add_executable(smtp_client examples/smtp_client.cpp)
		target_link_libraries(smtp_client PUBLIC ${PROJECT_NAME})
	endif()
endif()
