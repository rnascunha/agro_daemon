#include "agro.hpp"
#include "tt/tt.hpp"

#include <filesystem>
#include <memory>

#include "websocket/load_certificate.hpp"
#include "websocket/types.hpp"
#include "websocket/listener.hpp"

#include "resources/init.hpp"

#include "user/password.hpp"

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

	if(!db_.read_policies(policy_rules_))
	{
		ec = make_error_code(Error::internal_error);
		tt::error("Error reading policy rules from database!");
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

//	if(!db_.read_permission_list(permissions_))
//	{
//		ec = make_error_code(Error::internal_error);
//		tt::error("Error reading permissions from database!");
//		return;
//	}

	auto sh = std::make_shared<Agro::share>(*this);

	Resource::init(coap_engine_, *this, sh, vresource_);

	boost::system::error_code ecb;
#if USE_SSL == 0
	make_listener<Agro::websocket>(ioc, epl, sh, ecb);
#else /* USE_SSL == 0 */

	load_certificate(ctx_, ssl_cert, ssl_key, ecb);
	if(ec)
	{
		tt::error("Error loading key/certificate! [%d/%s]", ec.value(), ec.message());
		return;
	}

	make_listener<websocket>(ioc, ctx_, epl, sh, ecb);
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
}


bool instance::check_user_session_id(
				User::user_id id,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) const noexcept
{
	return users_.sessions()
			.check_user_session_id(
					id,
					session_id,
					user_agent,
					session_time);
}

User::Info const* instance::get_user_info(std::string const& username) const noexcept
{
	if(username == User::root_username) return &root_;
	return users_.infos().get(username);
}

User::Info const* instance::get_user_info(User::user_id id) const noexcept
{
	if(id == User::root_id) return &root_;
	return users_.infos().get(id);
}

bool instance::get_user_password(std::string const& username,
						std::vector<unsigned char>& salt,
						std::vector<unsigned char>& pass) noexcept
{
	return db_.get_password(username, salt, pass);
}

bool instance::add_user(std::string const& username,
						std::string const& name,
						std::string const& password,
						std::string const& email,
						User::user_id& id) noexcept
{
	id = User::invalid_id;

	User::key_password key;
	User::salt_password salt;
	if(!User::create_password(password, salt, key))
	{
		tt::error("Error hashing password of '%s'!", username.c_str());
		return false;
	}

	if(db_.add_user(username, key, salt, name, email, id) != SQLITE_DONE)
	{
		tt::error("Error adding user '%s' to database!", username.c_str());
		return false;
	}

	if(!users_.add_user(User::Info{id, username, name, User::Info::status::active, email}))
	{
		tt::error("Error adding user '%s'!", username.c_str());
		return false;
	}

	return true;
}

bool instance::add_user_to_group(User::user_id uid, User::group_id gid) noexcept
{
	if(db_.add_user_to_group(uid, gid) != SQLITE_DONE)
	{
		tt::error("Error adding user '%d' to group '%d' at database", uid, gid);
		return false;
	}

	if(!users_.groups().add_user(gid, uid))
	{
		tt::error("Error adding user '%d' to group '%d'", uid, gid);
		return false;
	}

	return true;
}

void instance::add_user_to_group(User::user_id uid, std::vector<User::group_id> const& gid_list) noexcept
{
	db_.add_user_to_group(uid, gid_list);
	users_.add_user_to_groups(uid, gid_list);
}

bool instance::edit_user(User::user_id id,
				std::string const& username,
				std::string const& name,
				std::string const& email,
				std::vector<User::group_id> const& groups) noexcept
{
	if(db_.edit_user(id, username, name, email, groups) != SQLITE_DONE)
	{
		tt::error("Error editing user '%d' at database", id);
		return false;
	}

	if(!users_.edit_user(id, username, name, email, groups))
	{
		tt::error("Error editing user '%d'", id);
		return false;
	}

	return true;
}

bool instance::delete_user(User::user_id id) noexcept
{
	if(db_.delete_user(id) != SQLITE_DONE)
	{
		tt::error("Error deleteing user '%d' from database", id);
		return false;
	}

	if(!users_.infos().remove(id))
	{
		tt::error("Error deleteing user '%d'", id);
		return false;
	}

	if(db_.delete_user_from_group(id) != SQLITE_DONE)
	{
		tt::error("Error deleteing user '%d' from database groups", id);
		return false;
	}

	users_.groups().remove_user_from_all(id);

//	if(db_.delete_permissions(Authorization::type::user,
//			Authorization::ref_type::single, id) != SQLITE_DONE)
//	{
//		tt::error("Error deleteing user '%d' permissions form database", id);
//		return false;
//	}

//	permissions_.remove_all(Authorization::type::user,
//			Authorization::ref_type::single,
//			id);

	return true;
}

User::Group const* instance::get_group_info(User::group_id id) const noexcept
{
	return users_.groups().get(id);
}

bool instance::add_group(std::string const& name,
				std::string const& description,
				std::vector<User::user_id> const& members,
				User::group_id& id) noexcept
{
	int rc = db_.add_user_group(name, description, id);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error adding group '%d' to database. [err=%d]", name.c_str(), rc);
		return false;
	}

	if(!users_.groups().add(User::Group{id, name, description}))
	{
		tt::error("Error adding group '%s'", name.c_str());
		return false;
	}

	db_.add_user_to_group(id, members);
	users_.groups().add_user(id, members);

	return true;
}

bool instance::delete_group(User::group_id id) noexcept
{
	int rc = db_.delete_group(id);
	if(db_.delete_group(id) != SQLITE_DONE)
	{
		tt::error("Error deleteing group '%d' from database. [err=%d]", id, rc);
		return false;
	}

	if(!users_.groups().remove(id))
	{
		tt::error("Error deleteing group '%d'", id);
		return false;
	}

//	if(db_.delete_permissions(Authorization::type::user_group,
//			Authorization::ref_type::group, id) != SQLITE_DONE)
//	{
//		tt::error("Error deleteing group '%d' permissions form database", id);
//		return false;
//	}

//	permissions_.remove_all(Authorization::type::user_group,
//			Authorization::ref_type::group,
//			id);

	return true;
}

void instance::policy_rules(User::Logged& user) const noexcept
{
	user.policy_rules(static_cast<int>(policy_rules_.get_policy(user.id(), users_.groups())));
}

bool instance::notify_is_valid() const noexcept
{
	return notify_.is_valid();
}

std::string_view const& instance::get_notify_public_key() const noexcept
{
	return notify_.public_key();
}

bool instance::update_user_session_id(User::user_id id,
				std::string const& session_id,
				std::string const& user_agent) noexcept
{
	users_.sessions().add_or_update(id, user_agent, session_id);
	return db_.update_user_session_id(id, session_id, user_agent);
}

void instance::push_subscribe_user(User::user_id id,
			std::string const& user_agent,
			std::string const& endpoint,
			std::string const& p256dh,
			std::string const& auth) noexcept
{
	users_.subscriptions().add_or_update(id, user_agent, endpoint, p256dh, auth);
	db_.push_subscribe_user(id, user_agent, endpoint, p256dh, auth);
}

void instance::push_unsubscribe_user(User::user_id id,
			std::string const& user_agent) noexcept
{
	users_.subscriptions().remove(id, user_agent);
	db_.push_unsubscribe_user(id, user_agent);
}

void instance::clear_session_user_agent(User::user_id id,
			std::string const& user_agent) noexcept
{
	users_.subscriptions().clear_subscription(id, user_agent);
	db_.clear_session_user_agent(id, user_agent);
}

void instance::notify_all(std::string const& data) noexcept
{
	for(auto const& s : users_.subscriptions())
	{
		notify_.notify(s, data);
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

User::Users const& instance::users() const noexcept
{
	return users_;
}

//Authorization::Permission_List const& instance::permissions() const noexcept
//{
//	return permissions_;
//}

bool instance::process_device_request(engine::message const& request,
		Device::Device** dev,
		CoAP::Message::Option::option& op) noexcept
{
	*dev = nullptr;
	if(!CoAP::Message::Option::get_option(request,
			op,
			CoAP::Message::Option::code::uri_host))
	{
		tt::debug("Ignoring device packet! Missing uri-host option");
		return false;
	}

	std::error_code ec;
	mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};
	if(ec)
	{
		tt::debug("Ignoring device packet! Error parsing address [%d/%s]", ec.value(), ec.message().c_str());
		return false;
	}

	*dev = device_list_[host];
	if(!(*dev))
	{
		//Device does's exists
		//notify users IF they have view_device authorization
		Device::device_id id;
		db_.add_device(host, id);
		*dev = device_list_.add(Device::Device{id, host});
	}

	return true;
}

bool instance::update_db_device(Device::Device const& device) noexcept
{
	int rc = db_.update_device(device);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error updating device database [%d]", rc);
	}
	return rc == SQLITE_DONE;
}

Device::Net* instance::get_or_add_net(mesh_addr_t const& net_addr) noexcept
{
	Device::Net* net = net_list_.get(net_addr);
	if(!net)
	{
		int id;
		db_.add_net(net_addr, id);
		net = net_list_.add(Device::Net{id, net_addr});
	}

	return net;
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
