#ifndef AGRO_DAEMON_USER_MESSAGE_MAKE_HPP__
#define AGRO_DAEMON_USER_MESSAGE_MAKE_HPP__

#include "rapidjson/document.h"
#include "../user.hpp"
#include "../group.hpp"

namespace Agro{
namespace User{
namespace Message{

template<typename Allocator>
void user_info(rapidjson::Value& user, Info const& info, Allocator& alloc) noexcept;
template<typename Allocator>
void group_info(rapidjson::Value& group, Group const& info, Allocator& alloc) noexcept;
//template<typename Allocator>
//static void permission_info(rapidjson::Value& perm,
//		Authorization::Permission const& info, Allocator& alloc) noexcept;
template<typename Allocator>
void user_list(rapidjson::Value& ulist, Info_List const& infos, Allocator& alloc) noexcept;
template<typename Allocator>
void group_list(rapidjson::Value& glist, Groups const& groups, Allocator& alloc) noexcept;
//template<typename Allocator>
//void permission_list(rapidjson::Value& plist,
//		Authorization::Permission_List const& permissions, Allocator alloc) noexcept;

}//Message
}//User
}//Agro

#include "impl/make_impl.hpp"

#endif /* AGRO_DAEMON_USER_MESSAGE_MAKE_HPP__ */
