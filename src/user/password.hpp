#ifndef AGRO_DAEMON_USER_PASSWORD_HPP__
#define AGRO_DAEMON_USER_PASSWORD_HPP__

#include <string>
#include "authenticate_params.h"


namespace Agro{
namespace User{

using salt_password = unsigned char[USER_AUTH_SALT_LENGTH];
using key_password = unsigned char[USER_AUTH_KEY_LENGTH];

void generate_salt(salt_password) noexcept;
bool create_password(std::string const& password,
					salt_password,
					key_password) noexcept;

}//User
}//Agro

#endif /* AGRO_DAEMON_USER_PASSWORD_HPP__ */
