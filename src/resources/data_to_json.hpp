#ifndef AGRO_MESH_MESSAGE_DATA_TO_JSON_HPP__
#define AGRO_MESH_MESSAGE_DATA_TO_JSON_HPP__

#include "types.hpp"
#include <string>
#include "../coap_engine.hpp"
#include "coap-te.hpp"

std::string config_to_json(engine::endpoint const& ep,
							CoAP::Message::Option::node const& op,
							config const& cfg) noexcept;
std::string status_to_json(engine::endpoint const&,
						CoAP::Message::Option::node const&,
						status const&) noexcept;
std::string route_to_json(engine::endpoint const& ep,
						CoAP::Message::Option::node const& op,
						route const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept;
std::string full_config_to_json(engine::endpoint const& ep,
						CoAP::Message::Option::node const& op,
						full_config const& config,
						const uint8_t* children,
						std::size_t children_size) noexcept;

#endif /* AGRO_MESH_MESSAGE_DATA_TO_JSON_HPP__ */
