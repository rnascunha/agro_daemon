if (NOT GIT_COMMIT)
    execute_process(COMMAND
        ${GIT_EXECUTABLE} describe --always --dirty --abbrev=40
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
        OUTPUT_VARIABLE GIT_COMMIT
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if (NOT GIT_SHORT_COMMIT)
    execute_process(COMMAND
        ${GIT_EXECUTABLE} describe --always --dirty
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
        OUTPUT_VARIABLE GIT_SHORT_COMMIT
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
	
if (NOT GIT_BRANCH)
    execute_process(COMMAND
        ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
        OUTPUT_VARIABLE GIT_BRANCH
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    configure_file("${CMAKE_SOURCE_DIR}/cmake/version.hpp.in" "${CMAKE_SOURCE_DIR}/src/version.hpp" @ONLY)
endif()