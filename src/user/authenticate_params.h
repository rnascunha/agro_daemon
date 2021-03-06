#ifndef AGRO_DAEMON_USER_AUTH_PARAM_HPP__
#define AGRO_DAEMON_USER_AUTH_PARAM_HPP__

#define USER_AUTH_INTERATION_NUMBER		1007
#define USER_AUTH_KEY_LENGTH			32
#define USER_AUTH_SALT_LENGTH			32
#define USER_AUTH_HASH_ALGORITHM		EVP_sha256()

#define USER_ROOT_KEY_MINIMUM_LENGHT	6

#define USER_SESSION_ID_SIZE			32
#define USER_SESSION_TIME_SECONDS		(12 * 60 * 60)//12h

#endif /* AGRO_DAEMON_USER_AUTH_PARAM_HPP__ */
