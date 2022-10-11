//https://github.com/nlohmann/json/issues/506#issuecomment-286459077
#ifdef _MSC_VER
#define NOMINMAX
#endif /* _MSC_VER */

#include "auth_message.hpp"
#include "rapidjson/document.h"
#include "../../message/types.hpp"
#include "../../message/make.hpp"
#include "make.hpp"

namespace Agro{
namespace User{
namespace Message{

std::string user_error_authentication(std::error_code const& ec) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();
	add_type(doc, ::Message::type::user);
	doc.AddMember("command",
			rapidjson::Value("authenticate", doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();
	data.AddMember("authenticated", false, doc.GetAllocator());
	data.AddMember("error_message",
			rapidjson::Value(ec.message().data(), static_cast<int>(ec.message().size()), doc.GetAllocator()).Move(),
			doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string user_authentication(Agro::User::Logged const& user) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);
	doc.AddMember("command",
			rapidjson::Value("authenticate", doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	user_info(data, *user.user(), doc.GetAllocator());
	data.AddMember("authenticated", true, doc.GetAllocator());
	data.AddMember("sessionid",
			rapidjson::Value(user.session_id().data(), static_cast<int>(user.session_id().size()), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	data.AddMember("policy", user.policy_rules(), doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

}//Message
}//User
}//Agro
