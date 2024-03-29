#include "agro.hpp"
#include "tt/tt.hpp"

#include <filesystem>
#include <memory>

#include "../websocket/load_certificate.hpp"
#include "../websocket/types.hpp"
#include "../websocket/listener.hpp"

#include "../device/resources/init.hpp"

#include "../user/password.hpp"

namespace Agro{

instance::instance(boost::asio::io_context& ioc,
			DB&& db,
			Notify::Factory&& notify,
			udp_conn::endpoint& ep,
			boost::asio::ip::tcp::endpoint const& epl,
#if USE_SSL == 1
			std::string const& ssl_key,
			std::string const& ssl_cert,
#endif /**/
			std::error_code& ec,
			configure config /* = configure{} */)
	:	configure_{config},
		image_{configure_.images_path}, app_{configure_.apps_path},
		ioc_{ioc},
		db_{std::move(db)},
		coap_engine_{udp_conn{}, CoAP::Message::message_id{CoAP::random_generator()}},
		notify_{std::move(notify)}
{
	if(!db_.read_user_all_db(users_))
	{
		ec = make_error_code(Error::internal_error);
		tt::error("Error reading user data from database!");
		return;
	}

	if(!db_.read_devices_net(net_list_))
	{
		ec = make_error_code(Error::internal_error);
		tt::error("Error reading device nets from database!");
		return;
	}

	if(!db_.read_devices(device_list_, net_list_))
	{
		ec = make_error_code(Error::internal_error);
		tt::error("Error reading devices from database!");
		return;
	}
	tree_.read_device_list();

	share_ = std::make_shared<Agro::share>(*this);

	Device::Resource::init(coap_engine_, *this, vresource_);

	boost::system::error_code ecb;
#if USE_SSL == 0
	make_listener<websocket>(ioc, epl, share_, ecb);
#else /* USE_SSL == 0 */

	load_certificate(ctx_, ssl_cert, ssl_key, ecb);
	if(ec)
	{
		tt::error("Error loading key/certificate! [%d/%s]", ec.value(), ec.message().c_str());
		return;
	}

	make_listener<websocket>(ioc, ctx_, epl, share_, ecb);
#endif /* USE_SSL == 0 */
	if(ec)
	{
		tt::error("Error opening listener! [%d/%s]", ec.value(), ec.message().c_str());
		return;
	}

	CoAP::Error ecp;
	coap_engine_.get_connection().open(ecp);
	if(ecp)
	{
		ec = std::make_error_code(std::errc::operation_canceled);
		tt::error("Error opening device socket! [%d/%s]", ecp.value(), ecp.message());
		return;
	}

	coap_engine_.get_connection().bind(ep, ecp);
	if(ecp)
	{
		ec = std::make_error_code(std::errc::address_in_use);
		tt::error("Error binding device socket! [%d/%s]", ecp.value(), ecp.message());
		return;
	}

	initiate_check_roots();

	if(!db_.read_sensor_types(sensor_list_))
	{
		ec = make_error_code(Error::statement_error);
		tt::error("Error reading sensor types values!");
		return;
	}
	db_.read_all_sensor_values(device_list_);

	for(auto const& device : device_list_.list())
	{
		unconnected_.push_back(device.second.mac());
	}
}

Device::Device_List const& instance::device_list() const noexcept
{
	return device_list_;
}

Device::Device_List& instance::device_list() noexcept
{
	return device_list_;
}

engine const& instance::coap_engine() const noexcept
{
	return coap_engine_;
}

engine& instance::coap_engine() noexcept
{
	return coap_engine_;
}

User::User_List const& instance::users() const noexcept
{
	return users_;
}

Device::Tree& instance::tree() noexcept
{
	return tree_;
}

Sensor::Sensor_Type_List const& instance::sensor_list() const noexcept
{
	return sensor_list_;
}

share_ptr instance::share() noexcept
{
	return share_;
}

Notify::Factory const& instance::notify() const noexcept
{
	return notify_;
}

}//Agro
