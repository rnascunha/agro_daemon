cmake_minimum_required (VERSION 3.10)
project (agro_daemon VERSION 1.0)

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
set(TT_LEVEL			type::debug)

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
  unset(VERBOSE CACHE)
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
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")
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

message(STATUS "Compiler: " ${CMAKE_C_COMPILER} " | " ${CMAKE_CXX_COMPILER} " " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION})

add_definitions(-DBOOST_SYSTEM_USE_UTF8		#Make windows error output UTF8 / Ignored at POSIX
				-DJSON_MESSAGES 
				-DUSE_SSL=${USE_SSL}
				-DKEEPALIVE_OLD=${KEEPALIVE_OLD}
				-DTT_LEVEL=${TT_LEVEL})

###############################

find_package(Threads REQUIRED)

###############################

find_package(Boost COMPONENTS REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

###############################

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
endif()

###############################
find_package(Git REQUIRED)
if(Git_FOUND)
	message("Git found: ${GIT_EXECUTABLE}")
	
	execute_process(COMMAND
		${GIT_EXECUTABLE} describe --always --dirty --abbrev=40
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
		OUTPUT_VARIABLE GIT_COMMIT
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	
	execute_process(COMMAND
		${GIT_EXECUTABLE} describe --always --dirty
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
		OUTPUT_VARIABLE GIT_SHORT_COMMIT
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
		
	execute_process(COMMAND
		${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
		OUTPUT_VARIABLE GIT_BRANCH
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	configure_file("${CMAKE_SOURCE_DIR}/src/version.hpp.in" "${CMAKE_SOURCE_DIR}/src/version.hpp" @ONLY)
	
	string(TIMESTAMP TIME_NOW "%Y/%m/%d %H:%M:%SZ" UTC)
	add_definitions(-DCOMPILE_TIME=${TIME_NOW})	
endif()

################################

find_package(SQLite3 REQUIRED)
if(SQLite3_FOUND)
	message(STATUS ${SQLite3_VERSION})
	include_directories(${SQLite3_INCLUDE_DIRS})
endif()

################################

set(SRC_DIR ./src)

##########################
# argh
##########################

include_directories(${ARGH_ROOT})

###########################
# tree trunks
###########################

include_directories(third/tree_trunks/src/include/)

###########################
# rapidjson
###########################

include_directories(${RAPIDJSON_ROOT})

#########################
# CoAP-te 
#########################
add_definitions(
#				-DCOAP_TE_PORT_C_STANDARD=1
#				-DCOAP_TE_PORT_POSIX=1
				-DCOAP_TE_LOG_COLOR=1
				-DCOAP_TE_LOG_LEVEL=4
				-DCOAP_TE_USE_ERROR_MESSAGES=1
				-DCOAP_TE_JSON_HELPER=1)

add_subdirectory(third/coap-te)

###########################
# My async
###########################

include_directories(third/my_async/include)

############################
# pusha
############################

add_subdirectory(third/pusha)

############################
# csv_export
############################
add_subdirectory(${SRC_DIR}/libs/csv)

###########################
# Adding files 
###########################
set(IMAGE_DIR		${SRC_DIR}/image)
set(IMAGE_SRC		${IMAGE_DIR}/image.cpp
					${IMAGE_DIR}/message/image.cpp
					${IMAGE_DIR}/message/process.cpp)

set(APP_DIR			${SRC_DIR}/app)
set(APP_SRC			${APP_DIR}/app.cpp
					${APP_DIR}/message/app.cpp
					${APP_DIR}/message/process.cpp)

set(MESSAGE_DIR		${SRC_DIR}/message)
set(MESSAGE_SRC		${MESSAGE_DIR}/make.cpp
					${MESSAGE_DIR}/process.cpp
					${MESSAGE_DIR}/user.cpp
					${MESSAGE_DIR}/report.cpp)

set(NOTIFY_DIR		${SRC_DIR}/notify)					
set(NOTIFY_SRC		${NOTIFY_DIR}/libs/push_notify.cpp
					${NOTIFY_DIR}/libs/telegram_bot.cpp
					${NOTIFY_DIR}/libs/smtp/client.cpp
					${NOTIFY_DIR}/libs/mail.cpp
					${NOTIFY_DIR}/factory.cpp
					${NOTIFY_DIR}/notify.cpp
					${NOTIFY_DIR}/message/make.cpp
					${NOTIFY_DIR}/message/make_json.cpp
					${NOTIFY_DIR}/message/process.cpp)

set(DEVICE_DIR		${SRC_DIR}/device)
set(DEVICE_SRC		${DEVICE_DIR}/device.cpp
					${DEVICE_DIR}/net.cpp
					${DEVICE_DIR}/message/device.cpp
					${DEVICE_DIR}/message/process.cpp
					${DEVICE_DIR}/message/esp_reset_reason.cpp
					${DEVICE_DIR}/helper.cpp
					${DEVICE_DIR}/types.cpp
					${DEVICE_DIR}/list.cpp
					${DEVICE_DIR}/tree.cpp
					)

set(SENSOR_DIR		${SRC_DIR}/sensor)
set(SENSOR_SRC		${SENSOR_DIR}/sensor_type.cpp
					${SENSOR_DIR}/sensor_type_list.cpp
					${SENSOR_DIR}/sensor_list.cpp
					${SENSOR_DIR}/helper.cpp
					${SENSOR_DIR}/message/sensor.cpp
					${SENSOR_DIR}/message/process.cpp
					)

set(REQUEST_DIR		${DEVICE_DIR}/request)					
set(REQUEST_SRC		${REQUEST_DIR}/request.cpp
					${REQUEST_DIR}/ac_load.cpp
					${REQUEST_DIR}/custom.cpp
					${REQUEST_DIR}/job.cpp
					${REQUEST_DIR}/packet.cpp
					${REQUEST_DIR}/system.cpp
					${REQUEST_DIR}/net.cpp
					${REQUEST_DIR}/ota.cpp
					${REQUEST_DIR}/app.cpp
					${REQUEST_DIR}/time.cpp
					${REQUEST_DIR}/in_progress.cpp
					)

set(RESOURCE_DIR	${DEVICE_DIR}/resources)
set(RESOURCE_SRC	${RESOURCE_DIR}/route.cpp
					${RESOURCE_DIR}/config.cpp
					${RESOURCE_DIR}/full_config.cpp
					${RESOURCE_DIR}/board_config.cpp
					${RESOURCE_DIR}/sensor_data.cpp
					${RESOURCE_DIR}/time.cpp
					${RESOURCE_DIR}/ota.cpp
					${RESOURCE_DIR}/app.cpp
					${RESOURCE_DIR}/job.cpp
					${RESOURCE_DIR}/info.cpp
					${RESOURCE_DIR}/init.cpp
					${RESOURCE_DIR}/process.cpp)					
					
set(USER_DIR		${SRC_DIR}/user)
set(USER_SRC		${USER_DIR}/user.cpp
					${USER_DIR}/group.cpp
					${USER_DIR}/list.cpp
					${USER_DIR}/policy.cpp
					${USER_DIR}/password.cpp
					${USER_DIR}/message/auth_message.cpp
					${USER_DIR}/message/user.cpp)

set(DB_DIR			${SRC_DIR}/db)
set(DB_SRC			${DB_DIR}/db.cpp
					${DB_DIR}/user.cpp
					${DB_DIR}/device.cpp
					${DB_DIR}/sensor.cpp
					${DB_DIR}/image.cpp
					${DB_DIR}/app.cpp
					${DB_DIR}/notify.cpp
					${DB_DIR}/report.cpp)
					
set(HELPER_DIR		${SRC_DIR}/helper)
set(HELPER_SRC		${HELPER_DIR}/sha256.cpp
					${HELPER_DIR}/time_helper.cpp
					${HELPER_DIR}/coap_helper.cpp)

set(LIBS_DIR		${SRC_DIR}/libs)
set(LIBS_SRC		${LIBS_DIR}/sqlite3/sqlite3.cpp)
					
set(INSTANCE_DIR	${SRC_DIR}/instance)
set(INSTANCE_SRC	${INSTANCE_DIR}/agro.cpp
					${INSTANCE_DIR}/check_root.cpp
					${INSTANCE_DIR}/notify.cpp
					${INSTANCE_DIR}/in_progress.cpp
					${INSTANCE_DIR}/user.cpp
					${INSTANCE_DIR}/device.cpp
					${INSTANCE_DIR}/tree.cpp
					${INSTANCE_DIR}/sensor.cpp
					${INSTANCE_DIR}/report.cpp
					${INSTANCE_DIR}/image.cpp
					${INSTANCE_DIR}/app.cpp
					${INSTANCE_DIR}/authenticate.cpp)

set(MAIN_SRC ${SRC_DIR}/main.cpp 
			${SRC_DIR}/error.cpp
			${SRC_DIR}/arguments.cpp
			${INSTANCE_SRC}
			${MESSAGE_SRC}
			${DEVICE_SRC}
			${SENSOR_SRC}
			${RESOURCE_SRC}
			${REQUEST_SRC}
			${IMAGE_SRC}
			${APP_SRC}
			${NOTIFY_SRC}
			${DB_SRC}
			${USER_SRC}
			${HELPER_SRC}
			${LIBS_SRC})
			
if(USE_SSL EQUAL 1)
	list(APPEND MAIN_SRC ${SRC_DIR}/websocket/load_certificate.cpp)
endif()

add_executable(${PROJECT_NAME} ${MAIN_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}
										${SQLite3_LIBRARIES}
										OpenSSL::SSL
										coap-te
										pusha
										csv_export
										${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
else()
	target_link_libraries(${PROJECT_NAME} pthread)
endif()

################
#tools
################
#### add user
add_executable(add_user tools/add_user.cpp 
						${LIBS_DIR}/sqlite3/sqlite3.cpp
						${SRC_DIR}/error.cpp)

set_target_properties(add_user PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(add_user OpenSSL::SSL
								${SQLite3_LIBRARIES})

##### add db
add_executable(init_db tools/init_db.cpp 
						${LIBS_DIR}/sqlite3/sqlite3.cpp
						${SRC_DIR}/error.cpp
						${USER_DIR}/password.cpp)

set_target_properties(init_db PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(init_db ${SQLite3_LIBRARIES} 
								pusha 
								coap-te			#just necessary because of includes dependencies
								${CMAKE_THREAD_LIBS_INIT})

########################################
# Installing
########################################
install(TARGETS agro_daemon init_db DESTINATION bin)
install(FILES db/scheme.sql DESTINATION bin)

#########################################
# Packing // https://cmake.org/cmake/help/latest/module/CPack.html
#########################################
#include(InstallRequiredSystemLibraries)
#set(CPACK_PACKAGE_NAME			"Agro Project Daemon")
set(CPACK_PACKAGE_VENDOR		"xeds corp")
set(CPACK_PACKAGE_DESCRIPTION	"Agro Project Daemon. Responsable to connect the field devices to the interface")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Agro Project central controller")
set(CPACK_PACKAGE_HOMEPAGE_URL 	"https://rnascunha.github.io/agro")
set(CPACK_RESOURCE_FILE_README 	"${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
#set(CPACK_PACKAGE_VERSION_MAJOR "${agro_daemon_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${agro_daemon_VERSION_MINOR}")
include(CPack)
