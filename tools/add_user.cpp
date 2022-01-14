//#include <sqlite3.h>
#include <iostream>
#include <filesystem>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "../src/libs/sqlite3/sqlite3.hpp"
#include "../src/user/authenticate_params.h"

#include <system_error>

void usage(const char* program)
{
	std::cout << R"(Usage:
	)" << program << R"( <db_file>
)";
}

//static void print_array_mey(const unsigned char* v, int s)
//{
//	int i = 0;
//	while(s--)
//	{
//		std::printf("%02X ", v[i++]);
//	}
//	printf("\n");
//}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cerr << "Wrong number of arguments!\n";
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if(!std::filesystem::is_regular_file(argv[1]))
	{
		std::cerr << "'" << argv[1] << "' is not a file!\n";
		return EXIT_FAILURE;
	}

	std::error_code ec;
	sqlite3 db{argv[1], ec};
	if(ec)
	{
		std::cerr << "Error opening file db '" << argv[1] << "'. [" << db.error() << "]\n";
		return EXIT_FAILURE;
	}

	std::string username, name, password, cpassword, email;
	do{
		std::cout << "Username: " << std::flush;
		std::getline(std::cin, username);
	}while(!username.length());

	std::cout << "Name[" << username << "]: " << std::flush;
	std::getline(std::cin, name);

	if(!name.length())
	{
		name = username;
	}

	do{
		std::cout << "Password: " << std::flush;
		std::getline(std::cin, password);
	}while(!password.length());

	std::cout << "Confirm password: " << std::flush;
	std::getline(std::cin, cpassword);

	if(password != cpassword)
	{
		std::cout << "Passowords does not match!\n" << std::flush;
		return EXIT_FAILURE;
	}

	std::cout << "Email[]: ";
	std::getline(std::cin, email);

	std::cout << R"(----------------
Username: )" << username << R"(
Name: )" << name << R"(
Password: )" << password << R"(
Email: )" << email << R"(
----------------
)";
	unsigned char salt[USER_AUTH_SALT_LENGTH] = {0};
	RAND_bytes(salt, sizeof(salt));

	unsigned char key[USER_AUTH_KEY_LENGTH] = {0};
	PKCS5_PBKDF2_HMAC(password.data(), password.size(),
		salt, USER_AUTH_SALT_LENGTH,
		USER_AUTH_INTERATION_NUMBER, USER_AUTH_HASH_ALGORITHM,
		USER_AUTH_KEY_LENGTH, key);

	static constexpr const
	std::string_view stmt{"INSERT INTO user(username,password,name,status,email,salt) VALUES(?,?,?,?,?,?)"};

	sqlite3::statement res;
	int rc = db.prepare(stmt, res);
	if(rc != SQLITE_OK) return rc;

	res.bind(1, username);
	res.bind(2, key, USER_AUTH_KEY_LENGTH);
	res.bind(3, name);
	res.bind(4, 0);
	res.bind(5, email);
	res.bind(6, salt, USER_AUTH_SALT_LENGTH);

	if(res.step() != SQLITE_DONE)
	{
		std::cerr << "Error inserting user at database! [" << rc << " / " << db.error() << "]\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
