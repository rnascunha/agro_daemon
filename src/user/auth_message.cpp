#include "auth_message.hpp"
#include "rapidjson/document.h"
#include "../message/types.hpp"
#include "../message/make.hpp"

namespace Message{

std::string user_error_authentication(std::error_code const& ec) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();
	add_type(doc, type::user);
	doc.AddMember("command",
			rapidjson::Value("authenticate", doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();
	data.AddMember("authenticated", false, doc.GetAllocator());
	data.AddMember("error_message",
			rapidjson::Value(ec.message().data(), ec.message().size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());

	add_data(doc, data);

	return stringify(doc);
}

std::string user_authentication(Agro::User::Logged const& user) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();
	add_type(doc, type::user);
	doc.AddMember("command",
			rapidjson::Value("authenticate", doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();
	data.AddMember("authenticated", true, doc.GetAllocator());
	data.AddMember("name",
			rapidjson::Value(user.info()->name().data(), user.info()->name().size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	data.AddMember("email",
				rapidjson::Value(user.info()->email().data(), user.info()->email().size(), doc.GetAllocator()).Move(),
				doc.GetAllocator());
	data.AddMember("sessionid",
			rapidjson::Value(user.session_id().data(), user.session_id().size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());

	add_data(doc, data);

	return stringify(doc);
}

}//Message
