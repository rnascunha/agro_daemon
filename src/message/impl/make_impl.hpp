#ifndef AGRO_DAEMON_MAKE_MESSAGE_IMPL_HPP__
#define AGRO_DAEMON_MAKE_MESSAGE_IMPL_HPP__

#include "../../device/helper.hpp"
#include "../make.hpp"
#include "coap-te-debug.hpp"

namespace Message{

template<typename Allocator>
void make_array(rapidjson::Value& data, const std::uint8_t* arr, std::size_t size, Allocator& alloc) noexcept
{
	data.SetArray();
	for(std::size_t i = 0; i < size; i++)
	{
		data.PushBack(arr[i], alloc);
	}
}

template<typename Message, typename Allocator>
bool add_device(rapidjson::Value& value, Message const& msg, Allocator& alloc) noexcept
{
	CoAP::Message::Option::option op;
	if(!CoAP::Message::Option::get_option(msg, op, CoAP::Message::Option::code::uri_host))
		return false;

	value.AddMember("device",
			rapidjson::Value(static_cast<const char*>(op.value), op.length, alloc).Move(),
			alloc);

	return true;
}

template<typename Allocator>
void add_device(rapidjson::Value& data, mesh_addr_t const& addr, Allocator& alloc) noexcept
{
	char buf[18];
	data.AddMember("device",
			rapidjson::Value(addr.to_string(buf, 18), alloc).Move(),
			alloc);
}

template<typename Allocator>
void add_device(rapidjson::Value& value, CoAP::Message::Option::option const& op, Allocator& alloc) noexcept
{
	value.AddMember("device",
			rapidjson::Value(static_cast<const char*>(op.value), op.length,
					alloc).Move(),
				alloc);
}

template<typename Allocator>
void add_device(rapidjson::Document& doc, mesh_addr_t const& mesh, Allocator& alloc) noexcept
{
	char addr[18];
	snprintf(addr, 18, MACSTR, MAC2STR(mesh.addr));
	doc.AddMember("device",
				rapidjson::Value(addr,
					alloc).Move(),
					alloc);
}

template<typename Endpoint>
void add_endpoint(rapidjson::Document& doc, Endpoint const& ep) noexcept
{
	add_endpoint(doc, ep, doc.GetAllocator());
}

template<typename Endpoint, typename Allocator>
void add_endpoint(rapidjson::Value& data, Endpoint const& ep, Allocator& alloc) noexcept
{
	rapidjson::Value epv;
	epv.SetObject();

	char buffer[50];
	ep.address(buffer, 50);
	epv.AddMember("addr", rapidjson::Value(buffer, alloc).Move(), alloc);
	epv.AddMember("port", ep.port(), alloc);

	data.AddMember("endpoint", epv, alloc);
}

template<typename Message>
void add_resource(rapidjson::Document& doc, Message const& msg) noexcept
{
	CoAP::Message::Option::Parser parser{msg};

	rapidjson::Value resv;
	resv.SetArray();

	CoAP::Message::Option::option const* op;
	while((op = parser.next()))
	{
		if(op->ocode == CoAP::Message::Option::code::uri_path)
		{
			rapidjson::Value uri{static_cast<const char*>(op->value), op->length, doc.GetAllocator()};
			resv.PushBack(uri, doc.GetAllocator());
		}
	}

	doc.AddMember("resource", resv, doc.GetAllocator());
}

template<typename Message, typename Allocator>
void add_option(rapidjson::Value& data,
		Message const& msg,
		CoAP::Message::Option::code op_code,
		Allocator& alloc) noexcept
{
	data.SetArray();

	CoAP::Message::Option::Parser parser{msg};

	CoAP::Message::Option::option const* op;
	while((op = parser.next()) != nullptr)
	{
		if(op->ocode == op_code)
		{
			rapidjson::Value opt{static_cast<const char*>(op->value), op->length, alloc};
			data.PushBack(opt, alloc);
		}
	}
}

template<typename Message, typename Allocator>
void add_payload(rapidjson::Value& data,
				Message const& msg,
				Allocator& alloc) noexcept
{
	data.SetArray();
	std::uint8_t const* p8u = static_cast<std::uint8_t const*>(msg.payload);
	for(std::size_t i = 0; i < msg.payload_len; i++)
	{
		data.PushBack(p8u[i], alloc);
	}
}

template<typename Message, typename Allocator>
void add_request_type(rapidjson::Value& data, Message const& request, Allocator& alloc) noexcept
{
	switch(request.mcode)
	{
		case CoAP::Message::code::get:
			data.AddMember("type", rapidjson::Value("get", alloc).Move(), alloc);
			break;
		case CoAP::Message::code::post:
			data.AddMember("type", rapidjson::Value("post", alloc).Move(), alloc);
			break;
		case CoAP::Message::code::put:
			data.AddMember("type", rapidjson::Value("put", alloc).Move(), alloc);
			break;
		case CoAP::Message::code::cdelete:
			data.AddMember("type", rapidjson::Value("delete", alloc).Move(), alloc);
			break;
		default:
			break;
	}
}

template<typename Allocator>
void add_transaction_status(rapidjson::Value& data, CoAP::Transmission::status_t status, Allocator& alloc) noexcept
{
	using namespace CoAP::Transmission;
	const char* str = status == CoAP::Transmission::status_t::empty ?
					"empty" :
					(status == status_t::canceled ?
							"canceled" :
							(status == status_t::success ?
									"success" :
									(status == status_t::timeout ?
											"timeout" : nullptr)));

	if(!str)
	{
		data.AddMember("trans_status", rapidjson::Value(), alloc);
		return;
	}

	data.AddMember("trans_status", rapidjson::Value(str, alloc).Move(), alloc);
}

template<typename Allocator>
void add_response_status(rapidjson::Value& data, CoAP::Message::code mcode, Allocator& alloc) noexcept
{
	data.AddMember("success", CoAP::Message::is_success(mcode), alloc);
	data.AddMember("code",
			rapidjson::Value(CoAP::Debug::code_string(mcode), alloc).Move(),
			alloc);
}

template<typename Message>
void add_payload(rapidjson::Document& doc, Message const& msg) noexcept
{
	CoAP::Helper::payload_to_json(doc, msg.payload, msg.payload_len);
}

template<typename Number, typename Allocator>
rapidjson::Value& make_value(rapidjson::Value& v,
		Agro::Sensor::Value<Number> const& value,
		Allocator& alloc) noexcept
{
	v.SetObject();
	v.AddMember("time", value.time, alloc);
	v.AddMember("value", value.value, alloc);

	return v;
}

template<typename Number, unsigned Max, typename Allocator>
void make_value_list_array(rapidjson::Value& data,
							Agro::Sensor::Value_List<Number, Max> const& list,
							Allocator& alloc) noexcept
{
	data.SetArray();
	for(auto const& r : list)
	{
		rapidjson::Value v;
		v.SetObject();
		v.AddMember("time", r.time, alloc);
		v.AddMember("value", r.value, alloc);

		data.PushBack(v, alloc);
	}
}

template<typename Message,
		typename Endpoint>
void make_response(rapidjson::Document& doc,
		Message const& request,
		Message const& response,
		Endpoint const& ep,
		CoAP::Transmission::status_t status) noexcept
{
	doc.SetObject();

	add_device(doc, request, doc.GetAllocator());
	add_type(doc, type::device);
	add_resource(doc, request);
	add_endpoint(doc, ep);
	add_transaction_status(doc, status);
	add_response_status(doc, response.mcode);
	add_payload(doc, response);
}

template<typename Message,
		typename Endpoint>
void make_response(rapidjson::Document& doc,
		Message const& request,
		Endpoint const& ep,
		CoAP::Transmission::status_t status) noexcept
{
	doc.SetObject();

	add_device(doc, request, doc.GetAllocator());
	add_resource(doc, request);
	add_endpoint(doc, ep);
	add_transaction_status(doc, status);
}

}//Message

#endif /* AGRO_DAEMON_MAKE_MESSAGE_IMPL_HPP__ */
