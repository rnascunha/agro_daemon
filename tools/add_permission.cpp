#include "../user/policy.hpp"
#include "../db/db.hpp"

#include <iostream>
#include <filesystem>
#include <system_error>

void usage(const char* program) noexcept
{
	std::cout << "Usage:\n\t" << program << " <database_file>\n";
}

int main(int argc, char** argv)
{
	if(argc == 0)
	{
		std::cerr << "Error! Database file not specified!\n";
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if(!std::filesystem::is_regular_file(argv[1]))
	{
		std::cerr << "Error! '" << argv[1] << "' is not a valid file!\n";
		return EXIT_FAILURE;
	}

	std::error_code ec;
	Agro::DB db{argv[1], ec};
	if(ec)
	{
		std::cerr << "Error trying to open database file '" << argv[1] << "'\n";
		return EXIT_FAILURE;
	}

	Agro::Authorization::Permission_List perm_list;



}
