#ifndef AGRO_DAEMON_MAKE_MESSAGE_HPP__
#define AGRO_DAEMON_MAKE_MESSAGE_HPP__

#include "coap-te.hpp"
#include "rapidjson/document.h"
#include <string>

namespace Message{

enum type{
	resource = 0,
	response,
	error
};

std::string stringify(rapidjson::Document const&) noexcept;

template<typename Message>
bool add_device(rapidjson::Document&, Message const&) noexcept;
void add_device(rapidjson::Document&, CoAP::Message::Option::option const& op) noexcept;

template<typename Endpoint>
void add_endpoint(rapidjson::Document&, Endpoint const&) noexcept;

template<typename Message>
void add_resource(rapidjson::Document&, Message const&) noexcept;

bool add_type(rapidjson::Document&, type) noexcept;
bool add_transaction_status(rapidjson::Document&, CoAP::Transmission::status_t) noexcept;
void add_response_status(rapidjson::Document&, CoAP::Message::code) noexcept;

template<typename Message>
void add_payload(rapidjson::Document&, Message const&) noexcept;
void add_data(rapidjson::Document&, rapidjson::Value&) noexcept;

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
