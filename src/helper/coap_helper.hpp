#ifndef AGRO_DAEMON_HELPER_COAP_HPP__
#define AGRO_DAEMON_HELPER_COAP_HPP__

#include <string>
#include "coap-te.hpp"

std::string make_coap_path(std::vector<CoAP::Message::Option::option> const&) noexcept;
std::string make_coap_path(CoAP::Message::message const&) noexcept;

#endif /* AGRO_DAEMON_HELPER_COAP_HPP__ */
