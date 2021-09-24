#ifndef AGRO_DAEMON_WEBSOCKET_AUTH_HPP__
#define AGRO_DAEMON_WEBSOCKET_AUTH_HPP__

#include "../../user/authenticate.hpp"
#include "../../user/authenticate_params.h"
#include "../../message/user_types.hpp"
#include "tt/tt.hpp"

template<bool UseSSL>
void Websocket<UseSSL>::error_auth(std::error_code const& ec, const char* what /* = nullptr */) noexcept
{
	tt::error("%s[%d]: %.*s", what ? what : "", ec.value(),
							ec.message().size(), ec.message().data());
	timer_.expires_after(boost::asio::chrono::seconds(1));
	timer_.async_wait([s = this->shared_from_this(), ec](const boost::system::error_code& /*e*/){
		s->close(boost::beast::websocket::close_reason{static_cast<uint16_t>(4000 + ec.value())});
	});
}


template<bool UseSSL>
bool
Websocket<UseSSL>::
check_authenticate(std::string const& data) noexcept
{
	if(user_.is_authenticated()) return true;

	std::error_code ec;
	rapidjson::Document doc;
	if(base_type::stream_.got_binary()
		|| doc.Parse(data.data(), data.size()).HasParseError()
		|| !doc.IsObject())
	{
		ec = make_error_code(Error::invalid_value);
		error_auth(ec);
		return false;
	}

	Message::user_commands comm;
	if(!Agro::authenticate<
			USER_AUTH_INTERATION_NUMBER,
			USER_AUTH_KEY_LENGTH,
			USER_AUTH_SALT_LENGTH,
			USER_SESSION_TIME_SECONDS>(user_, doc, instance(), comm, ec))
	{
		error_auth(ec, "auth fail");
		return false;
	}

	if(comm == Message::user_commands::autheticate
		&& user_.user()->id() != Agro::User::root_id)
	{
		Agro::create_session_id<USER_SESSION_ID_SIZE>(user_, doc, instance(), ec);
	}

	return false;
}


#endif /* AGRO_DAEMON_WEBSOCKET_AUTH_HPP__ */
