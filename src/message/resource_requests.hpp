#ifndef AGRO_DAEMON_RESOURCE_REQUESTS_HPP__
#define AGRO_DAEMON_RESOURCE_REQUESTS_HPP__

#include "make.hpp"
#include "../resources/types.hpp"
#include <string>

namespace Message{

template<typename Endpoint,
		typename Message>
std::string config_to_json(Endpoint const& ep,
							Message const&,
							CoAP::Message::Option::option const& op,
							Resource::config const& cfg) noexcept;

template<typename Endpoint,
		typename Message>
std::string status_to_json(Endpoint const&,
						Message const&,
						CoAP::Message::Option::option const&,
						Resource::status const&) noexcept;

template<typename Endpoint,
		typename Message>
std::string route_to_json(Endpoint const& ep,
						Message const&,
						CoAP::Message::Option::option const& op,
						Resource::route const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept;

template<typename Endpoint,
		typename Message>
std::string full_config_to_json(Endpoint const& ep,
						Message const&,
						CoAP::Message::Option::option const& op,
						Resource::full_config const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept;

template<typename Endpoint,
		typename Message>
std::string board_config_to_json(Endpoint const&,
						Message const&,
						CoAP::Message::Option::option const&,
						Resource::board_config const&,
						const char* version,
						std::size_t version_len) noexcept;

}//Message

#include "impl/resource_request_impl.hpp"

#endif /* AGRO_DAEMON_RESOURCE_REQUESTS_HPP__ */
