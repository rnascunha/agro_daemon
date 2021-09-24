#include "../make.hpp"
#include "rapidjson/document.h"
#include "../../user.hpp"
#include "../../group.hpp"

namespace Agro{
namespace User{
namespace Message{

template<typename Allocator>
void user_info(rapidjson::Value& user, User const& iuser, Allocator& alloc) noexcept
{
	user.SetObject();
	user.AddMember("id", iuser.id(), alloc);
	user.AddMember("username",
			rapidjson::Value(iuser.info().username().data(), iuser.info().username().size(), alloc).Move(),
			alloc);
	user.AddMember("name",
				rapidjson::Value(iuser.info().name().data(), iuser.info().name().size(), alloc).Move(),
				alloc);
	user.AddMember("email",
					rapidjson::Value(iuser.info().email().data(), iuser.info().email().size(), alloc).Move(),
					alloc);
	user.AddMember("status", static_cast<int>(iuser.info().get_status()), alloc);
}

template<typename Allocator>
void group_info(rapidjson::Value& group, Group const& info, Allocator& alloc) noexcept
{
	group.SetObject();

	group.AddMember("id", info.id(), alloc);
	group.AddMember("name",
			rapidjson::Value(info.name().data(), info.name().size(), alloc).Move(),
			alloc);
	group.AddMember("description",
				rapidjson::Value(info.description().data(), info.description().size(), alloc).Move(),
				alloc);
	group.AddMember("size", info.size(), alloc);

	rapidjson::Value members;
	members.SetArray();

	for(auto const& user : info)
	{
		members.PushBack(user, alloc);
	}

	group.AddMember("members", members, alloc);
}

template<typename Allocator>
void user_list(rapidjson::Value& ulist, User_List const& users, Allocator& alloc) noexcept
{
	ulist.SetArray();
	for(auto const& [uid, u] : users)
	{
		rapidjson::Value user;
		user_info(user, u, alloc);
		ulist.PushBack(user, alloc);
	}
}

template<typename Allocator>
void group_list(rapidjson::Value& glist, Groups const& groups, Allocator& alloc) noexcept
{
	glist.SetArray();
	for(auto const& u : groups)
	{
		rapidjson::Value group;
		group_info(group, u, alloc);
		glist.PushBack(group, alloc);
	}
}

}//Message
}//User
}//Agro
