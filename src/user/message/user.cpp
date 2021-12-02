#include "user.hpp"
#include "rapidjson/document.h"
#include "../../message/types.hpp"
#include "../../message/user_types.hpp"
#include "../../message/make.hpp"
#include "make.hpp"

namespace Agro{
namespace User{
namespace Message{

std::string user_group_permissions(Logged const&, instance const& instance) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);

	auto const* config = get_config(::Message::user_commands::user_group_policies);
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();

	rapidjson::Value ulist;
	user_list(ulist, instance.users(), doc.GetAllocator());
	data.AddMember("users", ulist.Move(), doc.GetAllocator());

	rapidjson::Value glist;
	group_list(glist, instance.users().groups(), doc.GetAllocator());
	data.AddMember("groups", glist.Move(), doc.GetAllocator());

//	rapidjson::Value plist;
//	permission_list(plist, instance.permissions(), doc.GetAllocator());
//	data.AddMember("policies", plist.Move(), doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string added_new_user(User const& user, std::vector<group_id> const& gid_list) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);

	auto const* config = get_config(::Message::user_commands::add_user);
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	user_info(data, user, doc.GetAllocator());

	/**
	 * Adding groups
	 */
	rapidjson::Value groups;
	groups.SetArray();
	for(auto const& gid : gid_list)
	{
		groups.PushBack(gid, doc.GetAllocator());
	}
	data.AddMember("groups", groups, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string edited_user(User const& user, std::vector<group_id> const& gid_list) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);

	auto const* config = get_config(::Message::user_commands::edit_user);
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	user_info(data, user, doc.GetAllocator());

	/**
	 * Adding groups
	 */
	rapidjson::Value groups;
	groups.SetArray();
	for(auto const& gid : gid_list)
	{
		groups.PushBack(gid, doc.GetAllocator());
	}
	data.AddMember("groups", groups, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string deleted_user(user_id id) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);

	auto const* config = get_config(::Message::user_commands::delete_user);
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();
	data.AddMember("id", id, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string added_new_group(Group const& group) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);

	auto const* config = get_config(::Message::user_commands::add_group);
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	group_info(data, group, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string deleted_group(group_id id) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);

	auto const* config = get_config(::Message::user_commands::delete_group);
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();
	data.AddMember("id", id, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string make_push_subscription(bool is_subscribed) noexcept
{
	rapidjson::Document doc;

	doc.SetObject();
	::Message::add_type(doc, ::Message::type::user);

	auto const* config = get_config(is_subscribed ?
			::Message::user_commands::push_subscribe :
			::Message::user_commands::push_unsubscribe);
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	return ::Message::stringify(doc);
}

}//Message
}//User
}//Agro
