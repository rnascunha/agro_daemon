#ifndef AGRO_DAEMON_USER_AUTH_IMPL_HPP__
#define AGRO_DAEMON_USER_AUTH_IMPL_HPP__

#include "../authenticate.hpp"
#include "../authenticate_params.h"
#include "../../error.hpp"
#include "../../message/types.hpp"
#include "../session_id.hpp"
#include "../../helper/time_helper.hpp"

#include <string>
#include <cstring>

#include <iostream>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "rapidjson/document.h"

namespace Agro{

static bool check_authentice_package(
		rapidjson::Value const& payload,
		std::error_code& ec) noexcept
{
	if(!(payload.HasMember("user") && payload["user"].IsString())
		|| !(payload.HasMember("password") && payload["password"].IsString()))
	{
		ec = make_error_code(Error::ill_formed);
		return false;
	}
	return true;
}

static bool check_auth_session_id_package(
		rapidjson::Value const& payload,
		std::error_code& ec) noexcept
{
	if(!(payload.HasMember("user") && payload["user"].IsString())
		|| !(payload.HasMember("sessionid") && payload["sessionid"].IsString())
		|| !(payload.HasMember("user_agent") && payload["user_agent"].IsString()))
	{
		ec = make_error_code(Error::ill_formed);
		return false;
	}
	return true;
}

static bool is_auth_package(rapidjson::Document const& doc,
		std::error_code& ec,
		::Message::user_commands& comm) noexcept
{
	std::cout << "1 " << doc["type"].GetString() << "\n";
	auto const* typec = ::Message::get_type_config(doc["type"].GetString());
	std::cout << "1.1\n";
	if(!typec)
	{
		std::cout << "2\n";
		ec = make_error_code(Error::missing_field);
		return false;
	}
	std::cout << "3\n";

	if(typec->mtype != ::Message::type::user)
	{
		ec = make_error_code(Error::invalid_value);
		return false;
	}

	if(!doc.HasMember("command")
		|| !doc["command"].IsString())
	{
		ec = make_error_code(Error::invalid_value);
		return false;
	}

	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		ec = make_error_code(Error::ill_formed);
		return false;
	}
	rapidjson::Value const& payload = doc["data"].GetObject();

	::Message::config<::Message::user_commands> const* user_type = ::Message::get_user_config(doc["command"].GetString());
	comm = user_type->mtype;
	switch(comm)
	{
		case ::Message::user_commands::autheticate:
			return check_authentice_package(payload, ec);
		case ::Message::user_commands::auth_session_id:
			return check_auth_session_id_package(payload, ec);
		default:
			ec = make_error_code(Error::invalid_value);
			break;
	}

	return false;
}

static bool check_user_status(User::Logged const& user,
		std::error_code& ec) noexcept
{
	switch(user.user()->info().get_status())
	{
		case User::Info::status::active:
			break;
		case User::Info::status::inactive:
			ec = make_error_code(Error::user_is_inactive);
			return false;
		case User::Info::status::suspended:
			ec = make_error_code(Error::user_is_suspended);
			return false;
		case User::Info::status::deleted:
			ec = make_error_code(Error::user_is_deleted);
			return false;
		default:
			ec = make_error_code(Error::invalid_value);
			return false;
	}
	return true;
}

template<std::size_t Iterations,
		std::size_t KeyLength,
		std::size_t SaltLength>
static bool check_password(User::Logged& user,
		rapidjson::Value const& payload,
		instance& instance,
		std::error_code& ec) noexcept
{
	const char	*password = payload["password"].GetString();

	if(!check_user_status(user, ec))
	{
		return false;
	}

	std::vector<unsigned char> salt, pass;
	if(!instance.get_user_password(user.user()->info().username(), salt, pass))
	{
		ec = make_error_code(Error::internal_error);
		return false;
	}

	if(pass.size() != KeyLength)
	{
		ec = make_error_code(Error::password_not_match);
		return false;
	}

	unsigned char key[KeyLength] = {0};
	if(!PKCS5_PBKDF2_HMAC(password, std::strlen(password),
			salt.data(), salt.size(),
			Iterations, USER_AUTH_HASH_ALGORITHM,
			KeyLength, key))
	{
		ec = make_error_code(Error::internal_error);
		return false;
	}

	if(std::memcmp(key, pass.data(), KeyLength) != 0)
	{
		ec = make_error_code(Error::password_not_match);
		return false;
	}

	user.authenticate();

	return true;
}

template<long SessionTime>
static bool check_session_id(User::Logged& user,
		rapidjson::Value const& payload,
		instance const& instance,
		std::error_code& ec) noexcept
{
	std::string const sessionid = payload["sessionid"].GetString(),
					  user_agent = payload["user_agent"].GetString();

	if(!check_user_status(user, ec))
	{
		return false;
	}

	long session_time;
	if(!instance.check_user_session_id(
					user.id(),
					sessionid,
					user_agent,
					session_time))
	{
		ec = make_error_code(Error::session_expired);
		return false;
	}

	if((session_time + SessionTime) < time_epoch_seconds())
	{
		ec = make_error_code(Error::session_expired);
		return false;
	}

	user.authenticate();

	return true;
}

template<std::size_t Iterations,
		std::size_t KeyLength,
		std::size_t SaltLength,
		long SessionTime>
bool authenticate(User::Logged& user,
		rapidjson::Document const& doc,
		instance& instance,
		::Message::user_commands& comm,
		std::error_code& ec) noexcept
{
	//Checking package
	if(!is_auth_package(doc, ec, comm))
	{
		return false;
	}

	//Getting user info
	rapidjson::Value const& payload = doc["data"].GetObject();

	std::string const username = payload["user"].GetString();
	user.user(instance.get_user(username));
	if(!user.user())
	{
		ec = make_error_code(Error::user_not_found);
		return false;
	}

	switch(comm)
	{
		case ::Message::user_commands::autheticate:
			return check_password<Iterations, KeyLength, SaltLength>(user, payload, instance, ec);
		case ::Message::user_commands::auth_session_id:
			return check_session_id<SessionTime>(user, payload, instance, ec);
		default:
			ec = make_error_code(Error::invalid_value);
			break;
	}

	return false;
}

template<unsigned SessionIDLenght>
bool create_session_id(User::Logged& user,
		rapidjson::Document const& doc,
		instance& instance,
		std::error_code& ec) noexcept
{
	rapidjson::Value const& payload = doc["data"].GetObject();
	if(!payload.HasMember("user_agent") || !payload["user_agent"].IsString())
	{
		ec = make_error_code(Error::missing_field);
		return false;
	}

	std::string const user_agent = payload["user_agent"].GetString();
	std::string const session_id = Agro::generate_session_id<SessionIDLenght>();

	if(!instance.update_user_session_id(user.id(), session_id, user_agent))
	{
		ec = make_error_code(Error::statement_error);
		return false;
	}

	user.authenticate(session_id, user_agent);

	return true;
}

}//Agro

#endif /* AGRO_DAEMON_USER_AUTH_IMPL_HPP__ */
