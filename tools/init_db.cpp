#include "../src/db/sqlite3.hpp"

#include <iostream>
#include <fstream>
#include <system_error>
#include "pusha.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "../src/helper/enum_ops.hpp"

#include "../src/user/authenticate_params.h"
#include "../src/user/password.hpp"

#include "../src/user/policy.hpp"

#define SQL_FILE		"scheme.sql"

struct group{
	int							id;
	std::string 				name;
	std::string 				description;
	Agro::Authorization::rule	rules;
};

struct sensor_type{
	std::string 				name;
	std::string					type;
	std::string					unit_name;
	std::string					unit;
};

using namespace Agro::Authorization;

static const Policy_Type policy_types[] = {
		{1, rule::user_admin, "User Admin", "Add/remove users/groups/policies"},
		{2, rule::view_device, "View Devices", "View devices data/status"},
		{3, rule::edit_device, "Edit Devices", "Edit devices data/status"},
		{4, rule::get_resource, "GET resource", "GET resource operation"},
		{5, rule::post_resource, "POST resource", "POST resource operation"},
		{6, rule::put_resource, "PUT resource", "PUT resource operation"},
		{7, rule::delete_resource, "DELETE resource", "DELETE resource operation"},
		{8, rule::view_image, "View Image", "View Image"},
		{9, rule::upload_image, "Upload Image", "Upload new image to server"},
		{10, rule::install_image, "Install Image", "Install image at devices"},
		{11, rule::upload_app, "Upload App", "Upload app to server"},
		{12, rule::install_app, "Install App", "Install app at devices"}
};

static const group groups[] = {
		{1, "system", "System administrators", rule::all},
		{2, "operator", "Can view e execute commands",
						rule::all_device | rule::all_resources | rule::all_image | rule::all_app},
		{3, "viewer", "Can only view devices", rule::view_device},
		{4, "op_get", "Can view/get operations on devices", rule::all_device | rule::get_resource},
		{5, "op_put", "Can view/put operations on devices", rule::all_device | rule::put_resource},
		{6, "op_post", "Can view/post operations on devices", rule::all_device | rule::post_resource},
		{6, "op_delete", "Can view/delete operations on devices", rule::all_device | rule::delete_resource},
};

static const sensor_type sensors[] = {
		{"Received Signal Strength Indicator", "RSSI", "decibel miliwatt", "dBm"},
		{"Temperature", "temperature", "celsius", "C"},
		{"GPIOs", "gpios", "signal level", ""}
};

static void usage(const char* program) noexcept
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

	if(std::filesystem::exists(argv[1]))
	{
		std::cerr << "File already exists! Quitting...\n";
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
		std::filesystem::remove(argv[1]);
		std::cerr << "Error opening scheme file\n";
		return 1;
	}
	char buffer[8192];
	std::size_t size = ifs.readsome(buffer, 8192);
	buffer[size] = '\0';
	int rc = db.exec(buffer);
	if(rc != SQLITE_OK)
	{
		std::filesystem::remove(argv[1]);
		std::cerr << "Error initiating database! [" << rc << " / " << sqlite3_errstr(rc) << "]\n";
		return 1;
	}

	pusha::key ec_key = pusha::key::generate(ec);
	if(ec)
	{
		std::filesystem::remove(argv[1]);
		std::cerr << "Error generating notify key! [" << ec.message() << "\n";
		return 1;
	}

	std::string name,
				description,
				subscribe,
				notify_key{ec_key.export_private_key()},
				root_password;
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

	do{
		std::cout << "Root password: " << std::flush;
		std::getline(std::cin, root_password);
		if(root_password.size() < USER_ROOT_KEY_MINIMUM_LENGHT)
		{
			std::cerr << "'Root password' can't be shorter than " << USER_ROOT_KEY_MINIMUM_LENGHT << " characters\n";
		}
		else break;
	}while(true);

	Agro::User::salt_password salt;
	Agro::User::key_password password;

	if(!Agro::User::create_password(root_password, salt, password))
	{
		std::filesystem::remove(argv[1]);
		std::cerr << "Error generation 'root' password!\n";
		return 1;
	}

	sqlite3::statement res;
	rc = db.prepare_bind(
			"INSERT INTO instance(name, description, notify_private_key, subscribe, root_password, root_salt) VALUES(?,?,?,?,?,?)",
			res,
			name, description, notify_key, subscribe,
			sqlite3::binary{password, USER_AUTH_KEY_LENGTH},
			sqlite3::binary{salt, USER_AUTH_SALT_LENGTH});
	if(rc != SQLITE_OK)
	{
		std::filesystem::remove(argv[1]);
		std::cerr << "Error preparing 'DB' instance data [" << rc << " / " << sqlite3_errstr(rc) << "]\n";
		return 1;
	}

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		std::filesystem::remove(argv[1]);
		std::cerr << "Error recording data to 'DB'. [" << rc << "]\n";
		return 1;
	}

	/**
	 * Adding Groups
	 */
	for(auto const& group : groups)
	{
		res.finalize();
		rc = db.prepare_bind("INSERT INTO user_group(name, description) VALUES(?,?)",
					res, group.name, group.description);
		if(rc != SQLITE_OK)
		{
			std::cerr << "Error preparing to insert group '" << group.name << "' [" << rc << "]\n";
			continue;
		}
		rc = res.step();
		if(rc != SQLITE_DONE)
		{
			std::cerr << "Error inserting group '" << group.name << "' [" << rc << "]\n";
		}

		res.finalize();
		int groupid = db.last_insert_rowid();
		rc = db.prepare_bind("INSERT INTO policy(groupid, rules) VALUES(?,?)",
				res, groupid, static_cast<int>(group.rules));

		if(rc != SQLITE_OK)
		{
			std::cerr << "Error preparing to insert group policies '" << group.name << "' [" << rc << "]\n";
			continue;
		}
		rc = res.step();
		if(rc != SQLITE_DONE)
		{
			std::cerr << "Error inserting group policies '" << group.name << "' [" << rc << "]\n";
		}
	}

	res.finalize();
	for(auto const& ptype : policy_types)
	{
		rc = db.prepare_bind("INSERT INTO policy_type(code, name, description) VALUES(?,?,?)",
							res, static_cast<int>(ptype.code), ptype.name, ptype.description);
		if(rc != SQLITE_OK)
		{
			std::cerr << "Error preparing to insert policy type '" << ptype.name << "' [" << rc << "]\n";
			continue;
		}
		rc = res.step();
		if(rc != SQLITE_DONE)
		{
			std::cerr << "Error inserting  policy type '" << ptype.name << "' [" << rc << "]\n";
		}
		res.clear_bidings();
	}
	res.finalize();

	/**
	 * Sensor types
	 */
	for(auto const& sensor : sensors)
	{
		rc = db.prepare_bind("INSERT INTO sensor_type(name, type, unit_name, unit) VALUES(?,?,?,?)",
				res, sensor.name, sensor.type, sensor.unit_name, sensor.unit);
		if(rc != SQLITE_OK)
		{
			std::cerr << "Error preparing to insert sensor type [" << rc << "]\n";
			continue;
		}

		rc = res.step();
		if(rc != SQLITE_DONE)
		{
			std::cerr << "Error preparing inserting sensor type [" << rc << "]\n";
		}
		res.clear_bidings();
	}

	return 0;
}
