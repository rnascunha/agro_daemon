#include "info.hpp"

namespace Message{

void add_info(rapidjson::Document& doc, info inf) noexcept
{
	config<info> const* conf = get_config(inf);
	if(!conf) return;

	doc.AddMember("info", rapidjson::Value(conf->name, doc.GetAllocator()).Move(), doc.GetAllocator());
}

void make_info(rapidjson::Document& doc, info inf, const char* message) noexcept
{
	doc.SetObject();
	add_type(doc, type::info);
	add_info(doc, inf);
	doc.AddMember("message", rapidjson::Value(message, doc.GetAllocator()).Move(), doc.GetAllocator());
}

void make_info(rapidjson::Document& doc, info inf, mesh_addr_t const& addr, const char* message) noexcept
{
	make_info(doc, inf, message);
	add_device(doc, addr);
}

void make_info(rapidjson::Document& doc, info inf, CoAP::Message::Option::option const& addr, const char* message) noexcept
{
	make_info(doc, inf, message);
	add_device(doc, addr);
}

std::string make_info(info inf, const char* message) noexcept
{
	rapidjson::Document doc;
	make_info(doc, inf, message);

	return stringify(doc);
}

std::string make_info(info inf, mesh_addr_t const& addr, const char* message) noexcept
{
	rapidjson::Document doc;
	make_info(doc, inf, addr, message);

	return stringify(doc);
}

std::string make_info(info inf, CoAP::Message::Option::option const& addr, const char* message) noexcept
{
	rapidjson::Document doc;
	make_info(doc, inf, addr, message);

	return stringify(doc);
}

}//Message
