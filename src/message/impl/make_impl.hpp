#ifndef AGRO_DAEMON_MAKE_MESSAGE_IMPL_HPP__
#define AGRO_DAEMON_MAKE_MESSAGE_IMPL_HPP__

#include "../../device/helper.hpp"
#include "../make.hpp"

namespace Message{

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

template<typename Message>
void add_payload(rapidjson::Document& doc, Message const& msg) noexcept
{
	CoAP::Helper::payload_to_json(doc, msg.payload, msg.payload_len);
}

template<typename Number, typename Allocator>
rapidjson::Value& make_value(rapidjson::Value& v,
		Value<Number> const& value,
		Allocator& alloc) noexcept
{
	v.SetObject();
	v.AddMember("time", value.time, alloc);
	v.AddMember("value", value.value, alloc);

	return v;
}

template<typename Number, unsigned Max, typename Allocator>
void make_value_list_array(rapidjson::Value& data,
							Value_List<Number, Max> const& list,
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
void make_resource(rapidjson::Document& doc,
		Message const& msg,
		Endpoint const& ep,
		CoAP::Message::Option::option const& uri_host,
		rapidjson::Value& data) noexcept
{
	doc.SetObject();

	add_type(doc, type::resource);
	add_device(data, uri_host, doc.GetAllocator());
	add_resource(doc, msg);
	add_endpoint(doc, ep);
	add_data(doc, data);
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
	add_type(doc, type::response);
	add_resource(doc, request);
	add_endpoint(doc, ep);
	add_payload(doc, response);
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
