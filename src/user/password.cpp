#include "password.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace Agro{
namespace User{

bool validate_password(std::string const& password) noexcept
{
	return password.length() >= 6;
}

void generate_salt(salt_password salt) noexcept
{
	RAND_bytes(salt, USER_AUTH_SALT_LENGTH);
}

bool create_password(std::string const& password,
					salt_password salt,
					key_password key) noexcept
{
	generate_salt(salt);
	return PKCS5_PBKDF2_HMAC(password.data(), password.size(),
			salt, USER_AUTH_SALT_LENGTH,
			USER_AUTH_INTERATION_NUMBER, USER_AUTH_HASH_ALGORITHM,
			USER_AUTH_KEY_LENGTH, key);
}

}//User
}//Agro
