#ifndef AGRO_DAEMON_AGRO_HPP__
#define AGRO_DAEMON_AGRO_HPP__

#include "db/db.hpp"
#include "coap_engine.hpp"
#include "device/list.hpp"
#include "notify/notify_request.hpp"
#include "user/user.hpp"

namespace Agro{

class instance{
	public:
		instance(DB&, engine&, Device_List&, notify_factory&, User::Users&);

		bool check_user_session_id(
						User::user_id id,
						std::string const& session_id,
						std::string const& user_agent,
						long& session_time) const noexcept;

		User::Info const* get_user_info(std::string const& username) const noexcept;
		bool get_user_password(std::string const& username,
						std::vector<unsigned char>& salt,
						std::vector<unsigned char>& pass) noexcept;

		bool notify_is_valid() const noexcept;
		std::string_view const& get_notify_public_key() const noexcept;

		bool update_user_session_id(User::user_id,
						std::string const& session_id,
						std::string const& user_agent) noexcept;

		void push_subscribe_user(User::user_id,
				std::string const& user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;

		void push_unsubscribe_user(User::user_id,
				std::string const& user_agent) noexcept;

		void clear_session_user_agent(User::user_id,
				std::string const& user_agent) noexcept;

		Device_List const& device_list() const noexcept;
		Device_List& device_list() noexcept;

		engine const& coap_engine() const noexcept;
		engine& coap_engine() noexcept;
	private:
		DB&					db_;
		engine&				coap_engine_;
		Device_List&		device_list_;
		notify_factory& 	notify_;
		User::Users&		users_;
};

}//Agro

#endif /* AGRO_DAEMON_AGRO_HPP__ */
