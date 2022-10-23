#### add user
add_executable(add_user tools/add_user.cpp 
						${LIBS_DIR}/sqlite3/sqlite3.cpp
						${SRC_DIR}/error.cpp
						${DB_DIR}/helper.cpp
						${USER_DIR}/info.cpp
						${USER_DIR}/group.cpp
						${USER_DIR}/password.cpp)

set_target_properties(add_user PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(add_user PUBLIC OpenSSL::SSL
								sqlite3_wrapper)

##### init db
add_executable(init_db tools/init_db.cpp 
						${SRC_DIR}/error.cpp
						${USER_DIR}/password.cpp)

set_target_properties(init_db PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(init_db pusha::pusha
								sqlite3_wrapper)