#include "agro.hpp"
#include "tt/tt.hpp"

#include <filesystem>
#include <memory>

#include "websocket/load_certificate.hpp"
#include "websocket/types.hpp"
#include "websocket/listener.hpp"

#include "resources/init.hpp"

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

	auto sh = std::make_shared<Agro::share>(*this);

	Resource::init(coap_engine_, device_list_, sh, vresource_);

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
	return users_.infos().get(username);
}

bool instance::get_user_password(std::string const& username,
						std::vector<unsigned char>& salt,
						std::vector<unsigned char>& pass) noexcept
{
	return db_.get_password(username, salt, pass);
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

Device_List const& instance::device_list() const noexcept
{
	return device_list_;
}

Device_List& instance::device_list() noexcept
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
