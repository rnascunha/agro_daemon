#ifndef AGRO_DAEMON_USER_MESSAGE_HPP__
#define AGRO_DAEMON_USER_MESSAGE_HPP__

#include <string>
#include "../user.hpp"
#include "../../instance/agro.hpp"

namespace Agro{
namespace User{
namespace Message{

std::string user_group_permissions(Logged const&, instance const&) noexcept;
std::string added_new_user(User const&, std::vector<group_id> const&) noexcept;
std::string edited_user(User const&, std::vector<group_id> const&) noexcept;
std::string deleted_user(user_id) noexcept;
std::string added_new_group(Group const&) noexcept;
std::string deleted_group(group_id) noexcept;
std::string make_push_subscription(bool is_subscribed) noexcept;

}//Message
}//User
}//Agro

#endif /* AGRO_DAEMON_USER_MESSAGE_HPP__ */
