#ifndef AGRO_DAEMON_USER_AUTH_IMPL_HPP__
#define AGRO_DAEMON_USER_AUTH_IMPL_HPP__

#include "../authenticate.hpp"
#include "../authenticate_params.h"
#include "../../error.hpp"
#include "../../message/types.hpp"
#include "../session_id.hpp"

#include <chrono>
#include <string>
#include <cstring>

#include <iostream>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "rapidjson/document.h"

namespace Agro{

static inline long time_epoch() noexcept
{
	return std::chrono::duration_cast<std::chrono::seconds>(
		        std::chrono::system_clock::now().time_since_epoch()).count();
}

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
		Message::user_commands& comm) noexcept
{
	auto const* typec = Message::get_type_config(doc["type"].GetString());
	if(!typec)
	{
		ec = make_error_code(Error::missing_field);
		return false;
	}

	if(typec->mtype != Message::type::user)
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

	auto const* user_type = Message::get_user_config(doc["command"].GetString());
	comm = user_type->mtype;
	switch(comm)
	{
		case Message::user_commands::autheticate:
			return check_authentice_package(payload, ec);
		case Message::user_commands::auth_session_id:
			return check_auth_session_id_package(payload, ec);
		default:
			ec = make_error_code(Error::invalid_value);
			break;
	}

	return false;
}

static bool check_user_status(User const& user,
		std::error_code& ec) noexcept
{
	switch(user.get_status())
	{
		case User::status::active:
			break;
		case User::status::inactive:
			ec = make_error_code(Error::user_is_inactive);
			return false;
		case User::status::suspended:
			ec = make_error_code(Error::user_is_suspended);
			return false;
		case User::status::deleted:
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
static bool check_password(User& user,
		rapidjson::Document const& doc,
		DB& db,
		std::error_code& ec) noexcept
{
	const char	*username = doc["data"]["user"].GetString(),
				*password = doc["data"]["password"].GetString();

	std::vector<unsigned char> salt, pass;
	user = db.get_user(username, salt, pass);
	if(!user)
	{
		ec = make_error_code(Error::user_not_found);
		return false;
	}

	if(!check_user_status(user, ec))
	{
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
static bool check_session_id(User& user,
		rapidjson::Document const& doc,
		DB& db,
		std::error_code& ec) noexcept
{
	std::string const username = doc["data"]["user"].GetString(),
					  sessionid = doc["data"]["sessionid"].GetString(),
					  user_agent = doc["data"]["user_agent"].GetString();

	user = db.get_user(username);
	if(!user)
	{
		ec = make_error_code(Error::user_not_found);
		return false;
	}

	if(!check_user_status(user, ec))
	{
		return false;
	}

	long session_time;
	if(!db.check_user_session_id(user,
					sessionid,
					user_agent,
					session_time))
	{
		ec = make_error_code(Error::session_expired);
		return false;
	}

	if((session_time + SessionTime) < time_epoch())
	{
		ec = make_error_code(Error::session_expired);
		return false;
	}
	return true;
}

template<std::size_t Iterations,
		std::size_t KeyLength,
		std::size_t SaltLength,
		long SessionTime>
bool authenticate(User& user,
		rapidjson::Document const& doc,
		DB& db,
		Message::user_commands& comm,
		std::error_code& ec) noexcept
{
	if(!is_auth_package(doc, ec, comm))
	{
		return false;
	}

	switch(comm)
	{
		case Message::user_commands::autheticate:
			return check_password<Iterations, KeyLength, SaltLength>(user, doc, db, ec);
		case Message::user_commands::auth_session_id:
			return check_session_id<SessionTime>(user, doc, db, ec);
		default:
			ec = make_error_code(Error::invalid_value);
			break;
	}

	return false;

}

template<unsigned SessionIDLenght>
bool create_session_id(User& user,
		rapidjson::Document const& doc,
		DB& db,
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
	if(!db.update_user_session_id(user, session_id, user_agent))
	{
		ec = make_error_code(Error::statement_error);
		return false;
	}

	user.authenticate(session_id, user_agent);

	return true;
}

}//Agro

#endif /* AGRO_DAEMON_USER_AUTH_IMPL_HPP__ */
