#include "../src/db/sqlite3.hpp"

#include <iostream>
#include <fstream>
#include <system_error>
#include "pusha.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "../src/user/authenticate_params.h"

#define SQL_FILE		"scheme.sql"

void usage(const char* program) noexcept
{
	std::cout << "\t" << program << " <database_name>\n";
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cerr << "Error! Wrong number of argumetns!\n";
		usage(argv[0]);
		return 1;
	}

	std::error_code ec;
	sqlite3 db{argv[1], ec};

	if(ec)
	{
		std::cerr << "Error opening db '" << argv[1] << "'. [" << ec.message() << "]\n";
		return 1;
	}

	std::ifstream ifs{SQL_FILE};
	if(!ifs)
	{
		std::cerr << "Error opening scheme file\n";
		return 1;
	}
	char buffer[4096];
	std::size_t size = ifs.readsome(buffer, 4096);
	buffer[size] = '\0';
	int rc = db.exec(buffer);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Error initiating database! [" << rc << " / " << sqlite3_errstr(rc) << "]";
		return 1;
	}

	pusha::key ec_key = pusha::key::generate(ec);
	if(ec)
	{
		std::cerr << "Error generating notify key! [" << ec.message() << "\n";
		return 1;
	}

	std::string name, description, subscribe, notify_key{ec_key.export_private_key()};
	do{
		std::cout << "DB Name: " << std::flush;
		std::getline(std::cin, name);
		if(name.empty())
		{
			std::cerr << "'Name' can't be empty.\n";
		}
		else break;
	}while(true);

	std::cout << "Description[]: " << std::flush;
	std::getline(std::cin, description);

	do{
		std::cout << "Subscriber: " << std::flush;
		std::getline(std::cin, subscribe);
		if(subscribe.empty())
		{
			std::cerr << "'Subscribe' can't be empty.\n";
		}
		else break;
	}while(true);

	static constexpr const
		std::string_view stmt{"INSERT INTO instance(name, description, notify_private_key, subscribe) VALUES(?,?,?,?)"};

	sqlite3::statement res;
	rc = db.prepare(stmt, res);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Error preparing 'DB' instance data [" << rc << " / " << sqlite3_errstr(rc) << "]\n";
		return 1;
	}

	res.bind(1, name);
	res.bind(2, description);
	res.bind(3, notify_key);
	res.bind(4, subscribe);

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		std::cerr << "Error recording data to 'DB'. [" << rc << "]\n";
		return 1;
	}

	/**
	 * Insert user root
	 */
	std::cout << "Add 'root' user? [Y/n]" << std::flush;
	std::string aws;
	std::getline(std::cin, aws);
	if(!(aws.empty() || aws[0] == 'y' || aws[1] == 'Y'))
	{
		return 0;
	}

	std::string root_passwd;
	do{
		std::cout << "root password: " << std::flush;
		std::getline(std::cin, root_passwd);
		if(root_passwd.empty())
		{
			std::cerr << "'root password' can't be empty.\n";
		}
		else break;
	}while(true);

	unsigned char salt[USER_AUTH_SALT_LENGTH] = {0};
	RAND_bytes(salt, sizeof(salt));

	unsigned char key[USER_AUTH_KEY_LENGTH] = {0};
	PKCS5_PBKDF2_HMAC(root_passwd.data(), root_passwd.size(),
		salt, USER_AUTH_SALT_LENGTH,
		USER_AUTH_INTERATION_NUMBER, USER_AUTH_HASH_ALGORITHM,
		USER_AUTH_KEY_LENGTH, key);

	static constexpr const
	std::string_view stmt_n{"INSERT INTO user(username,password,status,salt) VALUES('root',?,?,?)"};

	sqlite3::statement res_n;
	rc = db.prepare(stmt_n, res_n);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Error preparing 'DB' user root data [" << rc << " / " << sqlite3_errstr(rc) << "]\n";
		return 1;
	}

	res_n.bind(1, key, USER_AUTH_KEY_LENGTH);
	res_n.bind(2, 0);
	res_n.bind(3, salt, USER_AUTH_SALT_LENGTH);

	if(res_n.step() != SQLITE_DONE)
	{
		std::cerr << "Error inserting user 'root' at database! [" << rc << " / " << db.error() << "]\n";
		return EXIT_FAILURE;
	}

	return 0;
}
