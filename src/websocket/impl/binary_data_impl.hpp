#ifndef AGRO_DAEMON_WESOCKET_BINARY_DATA_HPP__
#define AGRO_DAEMON_WESOCKET_BINARY_DATA_HPP__

#include "../websocket.hpp"
//#include "../../notify/notify.hpp"
//#include "../../message/device.hpp"
//#include "../../message/ota.hpp"
//#include "../../message/app.hpp"
#include "../../message/info.hpp"
#include "../../ota/ota.hpp"
#include "../../app/app.hpp"
#include <fstream>

namespace Message{

std::string ota_image_list(std::filesystem::path const&) noexcept;
std::string app_list(std::filesystem::path const& path) noexcept;

}//Message

static constexpr const std::size_t image_name_max_size = 30;
static constexpr const std::size_t app_name_max_size = 12;

enum class binary_type{
	json = 0,
	image = 1,
	app = 2
};

//static void notify_new_update(const char* type, std::string const& name)
//{
//	std::stringstream ss;
//	ss << "New " << type << " added [" << name << "]";
//	notify(ss.str());
//}

template<bool UseSSL>
void Websocket<UseSSL>::get_app_file(
		const char* first_block, std::size_t block_size,
		std::filesystem::path const& app_path) noexcept
{
	std::size_t name_size = (std::uint8_t)first_block[0];
	if(name_size > app_name_max_size)
	{
		std::stringstream ss;
		ss << "size=" << name_size << "/max=" << app_name_max_size;
		write_all(Message::make_info(Message::info::warning,
				Message::info_category::app, "Invalid app name size", ss.str().c_str()));
		/**
		 * Clear buffer
		 */
		char buffer[1024];
		do
		{
			base_type::stream_.read_some(boost::asio::buffer(buffer, 1024));
		}
		while(!base_type::stream_.is_message_done());
		return;
	}

	std::string name(&first_block[1], name_size);
	std::string path = app_path;
			path += "/";
			path += name;

	if(std::filesystem::exists(path))
	{
		write_all(Message::make_info(Message::info::warning,
				Message::info_category::app, "App already uploaded", name.c_str()));
		/**
		 * Clear buffer
		 */
		char buffer[1024];
		do
		{
			base_type::stream_.read_some(boost::asio::buffer(buffer, 1024));
		}
		while(!base_type::stream_.is_message_done());
		return;
	}

	std::ofstream t{path, std::ios::binary};
	t.write(&first_block[1 + name_size], block_size - 1 - name_size);

	char buffer[1024];
	do
	{
		std::size_t s = base_type::stream_.read_some(boost::asio::buffer(buffer, 1024));
		t.write(buffer, s);
	}
	while(!base_type::stream_.is_message_done());
	t.close();

//	notify_new_update("app", name);
	write_all(Message::app_list(app_path));
}

template<bool UseSSL>
void Websocket<UseSSL>::get_image_file(
		const char* first_block, std::size_t block_size,
		std::filesystem::path const& images_path) noexcept
{
	std::size_t name_size = (std::uint8_t)first_block[0];
	if(name_size >= image_name_max_size)
	{
		std::stringstream ss;
		ss << "size=" << name_size << "/max=" << (image_name_max_size - 1);
		write_all(Message::make_info(Message::info::warning,
				Message::info_category::image, "Invalid image name size", ss.str().c_str()));
		/**
		 * Clear buffer
		 */
		char buffer[1024];
		do
		{
			base_type::stream_.read_some(boost::asio::buffer(buffer, 1024));
		}
		while(!base_type::stream_.is_message_done());
		return;
	}

	std::string name(&first_block[1], name_size);
	std::string path = images_path;
			path += "/";
			path += name;

	if(std::filesystem::exists(path))
	{
		write_all(Message::make_info(Message::info::warning,
				Message::info_category::image, "Image already uploaded", name.c_str()));
		/**
		 * Clear buffer
		 */
		char buffer[1024];
		do
		{
			base_type::stream_.read_some(boost::asio::buffer(buffer, 1024));
		}
		while(!base_type::stream_.is_message_done());
		return;
	}

	std::ofstream t{path, std::ios::binary};
	t.write(&first_block[1 + name_size], block_size - 1 - name_size);

	char buffer[1024];
	do
	{
		std::size_t s = base_type::stream_.read_some(boost::asio::buffer(buffer, 1024));
		t.write(buffer, s);
	}
	while(!base_type::stream_.is_message_done());
	t.close();

	if(!check_image(path))
	{
		write_all(Message::make_info(Message::info::error,
				Message::info_category::image, "Uploaded Image hash not match",
				name.c_str()));
		std::filesystem::remove(path);
	}
	else
	{
//		notify_new_update("image", name);
	}

	write_all(Message::ota_image_list(images_path));
}

template<bool UseSSL>
void Websocket<UseSSL>::get_file(const char* first_block, std::size_t block_size) noexcept
{
	std::cout << "File received::[" << static_cast<int>(first_block[0]) << "]\n";
	switch(static_cast<binary_type>(first_block[0]))
	{
		case binary_type::json:
			std::cerr << "JSON image not defined\n";
			break;
		case binary_type::image:
			std::cout << "File is a image\n";
			get_image_file(first_block + 1, block_size - 1, ota_path());
			break;
		case binary_type::app:
			std::cout << "File is a app\n";
			get_app_file(first_block + 1, block_size - 1, app_path());
			break;
		default:
			std::cerr << "Image type not defined [" << static_cast<int>(first_block[0]) << "]\n";
			break;
	}
}

#endif /* AGRO_DAEMON_WESOCKET_BINARY_DATA_HPP__ */
