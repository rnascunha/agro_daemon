#include "rapidjson/document.h"
#include "../message/make.hpp"

namespace Message{

std::string make_public_key(std::string_view const& public_key) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, type::user);
	doc.AddMember("command", rapidjson::Value("notify_key", doc.GetAllocator()).Move(), doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();

	data.AddMember("key",
			rapidjson::Value(public_key.data(), public_key.size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	add_data(doc, data);

	return stringify(doc);
}

}//Message
