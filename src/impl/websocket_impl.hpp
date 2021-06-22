#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"
#include "boost/beast.hpp"
#include <iostream>

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
			std::make_shared<Byte_Array const>(std::move(data))
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
			std::make_shared<Byte_Array const>(std::move(data)),
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
//	rapidjson::Document d;
//
//	std::string json(data.to_string());
//	if (d.Parse(json.c_str()).HasParseError() || !d.IsObject())
//	{
//		fail_message(Message::App::main, make_error_code(Message_Errc::ILL_FORMED));
//		//fail_message(this->shared_from_this(), Message::App::main, make_error_code(Message_Errc::ILL_FORMED));
//		return;
//	}
//
//	//Echoing to the other users (and... same user)
//	Propagator_Websocket::write_all(Byte_Array(std::move(Message::Factory::add_mid<true>(d))));
//
//	Message::Main_Data main;
//	if(!Apps::Main::parser_main(d, main)){
//		fail_message(Message::App::main, make_error_code(Message_Errc::ILL_FORMED));
//		return;
//	}
//	Util::print_json(d);
//
//	if(!Core::Dispatcher::exec(main.app, this->shared_from_this(), main, d))
//		fail_message(Message::App::main, make_error_code(Message_Errc::APP_NOT_FOUND));
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
