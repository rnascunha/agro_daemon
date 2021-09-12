#include "agro.hpp"

namespace Agro{

instance::instance(DB& db, engine& coap_engine,
		Device_List& dev_list, notify_factory& notify,
		User::Users& users)
	: db_(db), coap_engine_(coap_engine), device_list_(dev_list),
	  notify_(notify), users_(users){}

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
