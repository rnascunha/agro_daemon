#include "agro.hpp"
#include "../helper/time_helper.hpp"
#include "../user/session_id.hpp"
#include "../user/authenticate_params.h"
#include "tt/tt.hpp"

//https://github.com/Tencent/rapidjson/issues/1448
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#undef GetObject
#endif

static bool check_authentice_package(
		rapidjson::Value const& payload,
		std::error_code& ec) noexcept;
static bool check_auth_session_id_package(
		rapidjson::Value const& payload,
		std::error_code& ec) noexcept;
static bool is_auth_package(rapidjson::Document const& doc,
		std::error_code& ec,
		::Message::user_commands& comm) noexcept;
static bool check_user_status(Agro::User::Logged const& user,
		std::error_code& ec) noexcept;
static bool check_password(Agro::User::Logged& user,
		rapidjson::Value const& payload,
		Agro::instance& instance,
		std::error_code& ec) noexcept;
static bool check_session_id(Agro::User::Logged& user,
		rapidjson::Value const& payload,
		Agro::instance const& instance,
		std::error_code& ec) noexcept;

namespace Agro{

bool instance::authenticate(User::Logged& user,
		rapidjson::Document const& doc,
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

	auto* user_ptr = get_user(username);
	if(!user_ptr)
	{
		ec = make_error_code(Error::user_not_found);
		return false;
	}
	user.user(user_ptr);

	switch(comm)
	{
		case ::Message::user_commands::autheticate:
			return check_password(user, payload, *this, ec);
		case ::Message::user_commands::auth_session_id:
			return check_session_id(user, payload, *this, ec);
		default:
			ec = make_error_code(Error::invalid_value);
			break;
	}

	return false;
}

bool instance::create_session_id(User::Logged& user,
		rapidjson::Document const& doc,
		std::error_code& ec) noexcept
{
	rapidjson::Value const& payload = doc["data"].GetObject();
	if(!payload.HasMember("user_agent") || !payload["user_agent"].IsString())
	{
		ec = make_error_code(Error::missing_field);
		return false;
	}

	std::string const user_agent = payload["user_agent"].GetString();
	std::string const session_id = Agro::generate_session_id<USER_SESSION_ID_SIZE>();

	if(!update_user_session_id(user.id(), session_id, user_agent))
	{
		ec = make_error_code(Error::statement_error);
		return false;
	}

	user.authenticate(session_id, user_agent);

	return true;
}

}//Agro


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
	auto const* typec = ::Message::get_type_config(doc["type"].GetString());
	if(!typec)
	{
		ec = make_error_code(Error::missing_field);
		return false;
	}

	if(typec->type != ::Message::type::user)
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

	auto const* user_type = ::Message::get_user_config(doc["command"].GetString());
	comm = user_type->type;
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

static bool check_user_status(Agro::User::Logged const& user,
		std::error_code& ec) noexcept
{
	switch(user.user()->info().get_status())
	{
		case Agro::User::Info::status::active:
			break;
		case Agro::User::Info::status::inactive:
			ec = make_error_code(Error::user_is_inactive);
			return false;
		case Agro::User::Info::status::suspended:
			ec = make_error_code(Error::user_is_suspended);
			return false;
		case Agro::User::Info::status::deleted:
			ec = make_error_code(Error::user_is_deleted);
			return false;
		default:
			ec = make_error_code(Error::invalid_value);
			return false;
	}
	return true;
}

static bool check_password(Agro::User::Logged& user,
		rapidjson::Value const& payload,
		Agro::instance& instance,
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

	if(pass.size() != USER_AUTH_KEY_LENGTH)
	{
		ec = make_error_code(Error::password_not_match);
		return false;
	}

	unsigned char key[USER_AUTH_KEY_LENGTH] = {0};
	if(!PKCS5_PBKDF2_HMAC(password, static_cast<int>(std::strlen(password)),
			salt.data(), static_cast<int>(salt.size()),
			USER_AUTH_INTERATION_NUMBER, USER_AUTH_HASH_ALGORITHM,
			USER_AUTH_KEY_LENGTH, key))
	{
		ec = make_error_code(Error::internal_error);
		return false;
	}

	if(std::memcmp(key, pass.data(), USER_AUTH_KEY_LENGTH) != 0)
	{
		ec = make_error_code(Error::password_not_match);
		return false;
	}

	user.authenticate();

	return true;
}

static bool check_session_id(Agro::User::Logged& user,
		rapidjson::Value const& payload,
		Agro::instance const& instance,
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

	if((session_time + USER_SESSION_TIME_SECONDS) < time_epoch_seconds())
	{
		ec = make_error_code(Error::session_expired);
		return false;
	}

	user.authenticate();

	return true;
}
