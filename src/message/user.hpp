#ifndef AGRO_DAEMON_MESSAGE_USER_HPP__
#define AGRO_DAEMON_MESSAGE_USER_HPP__

#include "../user/user.hpp"
#include "types.hpp"
#include "rapidjson/document.h"
#include "../websocket/websocket.hpp"
#include "../agro.hpp"

namespace Message{

void process_user(rapidjson::Document const&,
		std::shared_ptr<Websocket<false>>,
		Agro::instance&,
		Agro::User&) noexcept;

}//Message

#endif /* AGRO_DAEMON_MESSAGE_USER_HPP__ */
