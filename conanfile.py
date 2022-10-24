from conans import ConanFile, CMake, tools
from conans.model.version import Version

class AgroDaemonConan(ConanFile):
    name = "agro_daemon"
    version = "0.1"
    license = "MIT"
    author = "Rafael Cunha <rnascunha@gmail.com>"
    url = "https://github.com/rnascunha/agro_daemon"
    description = "Agro Project Daemon"
    topics = ("agro", "daemon", "project")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "ssl": [True, False], "verbose": [0, 1, 2, 3, 4, 5]}
    default_options = {"shared": False, "fPIC": True, "ssl": False, "verbose": 4, "boost:header_only": True}
    generators = "cmake_find_package"
    requires = "argh/1.3.2", "sqlite3/[>3.0.0]", "boost/[>=1.70.0]", \
                "openssl/1.1.1q", "rapidjson/cci.20220822", \
                "tree_trunks/0.1@base/stable", \
                "coap-te/0.1@base/stable", "pusha/0.1@base/stable"
    exports = "LICENSE", "README.md", "URL.txt", "db/scheme.sql"
    exports_sources = "*.c", "*.cpp", "*.h", "*.hpp", "*.hpp.in", "*CMakeLists.txt", "*.cmake"

    def build_requirements(self):
        try:
            if CMake.get_version() < Version("3.10"):
                self.tool_requires("cmake/[>= 3.10]")
        except BaseException as err:
            self.tool_requires("cmake/[>= 3.10]")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.parallel = False
        cmake.definitions["WITH_SSL"] = 1 if self.options.ssl else 0
        cmake.definitions["VERBOSE"] = self.options.verbose
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("agro_daemon", dst="", src="bin")
        self.copy("init_db", dst="", src="bin")
        self.copy("scheme.sql", dst="", src="db")

    def package(self):
        self.copy("*.h", dst="include", src="src")
        self.copy("*.c", dst="include", src="src")
        self.copy("*.hpp", dst="src", src="src")
        self.copy("*.cpp", dst="src", src="src")
        self.copy("agro_daemon", dst="bin")
        self.copy("init_db", dst="bin")
        self.copy("scheme.sql", dst="bin", src="db")