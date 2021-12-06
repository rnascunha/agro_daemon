#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"

#include "boost/beast.hpp"

#include <utility>
#include <string_view>
#include <fstream>

#include "tt/tt.hpp"

#include "../../device/message/device.hpp"
#include "../../sensor/message/sensor.hpp"
#include "../../user/message/auth_message.hpp"

#include "../../notify/message/make.hpp"

#include "../../message/report.hpp"

namespace Agro{
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

}//Message
}//Agro

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
write_all_policy(Agro::Authorization::rule rule,
		std::shared_ptr<std::string const> data) noexcept
{
	share_->exec_policy(rule,
			std::bind(
				&self_type::write_share,
				std::placeholders::_1,
				data));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_policy(Agro::Authorization::rule rule,
				std::shared_ptr<std::string const> data) noexcept
{
	if(Agro::Authorization::can(user_, rule))
	{
		this->write(data);
	}
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_file(binary_type type,
				std::string const& name,
				std::filesystem::path const& path) noexcept
{
	tt::status("Sending file [%d]%s of path %s",
			static_cast<int>(type),
			name.c_str(), path.c_str());

	std::ifstream in(path, std::ios::binary);
	std::stringstream ss;

	std::uint8_t t = static_cast<std::uint8_t>(type);
	ss.write(reinterpret_cast<char const*>(&t), sizeof(t));
	std::uint16_t size = name.size();
	ss.write(reinterpret_cast<char const*>(&size), sizeof(size));
	ss << name;
	ss << in.rdbuf();

	this->write(std::make_shared<std::string>(ss.str()), false);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_write(boost::system::error_code ec, std::size_t bytes_transfered) noexcept
{
	base_type::on_write(ec, bytes_transfered);
	this->text(true);
}

template<bool UseSSL>
Agro::User::Logged const&
Websocket<UseSSL>::
user() const noexcept
{
	return user_;
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_open() noexcept
{
	this->text(true);
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
			tt::status("User %s (%s) authenticated",
					user_.user()->info().name().c_str(),
					user_.user()->info().username().c_str());

			/**
			 * Sending authenticate info
			 */
			this->write(Agro::User::Message::user_authentication(user_));

			/**
			 * Sending notification info
			 */
			this->write(Agro::Notify::Message::make_list(user_.user()->notify()));
			this->write(Agro::Notify::Message::make_device_list(user_.user()->notify()));
			this->write(Agro::Notify::Message::make_sensor_list(user_.user()->notify()));

			/**
			 * Sending notify public key
			 */
			if(share_->instance().push_notify_is_valid())
			{
				this->write(Agro::Notify::Message::make_public_key(share_->instance().get_notify_public_key()));
			}

			/**
			 * Sending sensor types configured list
			 */
			this->write(Agro::Sensor::Message::sensor_types_list(share_->instance().sensor_list()));

			/**
			 * Sending device data
			 */
			write_policy(Agro::Authorization::rule::view_device,
				std::make_shared<std::string>(
						Agro::Device::Message::device_list_to_json(share_->instance().device_list())));

			write_policy(Agro::Authorization::rule::view_device,
							std::make_shared<std::string>(
									Agro::Device::Message::device_list_sensor_data(
											share_->instance().device_list(),
											share_->instance().sensor_list())));

			/**
			 * Getting reports
			 */
			std::vector<Agro::Message::report> reports;
			share_->instance().read_all_reports(reports, user_.id(), 20);
			//Sending
			this->write(std::make_shared<std::string>(Agro::Message::report_message(reports)));
			/**
			 * Sending device tree
			 */
			write_policy(Agro::Authorization::rule::view_device,
							std::make_shared<std::string>(
									Agro::Device::Message::device_tree_to_json(share_->instance().tree())));

			/**
			 * Sending images
			 */
			share_->instance().send_all_image_list();

			/**
			 * Sending apps
			 */
			share_->instance().send_all_app_list();
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
		Agro::Message::process(std::move(data),
				this->shared_from_this(),
				share_->instance(),
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
