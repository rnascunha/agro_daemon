#ifndef AGRO_DAEMON_USER_HPP__
#define AGRO_DAEMON_USER_HPP__

#include <string>

namespace Agro{

class User{
	public:
		enum class status{
			invalid = -1,
			active = 0,
			inactive,
			suspended,
			deleted
		};

		User();
		User(int id, std::string const& username, std::string const& name,
				status stat, std::string const& email);
		User(int id, const char* username, const char* name,
				status stat, const char* email);

		bool is_valid() const noexcept;
		bool is_authenticated() const noexcept;

		void set(int id, std::string const& username, std::string const& name,
				status stat, std::string const& email) noexcept;

		int id() const noexcept;
		std::string const& username() const noexcept;
		std::string const& name() const noexcept;
		status get_status() const noexcept;
		std::string const& email() const noexcept;
		std::string const& session_id() const noexcept;
		std::string const& user_agent() const noexcept;

		operator bool() const noexcept;

		void authenticate() noexcept;
		void authenticate(std::string const& session_id,
				std::string const& user_agent) noexcept;

		bool is_subscribed() const noexcept;
		void push_subscribe(std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;

		std::string const& endpoint() const noexcept;
		std::string const& p256dh() const noexcept;
		std::string const& auth() const noexcept;

		void push_unsubscribe() noexcept;
	private:
		int 		id_ = -1;
		std::string username_;
		std::string name_;
		status		status_ = status::invalid;
		std::string	email_;
		std::string session_id_;
		std::string user_agent_;
		bool		authenticated_ = false;

		bool		is_subscribed_ = false;
		std::string	endpoint_;
		std::string	p256dh_;
		std::string auth_;
};

}//Agro

#endif /* AGRO_DAEMON_USER_HPP__ */
