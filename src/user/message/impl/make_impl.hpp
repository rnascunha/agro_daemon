#include "../make.hpp"
#include "rapidjson/document.h"
#include "../../user.hpp"
#include "../../group.hpp"

namespace Agro{
namespace User{
namespace Message{

template<typename Allocator>
void user_info(rapidjson::Value& user, Info const& info, Allocator& alloc) noexcept
{
	user.SetObject();
	user.AddMember("id", info.id(), alloc);
	user.AddMember("username",
			rapidjson::Value(info.username().data(), info.username().size(), alloc).Move(),
			alloc);
	user.AddMember("name",
				rapidjson::Value(info.name().data(), info.name().size(), alloc).Move(),
				alloc);
	user.AddMember("email",
					rapidjson::Value(info.email().data(), info.email().size(), alloc).Move(),
					alloc);
	user.AddMember("status", static_cast<int>(info.get_status()), alloc);
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

//template<typename Allocator>
//void permission_info(rapidjson::Value& perm,
//		Authorization::Permission const& info, Allocator& alloc) noexcept
//{
//	perm.SetObject();
//
//	perm.AddMember("id", info.id(), alloc);
//	perm.AddMember("type", static_cast<int>(info.get_type()), alloc);
//	perm.AddMember("action", static_cast<int>(info.get_action()), alloc);
//	perm.AddMember("effect", static_cast<int>(info.get_effect()), alloc);
//	perm.AddMember("ref_type", static_cast<int>(info.reference_type()), alloc);
//	perm.AddMember("ref_id", static_cast<int>(info.reference_id()), alloc);
//}

template<typename Allocator>
void user_list(rapidjson::Value& ulist, Info_List const& infos, Allocator& alloc) noexcept
{
	ulist.SetArray();
	for(auto const& u : infos)
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

//template<typename Allocator>
//void permission_list(rapidjson::Value& plist,
//		Authorization::Permission_List const& permissions, Allocator alloc) noexcept
//{
//	plist.SetArray();
//	for(auto const& p : permissions)
//	{
//		rapidjson::Value perm;
//		permission_info(perm, *p, alloc);
//		plist.PushBack(perm, alloc);
//	}
//}

}//Message
}//User
}//Agro
