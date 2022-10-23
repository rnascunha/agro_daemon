#ifndef AGRO_DAEMON_WESOCKET_BINARY_DATA_HPP__
#define AGRO_DAEMON_WESOCKET_BINARY_DATA_HPP__

#include <fstream>

#include "../websocket.hpp"

#include "../../message/report.hpp"

#include "../../image/image.hpp"
#include "../../app/app.hpp"

static constexpr const std::size_t image_name_max_size = 31;
static constexpr const std::size_t app_name_max_size = 12;

template<bool UseSSL>
void Websocket<UseSSL>::clear_read_buffer() noexcept
{
	char buffer[1024];
	while(!stream_.is_message_done())
	{
		stream_.read_some(boost::asio::buffer(buffer, 1024));
	}
}

template<bool UseSSL>
void Websocket<UseSSL>::get_app_file(
		const char* first_block, std::size_t block_size) noexcept
{
	std::size_t name_size = (std::uint8_t)first_block[0];
	if(name_size > app_name_max_size)
	{
		tt::warning("Upload app with invalid name size [%zu]", name_size);
				std::stringstream ss;
				ss << "size=" << name_size << "/max=" << (image_name_max_size - 1);
				this->write(share_->instance().make_report(
						Agro::Message::report_commands::app,
						Agro::Message::report_type::warning,
						"app upload", "Invalid name size", ss.str(), user_.id()
					)
				);
		/**
		 * Clear buffer
		 */
		clear_read_buffer();
		return;
	}

	std::string name(&first_block[1], name_size);
	std::string path(share_->instance().app_path().make_path(name).string());

	if(std::filesystem::exists(path))
	{
		this->write(share_->instance().make_report(
						Agro::Message::report_commands::app,
						Agro::Message::report_type::warning,
						"app upload", "App exist", name, user_.id()
			)
		);

		/**
		 * Clear buffer
		 */
		clear_read_buffer();
		return;
	}

	std::ofstream t{path, std::ios::binary};
	t.write(&first_block[1 + name_size], block_size - 1 - name_size);

	char buffer[1024];
	while(!stream_.is_message_done())
	{
		std::size_t s = stream_.read_some(boost::asio::buffer(buffer, 1024));
		t.write(buffer, s);
	}

	t.close();

	if(!share_->instance().add_app(name, user_.id()))
	{
		this->write(share_->instance().make_report(
					Agro::Message::report_commands::app,
					Agro::Message::report_type::error,
					"app upload", "Error inserting app to database", name, user_.id()
		));
		std::filesystem::remove(path);
		return;
	}

	tt::status("App '%s' uploaded successfully!", name.c_str());

	std::stringstream ss;
	ss << "New app uploaded [" << name << "]";
	share_->instance().notify_all_policy(Agro::Authorization::rule::view_app, ss.str());

	share_->instance().send_all_app_list();
}

template<bool UseSSL>
void Websocket<UseSSL>::get_image_file(
		const char* first_block, std::size_t block_size) noexcept
{
	std::size_t name_size = (std::uint8_t)first_block[0];
	if(name_size >= image_name_max_size)
	{
		tt::warning("Upload image with invalid name size [%zu]", name_size);
		std::stringstream ss;
		ss << "size=" << name_size << "/max=" << (image_name_max_size - 1);
		this->write(share_->instance().make_report(
				Agro::Message::report_commands::image,
				Agro::Message::report_type::warning,
				"image upload", "Invalid name size", ss.str(), user_.id()
			)
		);

		/**
		 * Clear buffer
		 */
		clear_read_buffer();
		return;
	}

	std::string name(&first_block[1], name_size);
	std::string path(share_->instance().image_path().make_path(name).string());

	if(std::filesystem::exists(path))
	{
		this->write(share_->instance().make_report(
						Agro::Message::report_commands::image,
						Agro::Message::report_type::warning,
						"image upload", "Image exist", name, user_.id()
			)
		);
		/**
		 * Clear buffer
		 */
		clear_read_buffer();
		return;
	}

	std::ofstream t{path, std::ios::binary};
	t.write(&first_block[1 + name_size], block_size - 1 - name_size);

	char buffer[1024];
	while(!stream_.is_message_done())
	{
		std::size_t s = stream_.read_some(boost::asio::buffer(buffer, 1024));
		t.write(buffer, s);
	}
	t.close();

	if(!Agro::check_image(path))
	{
		tt::error("Error uploading image '%s'! Hash not match!", name.c_str());
		this->write(share_->instance().make_report(
						Agro::Message::report_commands::image,
						Agro::Message::report_type::error,
						"image upload", "Image hash not match", name, user_.id()
			)
		);
		std::filesystem::remove(path);
		return;
	}

	if(!share_->instance().add_image(name, user_.id()))
	{
		this->write(share_->instance().make_report(
					Agro::Message::report_commands::image,
					Agro::Message::report_type::error,
					"image upload", "Error inserting image to database", name, user_.id()
		));
		std::filesystem::remove(path);
	}
}

template<bool UseSSL>
void Websocket<UseSSL>::get_file(const char* first_block, std::size_t block_size) noexcept
{
	switch(static_cast<binary_type>(first_block[0]))
	{
		case binary_type::json:
			tt::debug("JSON binary not defined");
			break;
		case binary_type::image:
			tt::debug("Binary data is a image.");
			get_image_file(first_block + 1, block_size - 1);
			break;
		case binary_type::app:
			tt::debug("Binary data is a app.");
			get_app_file(first_block + 1, block_size - 1);
			break;
		default:
			tt::warning("Image type not defined [%d]", static_cast<int>(first_block[0]));
			break;
	}
}

#endif /* AGRO_DAEMON_WESOCKET_BINARY_DATA_HPP__ */
