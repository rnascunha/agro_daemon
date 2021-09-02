#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"

#include "boost/beast.hpp"

#include <iostream>
#include <utility>
#include <string_view>

//#include "../../message/process.hpp"
#include "../../message/user.hpp"
#include "binary_data_impl.hpp"

#include "tt/tt.hpp"

namespace Message{

void process(std::string&& str,
		std::shared_ptr<Websocket<false>>,
		Agro::instance&,
		Agro::User&) noexcept;
}

template<bool UseSSL>
Websocket<UseSSL>::~Websocket()
{
	shared_type::leave(this);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_all(std::string const data) noexcept
{
	shared_type::exec_to_all(
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
	shared_type::exec_to_all(
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
	this->join(this);

//	this->write(Message::device_list_to_json(*dev_list_));
//	this->write(Message::ota_image_list(ota_path()));
//	this->write(Message::app_list(app_path()));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
read_handler(std::string data) noexcept
{
	if(!check_authenticate(data)) return;

	if(base_type::stream_.got_binary())
	{
		/**
		 * Is a image
		 */
		tt::debug("Received binary: %zu", data.size());
		get_file(base_type::stream_,
				data.data(), data.length());
	}
	else
	{
		tt::debug("Received[%zu]: %.*s", data.size(), data.size(), data.data());
		Message::process(std::move(data),
				this->shared_from_this(),
				*instance_,
				user_);
	}
}

template<bool UseSSL>
void
Websocket<UseSSL>::
fail(boost::system::error_code ec, char const* what) noexcept
{
	if(ec == boost::asio::error::operation_aborted ||
	   ec == boost::beast::websocket::error::closed)
	{
		return;
	}

	tt::error("%s[%d]: %.*s", what, ec.value(), ec.message().size(), ec.message().data());
}

#endif /* AGRO_MESH_WEBSOCKET_IMPL_HPP__ */
