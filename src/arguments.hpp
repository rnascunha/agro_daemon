#ifndef AGRO_DAEMON_ARGUMENTS_HPP__
#define AGRO_DAEMON_ARGUMENTS_HPP__

#include <string>

struct arguments{
	std::string 	addr;
	int 			port;
	std::string 	coap_addr;
	int 			coap_port;
	int 			num_threads;
	std::string 	db_file;
	std::string 	notify_priv_key;
#if USE_SSL == 1
	std::string 	key;
	std::string 	cert;
#endif /* USE_SSL == 1 */
};

void usage(const char* program) noexcept;
void read_parameters(int argc, char** argv, arguments&) noexcept;

#endif /* AGRO_DAEMON_ARGUMENTS_HPP__ */
