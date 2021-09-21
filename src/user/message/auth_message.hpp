#ifndef AGRO_DAEMON_USER_AUTH_MESSAGE_HPP__
#define AGRO_DAEMON_USER_AUTH_MESSAGE_HPP__

#include <string>
#include <system_error>
#include "../user.hpp"

namespace Agro{
namespace User{
namespace Message{

std::string user_error_authentication(std::error_code const&) noexcept;
std::string user_authentication(Agro::User::Logged const&) noexcept;

}//Message
}//User
}//Agro

#endif /* AGRO_DAEMON_USER_AUTH_MESSAGE_HPP__ */
