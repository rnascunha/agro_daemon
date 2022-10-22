#include "coap_helper.hpp"
#include <sstream>

std::string make_coap_path(std::vector<CoAP::Message::Option::option> const& options) noexcept
{
	std::stringstream ss{};
	bool has_query = false;

	for(auto const& op : options)
	{
		if(op.ocode == CoAP::Message::Option::code::uri_path)
		{
			ss << "/" << std::string{static_cast<const char*>(op.value), op.length};
		}
		else if(op.ocode == CoAP::Message::Option::code::uri_query)
		{
			ss << (has_query ? "?" : "&");
			ss << std::string{static_cast<const char*>(op.value), op.length};
			has_query = true;
		}
	}

	return ss.str();
}

std::string make_coap_path(CoAP::Message::message const& message) noexcept
{
	std::stringstream ss{};
	bool has_query = false;

	CoAP::Message::Option::Parser<> parser{message};
	CoAP::Message::Option::option const *op;
	while((op = parser.next()) != nullptr)
	{
		if(op->ocode == CoAP::Message::Option::code::uri_path)
		{
			ss << "/" << std::string{static_cast<const char*>(op->value), op->length};
		}
		else if(op->ocode == CoAP::Message::Option::code::uri_query)
		{
			ss << (has_query ? "?" : "&");
			ss << std::string{static_cast<const char*>(op->value), op->length};
			has_query = true;
		}
	}

	return ss.str();
}
