#ifndef AGRO_DAEMON_USER_AUTH_HPP__
#define AGRO_DAEMON_USER_AUTH_HPP__

#include <system_error>
#include <cstdlib>

#include "rapidjson/document.h"
#include "../message/user_types.hpp"
#include "../db/db.hpp"

namespace Agro{

namespace User{
class Logged;
}


template<std::size_t Iterations,
		std::size_t KeyLength,
		std::size_t SaltLength,
		long SessionTime>
bool authenticate(User::Logged&,
		rapidjson::Document const&,
		instance&,
		::Message::user_commands&,
		std::error_code&) noexcept;
template<unsigned SessionIDLenght>
bool create_session_id(User::Logged& user,
		rapidjson::Document const& doc,
		instance& db,
		std::error_code& ec) noexcept;

}//Agro

#include "impl/authenticate_impl.hpp"

#endif /* AGRO_DAEMON_USER_AUTH_HPP__ */
