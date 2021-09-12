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
	instance(boost::asio::io_context& ioc,
			std::string const& db_file,
			std::string const& notify_priv_key,
			std::string_view const& subscriber,
			udp_conn::endpoint& ep,
			boost::asio::ip::tcp::endpoint const& epl,
	#if USE_SSL == 1
			std::string const& ssl_key,
			std::string const& ssl_cert,
	#endif /**/
			std::error_code& ec);

		template<unsigned TimeoutMs>
		void run(int num_threads, CoAP::Error& ecp) noexcept;

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
		boost::asio::io_context& ioc_;
		DB					db_;
		engine				coap_engine_;
		notify_factory 		notify_;
		Device_List			device_list_;
		User::Users			users_;

		std::vector<engine::resource_node> vresource_;
#if USE_SSL == 1
		boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12};
#endif /* USE_SSL == 1 */
};

}//Agro

#include "impl/agro_impl.hpp"

#endif /* AGRO_DAEMON_AGRO_HPP__ */
