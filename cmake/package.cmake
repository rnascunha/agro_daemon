# https://cmake.org/cmake/help/latest/module/CPack.html

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME			"Agro Project Daemon")
set(CPACK_PACKAGE_VENDOR		"xeds corp")
set(CPACK_PACKAGE_DESCRIPTION	"Agro Project Daemon. Responsable to connect the field devices to the interface")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Agro Project central controller")
set(CPACK_PACKAGE_HOMEPAGE_URL 	"https://rnascunha.github.io/agro")
set(CPACK_RESOURCE_FILE_README 	"${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
#set(CPACK_PACKAGE_VERSION_MAJOR "${agro_daemon_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${agro_daemon_VERSION_MINOR}")
include(CPack)