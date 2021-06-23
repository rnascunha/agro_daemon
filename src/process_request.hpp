#ifndef AGRO_MESH_PROCESS_REQUEST_HPP__
#define AGRO_MESH_PROCESS_REQUEST_HPP__

#include "coap_engine.hpp"
#include <string>

void process_request(engine&, std::string&& str) noexcept;

#endif /* AGRO_MESH_PROCESS_REQUEST_HPP__ */
