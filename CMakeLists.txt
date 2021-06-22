cmake_minimum_required (VERSION 3.10)
project (agro_daemon)

function(print_vars)	
	message("USE_SSL: " ${USE_SSL})
	message("BOOST_ROOT: " ${BOOST_ROOT})
	message("RAPIDJSON_ROOT: " ${RAPIDJSON_ROOT})
	message("ARGH_ROOT: " ${ARGH_ROOT})
	message("OPENSSL_ROOT: " ${OPENSSL_ROOT})
	message("KEEPALIVE_OLD: " ${KEEPALIVE_OLD})
endfunction()

#------------DEFINING VARS-----------------

set(BOOST_ROOT 			/home/rnascunha/toolchain/libs/boost_1_76_0/include)
set(RAPIDJSON_ROOT 		third/rapidjson/include)
set(ARGH_ROOT 			third/argh)
set(USE_SSL				0)

#print_vars()

if(DEFINED WITH_SSL)
	if(${WITH_SSL} EQUAL 1)
		set(USE_SSL 1)
	endif()
	unset(WITH_SSL CACHE)
endif()

#
# PATHs
#

if(DEFINED BOOST_PATH)
	set(BOOST_ROOT ${BOOST_PATH})
	unset(BOOST_PATH CACHE)
endif()

if(DEFINED RAPIDJSON_PATH)
	set(RAPIDJSON_ROOT ${RAPIDJSON_PATH})
	unset(RAPIDJSON_PATH CACHE)
endif()

if(DEFINED ARGH_PATH)
	set(ARGH_ROOT ${ARGH_PATH})
	unset(ARGH_ROOT CACHE)
endif()

if(DEFINED OPENSSL_PATH)
	set(OPENSSL_ROOT_DIR ${OPENSSL_PATH})
	unset(OPENSSL_PATH CACHE)
endif()

#This flag works only on Windows OS. There is 2 implementations (that I call OLD or NEW).
#You can force the implementation with the -DCONFIG_KEEPALIVE_OLD=<0|1>, but if not defined MSVC will 
#use the NEW implementation, otherwise the OLD. This was defined by some tests (and no better
#defintion)
if(DEFINED CONFIG_KEEPALIVE_OLD)
	set(KEEPALIVE_OLD ${CONFIG_KEEPALIVE_OLD})
else()
	if(MSVC)
		set(KEEPALIVE_OLD 0)
	else()
		set(KEEPALIVE_OLD 1)
	endif()
endif()
unset(CONFIG_KEEPALIVE_OLD CACHE)

#print_vars()

#------------END DEFINING VARS-----------------

message(STATUS "Building OS: " ${CMAKE_SYSTEM_NAME})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

message("Builder type: " ${CMAKE_BUILD_TYPE}) 

if(MSVC)
	message(STATUS "MSVC build")
	set(CMAKE_CXX_FLAGS "/W4 /std:c++17 /EHsc /bigobj")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	add_compile_definitions(BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)
else()
	message(STATUS "NO MSVC build")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")	#-fmax-errors=5 
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if(WIN32)
    macro(get_WIN32_WINNT version)
        if(CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif()
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
        message("WIN32: " ${ver})
endif()

message(STATUS "Compiler: " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION})

add_definitions(-DBOOST_SYSTEM_USE_UTF8		#Make windows error output UTF8 / Ignored at POSIX
				-DJSON_MESSAGES 
				-DUSE_SSL=${USE_SSL}
				-DKEEPALIVE_OLD=${KEEPALIVE_OLD})

find_package(Boost COMPONENTS REQUIRED)

if(Boost_FOUND)
	message(STATUS "Boost Library found! " ${Boost_INCLUDE_DIRS})
#	include_directories(${Boost_INCLUDE_DIRS})
	include_directories(${BOOST_ROOT})
else()
	message(FATAL_ERROR "Boost Library NOT found! Define variable BOOST_ROOT (and BOOST_INCLUDEDIR if necessary")
endif()

if(USE_SSL)
	find_package(OpenSSL REQUIRED)
	if(OPENSSL_FOUND)
		message(STATUS "OpenSSL Libraty found!")
		include_directories(${OPENSSL_INCLUDE_DIR})
	else()
		message(FATAL_ERROR "OpenSSL Libraty NOT found! Define variable_requires OPENSSL_ROOT_DIR, or compile WITHOUT SSL support")
	endif()
else()
	message(STATUS "Compiling WITHOUT SSL support")
endif()

set(SRC_DIR ./src)

set(MY_LIBS_DIR	${SRC_DIR}/libs)

#include_directories(${RAPIDJSON_ROOT}
#					${MY_LIBS_DIR}/byte_array/include 
#					${MY_LIBS_DIR}/my_async/include
#					${MY_LIBS_DIR}/utility/include
#					${MY_LIBS_DIR}/system_info/include
#					${SRC_DIR}/helpers/include
#					${SRC_DIR}/core/include
#					${ARGH_ROOT})

###########################
# tree trunks
###########################

include_directories(third/tree_trunks/src/include/)

#########################
# CoAP-te 
#########################
add_definitions(-DCOAP_TE_PORT_C_STANDARD=1
				-DCOAP_TE_PORT_POSIX=1)

#Command line definitions
add_definitions(-DCOAP_TE_LOG_COLOR=1
				-DCOAP_TE_LOG_LEVEL=5
				-DCOAP_TE_USE_ERROR_MESSAGES=1)

add_subdirectory(third/coap-te)
include_directories(third/coap-te/src)		

###########################
# My async
###########################

add_subdirectory(third/my_async/src)
include_directories(third/my_async/include)

###########################

if(${USE_SSL} EQUAL 1)
	message("Enable SSL support")
endif()

set(MAIN_SRC ${SRC_DIR}/main.cpp)

if(USE_SSL)
	list(APPEND MAIN_SRC ${SRC_DIR}/helpers/load_certificate.cpp)
endif()

add_executable(${PROJECT_NAME} ${MAIN_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

if(USE_SSL)
	target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
endif()

target_link_libraries(${PROJECT_NAME} coap-te)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
else()
	target_link_libraries(${PROJECT_NAME} pthread)
endif()
