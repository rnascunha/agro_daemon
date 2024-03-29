#include "sqlite3.hpp"

#include <cstring>

#include <iostream>
#include <fstream>
#include <system_error>
#include <filesystem>

#include "pusha.hpp"

#include "../src/helper/enum_ops.hpp"

#include "../src/user/password.hpp"

#include "../src/user/policy.hpp"
#include "../src/sensor/sensor_type_list.hpp"
#include "../src/libs/smtp/types.hpp"
#include "scheme.hpp"

constexpr const char* default_db = "agro.db";

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
		{7, "op_delete", "Can view/delete operations on devices", rule::all_device | rule::delete_resource},
};

static const Agro::Sensor::sensor_description sensors[] = {
		{1, "rssi", "RSSI",
				Agro::Sensor::sensor_unit_type::integer, "dBm", "decibel miliwatt",
				"Received Signal Strength Indicator", false},
		{2, "temperature", "Temperature",
				Agro::Sensor::sensor_unit_type::tfloat, "C", "celsius",
				"Temperatura sensor in celsius", false},
		{3, "input", "Input",
				Agro::Sensor::sensor_unit_type::tunsigned, "digital", "digital level",
				"Input digital level", false},
		{4, "output", "Output",
				Agro::Sensor::sensor_unit_type::tunsigned, "digital", "digital level",
				"Output digital level", false},
		{5, "gpios", "GPIO's",
				Agro::Sensor::sensor_unit_type::tunsigned, "digital array", "digital array level",
				"Array of bits (input/outputs)", false},
		{6, "pressure", "Pressure",
						Agro::Sensor::sensor_unit_type::tfloat, "Pa", "Pascal",
						"Pressure in Pascal", false},
		{7, "density", "Density",
								Agro::Sensor::sensor_unit_type::tfloat, "Kg/m3", "Kilos per meter cubic",
								"Density", false}
};

static void usage(const char* program) noexcept
{
	std::cout << R"(Create 'agro_daemon' app database.
Usage:
	)" << program << R"( -h|[-f] [-s <sql_scheme>] [<output_file>])
Where:
	-h
		This help message.
	-s <sql_scheme>
		Define the SQL scheme that the database will be created.
	-f
		If <output_file> exists, remove it.
	[<output_file>]
		File output name database will be created. 
		Default: )" << default_db << "\n";
}

int main(int argc, char** argv)
{
	using xeds::sqlite3;

	bool force = false, set_output = false;
	const char* output = default_db;
	const char* scheme = nullptr;

	for(int i = 1; i != argc; ++i)
	{
		//Checking help option
		if(strcmp("-h", argv[i]) == 0)
		{
			usage(argv[0]);
			return EXIT_SUCCESS;
		}
		//Checking scheme option
		if(strcmp("-s", argv[i]) == 0)
		{
			++i;
			if(i == argc)
			{
				std::cerr << "- Scheme file not defined! Quitting!\n";
				return EXIT_FAILURE;
			}
			scheme = argv[i];
			continue;
		}
		//Checking if force
		if(strcmp("-f", argv[i]) == 0)
		{
			force = true;
			continue;
		}
		output = argv[i];
		set_output = true;
	}

	//Checking if scheme file exists
	if(scheme != nullptr && !std::filesystem::is_regular_file(scheme))
	{
		std::cerr << "- Scheme file '" << scheme << "' not found! Quitting!\n";
		return EXIT_FAILURE;
	}

	if(std::filesystem::is_regular_file(output))
	{
		if(!force)
		{
			std::cerr << "- File '" << output << "' already exists! Quitting...\n";
			return EXIT_FAILURE;
		}
		std::filesystem::remove(output);
	}
	if(!set_output)
	{
		std::cout << "> Output file not defined. Using '" << output << "'\n";
	}

	/**
	 * End parameter check
	 */

	std::error_code ec;
	sqlite3 db{output, ec};

	if(ec)
	{
		std::filesystem::remove(output);
		std::cerr << "- Error opening db '" << output << "'. [" << ec.message() << "]\n";
		return EXIT_FAILURE;
	}

	int rc;
	if (scheme) {
		std::ifstream ifs(scheme);
		if(!ifs)
		{
			std::filesystem::remove(output);
			std::cerr << "- Error opening scheme file. [" << scheme << "]\n";
			return EXIT_FAILURE;
		}
		char buffer[8192];
		std::size_t size = ifs.readsome(buffer, 8192);
		buffer[size] = '\0';
		rc = db.exec(buffer);
	} else {
		rc = db.exec(Agro::db_default_scheme.data());
	}
	if(rc != SQLITE_OK)
	{
		std::filesystem::remove(output);
		std::cerr << "- Error initiating database! [" << rc << " / " << sqlite3_errstr(rc) << "]\n";
		return 1;
	}

	pusha::key ec_key = pusha::key::generate(ec);
	if(ec)
	{
		std::filesystem::remove(output);
		std::cerr << "- Error generating notify key! [" << ec.message() << "\n";
		return EXIT_FAILURE;
	}

	std::string name,
				description,
				subscribe,
				notify_key{ec_key.export_private_key()},
				telegram_bot_token,
				root_password;
	SMTP::server smtp_server;

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

	std::cout << "Subscriber [mailto:email@company.com]: " << std::flush;
	std::getline(std::cin, subscribe);
	if(subscribe.empty())
	{
		subscribe = "email@company.com";
	}

	std::cout << "Telegram Bot Token []: " << std::flush;
	std::getline(std::cin, telegram_bot_token);


	std::string smtp_config;
	std::cout << "Configure email notify [y/N]: ";
	std::getline(std::cin, smtp_config);
	if(smtp_config[0] == 'y' || smtp_config[0] == 'Y')
	{
		std::cout << "SMTP server: ";
		std::getline(std::cin, smtp_server.addr);

		std::cout << "SMTP port: ";
		std::getline(std::cin, smtp_server.port);

		std::cout << "SMTP user: ";
		std::getline(std::cin, smtp_server.user);

		std::cout << "SMTP password: ";
		std::getline(std::cin, smtp_server.password);
	}

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
		std::filesystem::remove(output);
		std::cerr << "- Error generation 'root' password!\n";
		return EXIT_FAILURE;
	}

	sqlite3::statement res;
	rc = db.prepare_bind(
			"INSERT INTO instance(name, description, "
						"notify_private_key, subscribe, root_password, root_salt, "
						"telegram_bot_token, smtp_server, smtp_port, smtp_user, smtp_password) "
			"VALUES(?,?,?,?,?,?,?,?,?,?,?)",
			res,
			name, description, notify_key, subscribe,
			sqlite3::binary{password, USER_AUTH_KEY_LENGTH},
			sqlite3::binary{salt, USER_AUTH_SALT_LENGTH},
			telegram_bot_token,
			smtp_server.addr, smtp_server.port, smtp_server.user, smtp_server.password);
	if(rc != SQLITE_OK)
	{
		std::filesystem::remove(output);
		std::cerr << "- Error preparing 'DB' instance data [" << rc << " / " << sqlite3_errstr(rc) << "]\n";
		return EXIT_FAILURE;
	}

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		std::filesystem::remove(output);
		std::cerr << "- Error recording data to 'DB'. [" << rc << "]\n";
		return EXIT_FAILURE;
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
			std::cerr << "- Error preparing to insert group '" << group.name << "' [" << rc << "]\n";
			continue;
		}
		rc = res.step();
		if(rc != SQLITE_DONE)
		{
			std::cerr << "- Error inserting group '" << group.name << "' [" << rc << "]\n";
		}

		res.finalize();
		int groupid = static_cast<int>(db.last_insert_rowid());
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
		rc = db.prepare_bind("INSERT INTO sensor_type(name, long_name, type, "
				"unit, unit_name, description,add_change) "
				"VALUES(?,?,?,?,?,?,?)",
				res,
				sensor.name, sensor.long_name,
				static_cast<int>(sensor.type),
				sensor.unit, sensor.unit_name,
				sensor.description,
				sensor.add_change);
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

	return EXIT_SUCCESS;
}
