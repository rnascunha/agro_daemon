#include <iostream>
#include <filesystem>
#include <cstring>
#include <string>

//#include "../src/libs/sqlite3/sqlite3.hpp"
#include "sqlite3.hpp"

#include "../src/user/info.hpp"
#include "../src/user/group.hpp"
#include "../src/user/password.hpp"
#include "../src/db/helper.hpp"

#include "../src/error.hpp"

#include <system_error>

constexpr const char* default_db = "agro.db";

static void usage(const char* program)
{
	std::cout << R"(Add user to 'agro_daemon' app database.
Usage:
	)" << program << R"( -h|[<db_file>] [-d] [-u <user>] [-p <password>] 
			[-n <name>] [-m <mail>] [-t telegram_id] 
Where:
	-h
		This help message.
	[<db_file>]
		Database file. Default: )" << default_db << R"(
	-d
		Do not prompt missing fields. User will be recorded with the
		data provided by command line. Command will fail if any of the
		required (*) fields be missing.
	-u <user>	*required
		Username to be included at database. Must be unique.
	-p <password>	*required
		Password of username to be included.
	-n <name>
		Name of username to be included.
	-m <mail>
		Email of user.
	-t <telegram_id>
		Telegram Chat ID of user.
)";
}

static bool check_user_exists(xeds::sqlite3& db, std::string const& username) noexcept
{
	auto user = Agro::DB_Helper::get_user(db, username);
	return user.is_valid();
}

static bool add_user(xeds::sqlite3& db,
						std::string const& username,
						std::string const& password,
						std::string const& name,
						std::string const& mail,
						std::string const& telegram_chat_id,
						Agro::User::user_id& id) noexcept
{
	id = Agro::User::invalid_id;

	Agro::User::key_password key;
	Agro::User::salt_password salt;
	if(!Agro::User::create_password(password, salt, key))
	{
		std::cerr << "- Error hashing password of '" << username << "'!\n";
		return false;
	}

	if(Agro::DB_Helper::add_user(db, username, key, salt, name, mail, telegram_chat_id, id) != SQLITE_DONE)
	{
		std::cerr << "- Error adding user '" << username << "' to database! [" << db.error() << "]\n";
		return false;
	}

	return true;
}

bool prompt_add_user_groups(xeds::sqlite3& db,
		Agro::User::user_id uid) noexcept
{
	std::cout << "> Adding user to groups:\n";

	Agro::User::Groups groups;

	if(Agro::DB_Helper::read_user_groups(db, groups) != SQLITE_DONE)
	{
		std::cerr << "- Error reading user groups! Quitting!\n";
		return false;
	}

	std::vector<Agro::User::group_id> gids;
	for(auto const& group : groups)
	{
		std::string op;
		std::cout << group.name() << "[" << group.id() << "] "
				<< group.description() << " [Y/n]: "  << std::flush;
		std::getline(std::cin, op);
		if(!op.length() || op[0] == 'y' || op[0] == 'Y')
		{
			std::cout << "> Adding user to group '" << group.name() << "'\n";
			gids.push_back(group.id());
		}
	}

	Agro::DB_Helper::add_user_to_groups(db, uid, gids);

	return true;
}

int main(int argc, const char** argv)
{
	const char* db_file{default_db};
	std::string user, name, password,
				mail, telegram_chat_id;
	bool prompt = true, db_set = false;

	for(int i = 1; i != argc; i++)
	{
		//checking help option
		if(strcmp("-h", argv[i]) == 0)
		{
			usage(argv[0]);
			return EXIT_SUCCESS;
		}
		//Checking prompt option
		if(strcmp("-d", argv[i]) == 0)
		{
			prompt = false;
			continue;
		}
		//checking user option
		if(strcmp("-u", argv[i]) == 0)
		{
			++i;
			if(argc == i)
			{
				std::cerr << "Username not supplied! Quitting!\n";
				return EXIT_FAILURE;
			}
			user = argv[i];
			continue;
		}
		//checking password option
		if(strcmp("-p", argv[i]) == 0)
		{
			++i;
			if(argc == i)
			{
				std::cerr << "Password not supplied! Quitting!\n";
				return EXIT_FAILURE;
			}
			password = argv[i];
			continue;
		}
		//checking mail option
		if(strcmp("-n", argv[i]) == 0)
		{
			++i;
			if(argc == i)
			{
				std::cerr << "Name not supplied! Quitting!\n";
				return EXIT_FAILURE;
			}
			name = argv[i];
			continue;
		}
		//checking mail option
		if(strcmp("-m", argv[i]) == 0)
		{
			++i;
			if(argc == i)
			{
				std::cerr << "Mail not supplied! Quitting!\n";
				return EXIT_FAILURE;
			}
			mail = argv[i];
			continue;
		}
		//checking telegram chat id option
		if(strcmp("-t", argv[i]) == 0)
		{
			++i;
			if(argc == i)
			{
				std::cerr << "Telegram Chat ID not supplied! Quitting!\n";
				return EXIT_FAILURE;
			}
			telegram_chat_id = argv[i];
			continue;
		}
		db_file = argv[i];
		db_set = true;
	}

	if(!db_set)
	{
		std::cout << "> DB file not defined. Using '" << db_file << "'\n";
	}

	if(!std::filesystem::is_regular_file(db_file))
	{
		std::cerr << "DB file '" << db_file << "' is not a file! Quitting!\n";
		return EXIT_FAILURE;
	}

	if(!prompt)
	{
		if(user.empty())
		{
			std::cerr << "- Required 'user' field not provided! Quitting!\n";
			return EXIT_FAILURE;
		}

		if(password.empty())
		{
			std::cerr << "- Required 'password' field not provided! Quitting!\n";
			return EXIT_FAILURE;
		}

		if(!Agro::User::validate_password(password))
		{
			std::cerr << "- Invalid password! Quitting!\n";
			return EXIT_FAILURE;
		}

		if(name.empty())
		{
			name = user;
		}
	}

	std::cout << db_file << " / " << user << " / " << password << " / " << name << " / "
				<< mail << " / " << telegram_chat_id << "\n";

	/**
	 * End paramters
	 */

	std::error_code ec;
	xeds::sqlite3 db{db_file, ec};
	if(ec)
	{
		std::cerr << "- Error opening file DB '" << db_file << "'. [" << ec.message() << "]\n";
		return EXIT_FAILURE;
	}

	if(!user.empty())
	{
		bool rc = check_user_exists(db, user);
		if(ec)
		{
			std::cerr << "- Error checking 'user' at database! Quitting!";
			return EXIT_FAILURE;
		}
		if(rc)
		{
			std::cerr << "- User '" << user << "' already exist at database!";
			if(!prompt)
			{
				std::cerr << " Quitting\n";
				return EXIT_FAILURE;
			}
			user.clear();
			std::cerr << "\n";
		}
	}

	if(!prompt)
	{
		Agro::User::user_id id = Agro::User::invalid_id;;
		if(!add_user(db, user, password, name, mail, telegram_chat_id, id))
		{
			return EXIT_FAILURE;
		}
		return prompt_add_user_groups(db, id) ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	/**
	 * Interactively ask missing fields
	 */
	if(user.empty())
	{
		do{
			std::cout << "Username: " << std::flush;
			std::getline(std::cin, user);
		}while(!user.length() || check_user_exists(db, user));
	}

	if(name.empty())
	{
		std::cout << "Name[" << user << "]: " << std::flush;
		std::getline(std::cin, name);

		if(!name.length())
		{
			name = user;
		}
	}

	if(password.empty())
	{
		do{
			std::string confirm_password;
			do{
				std::cout << "Password: " << std::flush;
				std::getline(std::cin, password);
				if(password.length() && Agro::User::validate_password(password))
				{
					break;
				}
				std::cout << "- Invalid password!\n";
			}while(true);

			std::cout << "Confirm password: " << std::flush;
			std::getline(std::cin, confirm_password);

			if(password == confirm_password)
			{
				break;
			}
			std::cout << "- Passowords does not match!\n";
		}while(true);
	}

	if(mail.empty())
	{
		std::cout << "Email[]: " << std::flush;
		std::getline(std::cin, mail);
	}

	if(telegram_chat_id.empty())
	{
		std::cout << "Telegram Chat ID[]: " << std::flush;
		std::getline(std::cin, telegram_chat_id);
	}

	Agro::User::user_id id;
	if(!add_user(db, user, password, name, mail, telegram_chat_id, id))
	{
		return EXIT_FAILURE;
	}

	return prompt_add_user_groups(db, id) ? EXIT_SUCCESS : EXIT_FAILURE;
}
