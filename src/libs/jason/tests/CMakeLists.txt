message(STATUS "Running testes")

set(test_bin	jason_test)

find_package(GTest)
if(NOT GTest_FOUND)
	message(STATUS "Fetching GTest...")
	include(FetchContent)
	FetchContent_Declare(
	  googletest
	  URL https://github.com/google/googletest/archive/ae1b7ad4308249bfa928e65d1a33be117fc0992c.zip
	)
	
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	
	set(GTEST_LIBRARIES gtest)
	set(GTEST_MAIN_LIBRARIES gtest_main)
endif()

enable_testing()

add_executable(${test_bin} tests.cpp)
target_link_libraries(${test_bin} ${GTEST_MAIN_LIBRARIES} ${PROJECT_NAME})

include(GoogleTest)
gtest_discover_tests(${test_bin})