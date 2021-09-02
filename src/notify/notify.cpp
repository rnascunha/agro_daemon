#include "notify.hpp"
#include <filesystem>
#include "pusha.h"
#include "pusha.hpp"
#include <fstream>
#include <iostream>
#include "notify_request.hpp"

static const std::filesystem::path private_key{"priv.pem"};
static pusha::key notify_key;
static bool is_notify_enable = false;
static const std::filesystem::path user_dir{"user"};
static const std::filesystem::path user_data{"user/user.config"};
static const std::string_view subscriber{"mailto:email@company.com"};
static notify_factory* factory = NULL;

static void create_user_directory() noexcept
{
	if(std::filesystem::exists(user_dir) &&
			std::filesystem::is_directory(user_dir))
	{
		std::cout << "User directory already exists\n";
	}
	else
	{
		std::error_code ec;
		std::filesystem::create_directory(user_dir, ec);
		if(ec)
		{
			std::cerr << "Error creating images directory: "
					<< ec.value() << "/" << ec.message() << "\n";
		}
	}
	std::cout << "Created user directory\n";
}

bool init_notify(notify_factory& factory_) noexcept
{
	create_user_directory();

	factory = &factory_;

	is_notify_enable = notify_key.import(private_key);
	if(is_notify_enable)
	{
		std::cout << "Notify system initiated\n";
		return true;
	}

	std::cerr << "Error initiating notify system\n";
	return false;
}

void remove_subscription() noexcept
{
	std::filesystem::remove(user_data);
}

void add_subscription(const char* endpoint, const char* p256dh, const char* auth) noexcept
{
	std::ofstream ofs{user_data.string()};
	if(!ofs)
	{
		std::cerr << "Erro opening 'user_data' file\n";
		return;
	}

	ofs << endpoint << "\n" << p256dh << "\n" << auth;
	ofs.close();
}

static bool read_notify_file(std::string& endpoint,
							std::string& p256dh,
							std::string& auth) noexcept
{
	std::ifstream ifs{user_data.string()};
	if(!ifs)
	{
		std::cerr << "Erro opening 'user_data' file\n";
		return false;
	}

	getline(ifs, endpoint);
	if(ifs.eof()) goto end;
	getline(ifs, p256dh);
	if(ifs.eof()) goto end;
	getline(ifs, auth);
end:
	ifs.close();
	if(endpoint.empty() || p256dh.empty() || p256dh.empty())
	{
		std::cerr << "Erro reading 'user_data' file\n";
		return false;
	}

	return true;
}

bool notify(const void* payload, size_t size, unsigned expiration /* = 0 */, unsigned ttl /* = 0 */) noexcept
{
	if(!is_notify_enable) return false;

	std::string endpoint, p256dh, auth;
	if(!read_notify_file(endpoint, p256dh, auth))
		return false;

	if(!expiration)
	{
		expiration = std::time(NULL) + (12 * 60 * 60);
	}

	if(!ttl)
	{
		ttl = 60;
	}

	pusha_http_request req;
	int err = pusha_notify_http(&req,
			notify_key.get_key(),
			endpoint.data(), endpoint.size(),
			subscriber.data(), subscriber.size(),
			p256dh.data(), p256dh.size(),
			auth.data(), auth.size(),
			expiration, ttl,
			payload, size,
			pusha_HTTPver_1_1);

	if(err)
	{
		std::cerr << "Error making 'pusha' http request\n";
		return false;
	}

	std::size_t packet_size;
	std::uint8_t * packet = http_request_serialize2(&req, &packet_size);
	free_http_request(&req);
	if(!packet)
	{
		std::cerr << "Error serializing notify packet\n";
		return false;
	}
	std::cout << "Notify serialized [" << packet_size << "]\n";

	factory->request(endpoint, packet, packet_size);

	return true;
}

bool notify(std::string const& payload, unsigned expiration /* = 0 */, unsigned ttl /* = 0 */) noexcept
{
	return notify(payload.data(), payload.size(), expiration, ttl);
}
