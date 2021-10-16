#include "agro.hpp"
#include "tt/tt.hpp"

#include <filesystem>
#include <memory>

#include "../websocket/load_certificate.hpp"
#include "../websocket/types.hpp"
#include "../websocket/listener.hpp"

#include "../resources/init.hpp"

#include "../user/password.hpp"

static pusha::key get_notify_key(std::filesystem::path const& path, Agro::DB& db) noexcept;

namespace Agro{

instance::instance(
		boost::asio::io_context& ioc,
		std::string const& db_file,
		std::string const& notify_priv_key,
		std::string_view const& subscriber,
		udp_conn::endpoint& ep,
		boost::asio::ip::tcp::endpoint const& epl,
#if USE_SSL == 1
		std::string const& ssl_key,
		std::string const& ssl_cert,
#endif /**/
		std::error_code& ec)
	: ioc_{ioc},
	  db_{db_file.c_str(), ec},
	  coap_engine_{udp_conn{}, CoAP::Message::message_id{CoAP::random_generator()}},
	  notify_{ioc, !ec ? get_notify_key(notify_priv_key, db_) : pusha::key{}, subscriber}
{
	if(ec)
	{
		tt::error("Error opening DB! [%s]", db_file.c_str());
		return;
	}

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

	Resource::init(coap_engine_, *this, share_, vresource_);

	boost::system::error_code ecb;
#if USE_SSL == 0
	make_listener<Agro::websocket>(ioc, epl, share_, ecb);
#else /* USE_SSL == 0 */

	load_certificate(ctx_, ssl_cert, ssl_key, ecb);
	if(ec)
	{
		tt::error("Error loading key/certificate! [%d/%s]", ec.value(), ec.message());
		return;
	}

	make_listener<websocket>(ioc, ctx_, epl, share_, ecb);
#endif /* USE_SSL == 0 */
	if(ec)
	{
		tt::error("Error opening listener! [%d/%s]", ec.value(), ec.message());
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
}

bool instance::remove_node_from_tree(mesh_addr_t const& addr) noexcept
{
	bool change = tree_.remove_node(addr);
	if(change)
	{
//		std::cout << "Tree changed\n";
		share_->write_all_policy(Authorization::rule::view_device,
				std::make_shared<std::string>(Device::Message::device_tree_to_json(tree_)));
	}
	return change;
}

bool instance::update_tree(Device::Device const& device) noexcept
{
	bool change = tree_.update(device);
	if(change)
	{
//		std::cout << "Tree changed\n";
		share_->write_all_policy(Authorization::rule::view_device,
				std::make_shared<std::string>(Device::Message::device_tree_to_json(tree_)));
	}
	return change;
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

share_ptr instance::share() noexcept
{
	return share_;
}

}//Agro

static pusha::key get_notify_key(std::filesystem::path const& path, Agro::DB& db) noexcept
{
	pusha::key key;

	if(!path.empty())
	{
		if(!std::filesystem::is_regular_file(path))
		{
			tt::error("notify private key: '%s' is not a valid file.", path.c_str());
			goto import_db;
		}
		else if(!key.import(path))
		{
			tt::error("notify private key: Failed to import from '%s' file.", path.c_str());
			goto import_db;
		}
		else {
			tt::debug("notify private key: imported from '%s' file.", path.c_str());
			goto end;
		}
	}
	else
	{
import_db:
		std::string keyb64 = db.notify_private_key();
		if(!keyb64.empty())
		{
			if(!key.import(keyb64))
			{
				tt::error("notifiy private key: import from 'DB' fail.");
			}
			else
			{
				tt::debug("notify private key: key imported from DB.");
			}
		}
	}

end:
	return key;
}

