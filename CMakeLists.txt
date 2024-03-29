cmake_minimum_required (VERSION 3.10)
project (agro_daemon VERSION 0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

option(WITH_SSL "compile SSL support" FALSE)
set(USE_SSL 0)
if(WITH_SSL)
	set(USE_SSL 1)
endif()

set(TT_LEVEL			type::status)
if(DEFINED VERBOSE)
	if(VERBOSE EQUAL 0)
		set(TT_LEVEL type::none)
	elseif(VERBOSE EQUAL 1)
		set(TT_LEVEL type::error)
	elseif(VERBOSE EQUAL 2)
		set(TT_LEVEL type::warning)
	elseif(VERBOSE EQUAL 3)
		set(TT_LEVEL type::deprecated)
	elseif(VERBOSE EQUAL 4)
		set(TT_LEVEL type::status)
	elseif(VERBOSE EQUAL 5)
		set(TT_LEVEL type::debug)
	endif()
else()
	set(VERBOSE		4)
endif()

set(AGRO_DB_SCHEME		"${PROJECT_SOURCE_DIR}/db/scheme.sql")
if (DEFINED AGRO_DB_SCHEME_FILE)
set(AGRO_DB_SCHEME		${AGRO_DB_SCHEME_FILE})
endif()

#------------END DEFINING VARS-----------------
if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	add_compile_definitions(BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -Werror")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

add_definitions(#-DBOOST_SYSTEM_USE_UTF8		#Make windows error output UTF8 / Ignored at POSIX
				-DUSE_SSL=${USE_SSL}
				-DTT_LEVEL=${TT_LEVEL})
				
string(TIMESTAMP TIME_NOW "%Y/%m/%d %H:%M:%SZ" UTC)
add_definitions(-DCOMPILE_TIME=${TIME_NOW})

# Required packages

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS SSL)
find_package(argh REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(tree_trunks REQUIRED)
find_package(coap-te REQUIRED)
find_package(pusha REQUIRED)

# Creating version file
file(READ ${AGRO_DB_SCHEME} AGRO_DB_SCHEME_CONTENT)
configure_file("${CMAKE_SOURCE_DIR}/cmake/version.hpp.in" "${CMAKE_SOURCE_DIR}/src/version.hpp" @ONLY)

# Definitions
add_definitions(
				-DCOAP_TE_PORT_C_STANDARD=1
				-DCOAP_TE_PORT_POSIX=1
				-DCOAP_TE_LOG_COLOR=1
				-DCOAP_TE_LOG_LEVEL=${VERBOSE}
				-DCOAP_TE_USE_ERROR_MESSAGES=1
				-DCOAP_TE_JSON_HELPER=1)

set(SRC_DIR ./src)

############################
# internal libs
############################
set(LIBS_DIR		${SRC_DIR}/libs)

add_subdirectory(${LIBS_DIR}/sqlite3)
add_subdirectory(${LIBS_DIR}/csv)
add_subdirectory(${LIBS_DIR}/smtp)

###########################
# Adding files 
###########################
include(${SRC_DIR}/files.cmake)
			
if(${USE_SSL})
	list(APPEND MAIN_SRC ${SRC_DIR}/websocket/load_certificate.cpp)
endif()

add_executable(${PROJECT_NAME} ${MAIN_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(${PROJECT_NAME} Boost::boost
										sqlite3_wrapper
										OpenSSL::SSL
										coap-te::coap-te
										pusha::pusha
										tree_trunks::tree_trunks
										csv_export
										smtp
										argh::argh
										${CMAKE_THREAD_LIBS_INIT})

target_include_directories(${PROJECT_NAME} PRIVATE ${RapidJSON_INCLUDE_DIR})

if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

# tools
include(tools/tools.cmake)

# Installing
include(cmake/install.cmake)

# Packing 
include(cmake/package.cmake)
