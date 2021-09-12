#ifndef AGRO_DAEMON_MESSAGE_USER_HPP__
#define AGRO_DAEMON_MESSAGE_USER_HPP__

#include "../user/user.hpp"
#include "types.hpp"
#include "rapidjson/document.h"
#include "../websocket/types.hpp"
#include "../agro.hpp"

namespace Message{

void process_user(rapidjson::Document const&,
		Agro::websocket_ptr,
		Agro::instance&,
		Agro::User::Logged&) noexcept;

}//Message

#endif /* AGRO_DAEMON_MESSAGE_USER_HPP__ */
