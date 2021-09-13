#include <iostream>
#include <filesystem>
#include <system_error>

#include <cstdlib>

#include "../src/db/db.hpp"

void usage(const char* program) noexcept
{
	std::cout << "Usage:\n\t" << program << " <database_file>\n";
}

void print_database_status(Agro::User::Users const& users) noexcept
{
	std::cout << "Totals: users[" << users.infos().size() << "]/"
			<< "groups[" << users.groups().size() << "]\n";

	std::cout << "user list:\n";
	for(auto const& u : users.infos())
	{
		std:: cout << "\t" << u.id() << ": " << u.username() << "\n";
	}

	std::cout << "group list:\n";
	for(auto const& g : users.groups())
	{
		std::cout << "\t" << g.id() << ": " << g.name() << "[";
		for(auto const& u : g)
		{
			std::cout << u << " ";
		}
		std::cout << "]\n";
	}
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

	Agro::User::Users users;
	if(!db.read_user_all_db(users))
	{
		std::cerr << "Error reading user info from database!\n";
		std::cerr << "DB error: " << db.error() << "\n";
		return EXIT_FAILURE;
	}

	print_database_status(users);

	std::cout << "\n";

	std::string gname, gdescription;
	do{
		std::cout << "New group name: ";
		std::getline(std::cin, gname);
		if(gname.empty())
		{
			std::cerr << "User group name can't be empty!\n";
			continue;
		}
		if(users.groups().contains(gname))
		{
			std::cerr << "User '" << gname << "' already exists!\n";
			continue;
		}
		break;
	}while(true);
	std::cout << "New group description[]: ";
	std::getline(std::cin, gdescription);

	auto gid = db.add_user_group(gname, gdescription);
	if(gid <= 0)
	{
		std::cerr << "Error adding group '" << gname << "' to database!\n";
		return EXIT_FAILURE;
	}
	if(!users.groups().add(Agro::User::Group{gid, gname, gdescription}))
	{
		std::cerr << "Error adding group '" << gname << "'!\n";
		return EXIT_FAILURE;
	}

	std::cout << "Added group '" << gname << "[" << gid << "]'\n";
	auto group = users.groups().get(gid);

	while(true)
	{
		std::string aws;
		std::cout << "Add user '" << gname << "' group? [Y/n]: ";
		std::getline(std::cin, aws);
		if(!(aws.empty() || aws[0] == 'y' || aws[1] == 'Y'))
		{
			break;
		}
		std::cout << "User ID: ";
		std::getline(std::cin, aws);
		if(aws.empty())
		{
			std::cerr << "User ID not informed!\n";
			continue;
		}
		int uid = static_cast<int>(std::strtoll(aws.c_str(), nullptr, 10));
		if(!uid)
		{
			std::cerr << "Invalid input! [" << aws << "]\n";
			continue;
		}
		if(group->contains(uid))
		{
			std::cerr << "User '" << uid << "' already at group '" << gname << "'!\n";
			continue;
		}
		if(db.add_user_to_group(gid, uid) != SQLITE_DONE)
		{
			std::cerr << "Error adding user '" << uid << "' to group '" << gname << "' at database!\n";
			return EXIT_FAILURE;
		}
		if(!group->add(uid))
		{
			std::cerr << "Error adding user '" << uid << "' to group '" << gname << "'!\n";
			return EXIT_FAILURE;
		}
		std::cerr << "Added user '" << uid << "' to group '" << gname << "'\n";
	}

	std::cout << "\n";
	print_database_status(users);

	return EXIT_SUCCESS;
}
