#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"

#include "boost/beast.hpp"

#include <utility>
#include <string_view>

#include "../../message/device.hpp"
#include "../../notify/message.hpp"
#include "tt/tt.hpp"

namespace Message{

#if USE_SSL == 0
void process(std::string&& str,
		std::shared_ptr<Websocket<false>>,
		Agro::instance&,
		Agro::User::Logged&) noexcept;
#else /* USE_SSL == 0 */
void process(std::string&& str,
		std::shared_ptr<Websocket<true>>,
		Agro::instance&,
		Agro::User::Logged&) noexcept;
#endif /* USE_SSL == 0 */

}

template<bool UseSSL>
Websocket<UseSSL>::~Websocket()
{
	share_->leave(this);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_all(std::string const data) noexcept
{
	share_->exec_to_all(
		std::bind(
			&self_type::write_share,
			std::placeholders::_1,
			std::make_shared<std::string const>(std::move(data))
		)
	);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_all(std::string const data, bool text) noexcept
{
	share_->exec_to_all(
		std::bind(
			&self_type::write_share,
			std::placeholders::_1,
			std::make_shared<std::string const>(std::move(data))
		)
	);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_open() noexcept
{
	this->text(true);
//	this->join(this);
	share_->join(this);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
read_handler(std::string data) noexcept
{
	if(!check_authenticate(data))
	{
		if(user_.is_authenticated())
		{
			if(instance().notify_is_valid())
			{
				this->write(Message::make_public_key(instance().get_notify_public_key()));
			}
			this->write(Message::device_list_to_json(instance().device_list()));
//			this->write(Message::ota_image_list(ota_path()));
//			this->write(Message::app_list(app_path()));
		}
		return;
	}

	if(base_type::stream_.got_binary())
	{
		/**
		 * Is a image
		 */
		tt::debug("Received binary: %zu", data.size());
		get_file(data.data(), data.length());
	}
	else
	{
		tt::debug("Received[%zu]: %.*s", data.size(), data.size(), data.data());
		Message::process(std::move(data),
				this->shared_from_this(),
				instance(),
				user_);
	}
}

template<bool UseSSL>
void
Websocket<UseSSL>::
fail(boost::system::error_code ec, char const* what) noexcept
{
//	if(ec == boost::asio::error::operation_aborted ||
//	   ec == boost::beast::websocket::error::closed)
//	{
//		return;
//	}

	tt::error("%s[%d]: %.*s", what, ec.value(), ec.message().size(), ec.message().data());
}

template<bool UseSSL>
std::shared_ptr<share<Websocket<UseSSL>>>
Websocket<UseSSL>::get_share() noexcept
{
	return share_;
}

#endif /* AGRO_MESH_WEBSOCKET_IMPL_HPP__ */
