#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"
#include "boost/beast.hpp"
#include <iostream>
#include <utility>
#include "../message/device.hpp"
#include "../message/process.hpp"
#include "../message/ota.hpp"
#include "../message/info.hpp"
#include "../ota/ota.hpp"

#include <fstream>

static constexpr const std::size_t image_name_max_size = 30;

template<typename Stream>
void get_image_file(Stream& stream,
		const char* first_block, std::size_t block_size,
		std::filesystem::path const& images_path) noexcept
{
	std::size_t name_size = (std::uint8_t)first_block[0];
	if(name_size >= image_name_max_size)
	{
		std::stringstream ss;
		ss << "size=" << name_size << "/max=" << (image_name_max_size - 1);
		Websocket<false>::write_all(Message::make_info(Message::info::warning,
				Message::info_category::image, "Invalid image name size", ss.str().c_str()));
		/**
		 * Clear buffer
		 */
		char buffer[1024];
		do
		{
			stream.read_some(boost::asio::buffer(buffer, 1024));
		}
		while(!stream.is_message_done());
		return;
	}

	std::string name(&first_block[1], name_size);
	std::string path = images_path;
			path += "/";
			path += name;

	if(std::filesystem::exists(path))
	{
		Websocket<false>::write_all(Message::make_info(Message::info::warning,
				Message::info_category::image, "Image already uploaded", name.c_str()));
		/**
		 * Clear buffer
		 */
		char buffer[1024];
		do
		{
			stream.read_some(boost::asio::buffer(buffer, 1024));
		}
		while(!stream.is_message_done());
		return;
	}

	std::ofstream t{path, std::ios::binary};
	t.write(&first_block[1 + name_size], block_size - 1 - name_size);

	char buffer[1024];
	do
	{
		std::size_t s = stream.read_some(boost::asio::buffer(buffer, 1024));
		t.write(buffer, s);
	}
	while(!stream.is_message_done());
	t.close();

	if(!check_image(path))
	{
		Websocket<false>::write_all(Message::make_info(Message::info::error,
				Message::info_category::image, "Uploaded Image hash not match",
				name.c_str()));
		std::filesystem::remove(path);
	}

	Websocket<false>::write_all(Message::ota_image_list(ota_path()));
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

	this->write(Message::device_list_to_json(*dev_list_));
	this->write(Message::ota_image_list(ota_path()));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
read_handler(std::string data) noexcept
{
	if(base_type::stream_.got_binary())
	{
		/**
		 * Is a image
		 */
		std::cout << "Received binary: " << data.size() << "\n";
		get_image_file(base_type::stream_,
				data.data(), data.length(),
				ota_path());
	}
	else
	{
		std::cout << "Received[" << data.size() << "]: " << data << "\n";

		Message::process(*coap_engine_, std::move(data), *dev_list_);
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

	std::cerr << what << "[" << ec.value() << "]: " << ec.message() << "\n";
}

#endif /* AGRO_MESH_WEBSOCKET_IMPL_HPP__ */
