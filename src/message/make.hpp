#ifndef AGRO_DAEMON_MAKE_MESSAGE_HPP__
#define AGRO_DAEMON_MAKE_MESSAGE_HPP__

#include "coap-te.hpp"
#include "rapidjson/document.h"
#include <string>
#include "../device/types.hpp"
#include "types.hpp"
#include "../sensor/value.hpp"

namespace Message{

std::string stringify(rapidjson::Document const&) noexcept;

template<typename Message, typename Allocator>
bool add_device(rapidjson::Value&, Message const&, Allocator&) noexcept;
template<typename Allocator>
void add_device(rapidjson::Value&, CoAP::Message::Option::option const& op, Allocator&) noexcept;
template<typename Allocator>
void add_device(rapidjson::Value&, mesh_addr_t const&, Allocator&) noexcept;
template<typename Allocator>
void add_device(rapidjson::Value&, mesh_addr_t const&, Allocator&) noexcept;

template<typename Endpoint>
void add_endpoint(rapidjson::Document&, Endpoint const&) noexcept;
template<typename Endpoint, typename Allocator>
void add_endpoint(rapidjson::Value&, Endpoint const&, Allocator&) noexcept;

template<typename Message>
void add_resource(rapidjson::Document&, Message const&) noexcept;

bool add_type(rapidjson::Document&, type) noexcept;
bool add_transaction_status(rapidjson::Document&, CoAP::Transmission::status_t) noexcept;
void add_response_status(rapidjson::Document&, CoAP::Message::code) noexcept;

template<typename Message, typename Allocator>
void add_request_type(rapidjson::Value&, Message const& request, Allocator&) noexcept;

template<typename Message, typename Allocator>
void add_option(rapidjson::Value& data,
		Message const& msg,
		CoAP::Message::Option::code op_code,
		Allocator& alloc) noexcept;
template<typename Message, typename Allocator>
void add_payload(rapidjson::Value& data,
				Message const& msg,
				Allocator& alloc) noexcept;
template<typename Allocator>
void add_transaction_status(rapidjson::Value&, CoAP::Transmission::status_t, Allocator&) noexcept;
template<typename Allocator>
void add_response_status(rapidjson::Value&, CoAP::Message::code, Allocator&) noexcept;

template<typename Message>
void add_payload(rapidjson::Document&, Message const&) noexcept;
void add_data(rapidjson::Document&, rapidjson::Value&) noexcept;

template<typename Number, typename Allocator>
rapidjson::Value& make_value(rapidjson::Value&, Agro::Sensor::Value<Number> const&, Allocator&) noexcept;

template<typename Number, unsigned Max, typename Allocator>
void make_value_list_array(rapidjson::Value&,
							Agro::Sensor::Value_List<Number, Max> const&,
							Allocator&) noexcept;

template<typename Message,
		typename Endpoint>
void make_resource(rapidjson::Document&,
		Message const&,
		Endpoint const&,
		CoAP::Message::Option::option const& uri_host,
		rapidjson::Value& data = rapidjson::Value()) noexcept;

template<typename Message,
		typename Endpoint>
void make_response(rapidjson::Document&,
		Message const& request,
		Message const& response,
		Endpoint const&,
		CoAP::Transmission::status_t) noexcept;

template<typename Message,
		typename Endpoint>
void make_response(rapidjson::Document&,
		Message const& request,
		Endpoint const&,
		CoAP::Transmission::status_t) noexcept;

template<typename Message,
		typename Endpoint>
void make_error(rapidjson::Document&, Message const&, Endpoint const&) noexcept;

}//Message

#include "impl/make_impl.hpp"

#endif /* AGRO_DAEMON_MAKE_MESSAGE_HPP__ */
