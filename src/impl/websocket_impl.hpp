#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"
#include "boost/beast.hpp"
#include <iostream>
#include <utility>
#include "../message/device.hpp"
#include "../message/process.hpp"

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
	std::cout << "Opened\n";

	this->text(true);
	this->join(this);

	std::string devices{Message::device_list_to_json(*dev_list_)};
	std::cout << "Open: " << devices << "\n";
	this->write(devices);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
read_handler(std::string data) noexcept
{
	std::cout << "Received[" << data.size() << "]: " << data << "\n";

	Message::process(*coap_engine_, std::move(data), *dev_list_);
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

	std::cerr << what << "[" << ec.value() << "]: " << ec.message() << "\n";
}

#endif /* AGRO_MESH_WEBSOCKET_IMPL_HPP__ */
