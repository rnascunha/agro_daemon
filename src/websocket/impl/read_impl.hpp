#ifndef AGRO_MESH_WEBSOCKET_READ_HANDLER_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_READ_HANDLER_IMPL_HPP__

#include "../websocket.hpp"

#include "../../device/message/device.hpp"
#include "../../sensor/message/sensor.hpp"
#include "../../user/message/auth_message.hpp"

#include "../../notify/message/make.hpp"

#include "../../message/report.hpp"

#include "tt/tt.hpp"

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

//template<typename Websocket>
//void process(std::string&& str,
//		Websocket,
//		Agro::instance&,
//		Agro::User::Logged&) noexcept;


}//Message
}//Agro

template<bool UseSSL>
void
Websocket<UseSSL>::
send_initial_data() noexcept
{
	tt::status("User %s (%s) authenticated",
						user_.user()->info().name().c_str(),
						user_.user()->info().username().c_str());

	/**
	 * Sending authenticate info
	 */
	write(Agro::User::Message::user_authentication(user_));

	/**
	 * Sending notification info
	 */
	write(Agro::Notify::Message::make_list(user_.user()->notify()));
	write(Agro::Notify::Message::make_device_list(user_.user()->notify()));
	write(Agro::Notify::Message::make_sensor_list(user_.user()->notify()));

	/**
	 * Sending notify public key
	 */
	if(share_->instance().push_notify_is_valid())
	{
		write(Agro::Notify::Message::make_public_key(share_->instance().get_notify_public_key()));
	}

	/**
	 * Sending sensor types configured list
	 */
	write(Agro::Sensor::Message::sensor_types_list(share_->instance().sensor_list()));

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
	write(std::make_shared<std::string>(Agro::Message::report_message(reports)));
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

template<bool UseSSL>
void
Websocket<UseSSL>::
read_handler(std::string data) noexcept
{
	/**
	 * Checks if user is already atheticated and athenticate (try) if not.
	 *
	 * \retval true If user already authenticated
	 * \retval false if user was not alreaty ahthenticated
	 */
	if(!check_authenticate(data))
	{
		tt::debug("User not authenticated!");

		/**
		 * Enters here onlty if user was not already athenticated. Check if
		 * Athetication was succeceded.
		 */
		if(user_.is_authenticated())
		{
			/**
			 * Send data to the user
			 */
			send_initial_data();
		}
	}
	//User was already authenticated
	else if(stream_.got_binary())
	{
		/**
		 * Data is of binary type. Can be a APP or a IMAGE
		 */
		tt::debug("Received binary: %zu", data.size());
		get_file(data.data(), data.length());
	}
	else
	{
		/**
		 * User is of type JSON to be processed.
		 */
		tt::debug("Received[%zu]: %.*s", data.size(), data.size(), data.data());
		Agro::Message::process(std::move(data),
				this->shared_from_this(),
				share_->instance(),
				user_);
	}
}

#endif /* AGRO_MESH_WEBSOCKET_READ_HANDLER_IMPL_HPP__ */
