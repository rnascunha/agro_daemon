#include "make.hpp"
#include "coap-te/helper/json/util.hpp"
#include "rapidjson/writer.h" // for stringify JSON

namespace Message{

bool add_type(rapidjson::Document& doc, type type_) noexcept
{
	config<type> const * t = get_type_config(type_);

	if(!t) return false;

	doc.AddMember("type", STRING_TO_JSON_VALUE(t->name, doc), doc.GetAllocator());
	return true;
}

bool add_transaction_status(rapidjson::Document& doc, CoAP::Transmission::status_t status) noexcept
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

	if(!str) return false;

	doc.AddMember("trans_status", STRING_TO_JSON_VALUE(str, doc), doc.GetAllocator());
	return true;
}

void add_response_status(rapidjson::Document& doc, CoAP::Message::code mcode) noexcept
{
	doc.AddMember("status", CoAP::Message::is_success(mcode), doc.GetAllocator());
}

void add_data(rapidjson::Document& doc, rapidjson::Value& data) noexcept
{
	doc.AddMember("data", data, doc.GetAllocator());
}

std::string stringify(rapidjson::Document const& doc) noexcept
{
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	doc.Accept(writer);

	return std::string{sb.GetString()};
}

}//Message
