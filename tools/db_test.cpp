#include "../src/db/sqlite3.hpp"
#include <iostream>
#include <system_error>
#include <string>

#define DB_FILE		"agro_db_new.db"

static const uint8_t pass[] = {0x1, 0x2, 0x3};
static const uint8_t salt[] = {0x4, 0x5, 0x6, 0x7};

int main()
{
	std::error_code ec;
	sqlite3 db{DB_FILE, ec};

	if(ec)
	{
		std::cerr << "Error opening...\n";
		return 1;
	}

	sqlite3::statement res;
	int rc = db.prepare_bind("SELECT * FROM user WHERE userid = ? AND username = ?", res, 7, "rafael");

	std::cout << "Returned: " << rc << "\n";

	if(res.step() != SQLITE_ROW)
	{
		std::cout << "Find notting...\n";
		return 1;
	}

	std:: cout << "User name: " << res.text(3) << "\n";

	res.reset();
	rc = db.prepare_bind(std::string_view{"INSERT INTO user(username, name, password, salt) VALUES(?,?,?,?)"},
			res, "nuser", "Nuser", sqlite3::binary{pass, 3}, sqlite3::binary{salt, 4});

	if(res.step() != SQLITE_DONE)
	{
		std::cout << "INSERT problem...\n";
		return 1;
	}

	return 0;
}
