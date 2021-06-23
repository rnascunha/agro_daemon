#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"
#include "boost/beast.hpp"
#include <iostream>
#include <utility>
#include "../process_request.hpp"

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
			&self_type::write_share2,
			std::placeholders::_1,
			std::make_shared<std::string const>(std::move(data)),
			text
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

//	this->write(Apps::Main::make_uid());
//
//	std::vector<std::string> app_str;
//	Core::Dispatcher::for_each([&app_str](Core::App const& a){
//		app_str.push_back(a.name());
//	});
//	this->write(Apps::Main::make_app_list(app_str));
//
//	Propagator_Websocket::write_all(Apps::Main::make_status(Propagator_Websocket::get_num_shares()));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
read_handler(std::string data) noexcept
{
	std::cout << "Received[" << data.size() << "]: " << data << "\n";

	process_request(*coap_engine_, std::move(data));
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

	std::cerr << what << ": " << ec.message() << "\n";
}

#endif /* AGRO_MESH_WEBSOCKET_IMPL_HPP__ */
