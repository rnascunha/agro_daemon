#ifndef AGRO_DAEMON_USER_HPP__
#define AGRO_DAEMON_USER_HPP__

#include <vector>
#include <string>
#include <unordered_map>

namespace Agro{
namespace User{

using user_id = int;
static constexpr const user_id invalid_id = -1;
/**
 * Root definitions
 */
static constexpr const user_id root_id = 0;
static constexpr const char* root_username = "root";
static constexpr const char* root_name = "root";

class Info{
	public:
		enum class status{
			invalid = -1,
			active = 0,
			inactive,
			suspended,
			deleted
		};

		Info();
		Info(std::string const& username, std::string const& name,
				status stat, std::string const& email);
		Info(const char* username, const char* name,
				status stat, const char* email);

		bool is_valid() const noexcept;

		void set(std::string const& username, std::string const& name,
				status stat, std::string const& email) noexcept;

		std::string const& username() const noexcept;
		std::string const& name() const noexcept;
		status get_status() const noexcept;
		std::string const& email() const noexcept;

		operator bool() const noexcept;
	private:
		std::string username_;
		std::string name_;
		status		status_ = status::invalid;
		std::string	email_;
};

class Subscription{
	public:
		Subscription(std::string const user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const auth);

		std::string const& user_agent() const noexcept;

		std::string const& endpoint() const noexcept;
		std::string const& p256dh() const noexcept;
		std::string const& auth() const noexcept;

		void update(std::string const& endpoint,
				std::string const& p256dh,
				std::string const auth) noexcept;
	private:
		std::string user_agent_;

		std::string	endpoint_;
		std::string	p256dh_;
		std::string auth_;
};

class Subscription_List{
	public:
		void add_or_update(std::string const& user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;
		bool remove(std::string const& user_agent) noexcept;

		void clear_subscription(std::string const& user_agent) noexcept;

		std::size_t size() const noexcept;

		std::vector<Subscription>::const_iterator begin() const { return list_.begin(); }
		std::vector<Subscription>::const_iterator end() const { return list_.end(); }
		std::vector<Subscription>::const_iterator cbegin() const { return list_.cbegin(); }
		std::vector<Subscription>::const_iterator cend() const { return list_.cend(); }
	private:
		std::vector<Subscription> list_;
};

class Session{
	public:
		Session(std::string const& user_agent,
				std::string const& session_id,
				long session_time);

		Session(std::string const& user_agent,
				std::string const& session_id);

		std::string const& user_agent() const noexcept;

		std::string const& session_id() const noexcept;
		long session_time() const noexcept;
		long login_time() const noexcept;
		long logout_time() const noexcept;

		bool check(std::string const& session_id,
				std::string const& user_agent) const noexcept;

		bool check_time(long session_time) const noexcept;

		bool is_logged() const noexcept;

		void login(std::string const& session_id) noexcept;
		void logout() noexcept;

		void update(std::string const& session_id) noexcept;
	private:
		std::string user_agent_;
		std::string session_id_;

		bool logged_ = false;

		long session_time_ = 0;
		long session_time_login_ = 0;
		long session_time_logout_ = 0;
};

class Session_List{
	public:
		void add_or_update(Session&& session) noexcept;
		void add_or_update(
				std::string const& session_id,
				std::string const& user_agent) noexcept;
		bool remove(std::string const& session_id) noexcept;

		bool check_user_session_id(
						std::string const& session_id,
						std::string const& user_agent,
						long& session_time) const noexcept;

		std::size_t size() const noexcept;
	private:
		std::vector<Session> list_;
};

class User{
	public:
		User(user_id, Info&&);

		user_id	id() const noexcept;
		int policy() const noexcept;
		void policy(int pol) noexcept;

		Info const& info() const noexcept;
		Info& info() noexcept;

		Subscription_List const& subscriptions() const noexcept;
		Subscription_List& subscriptions() noexcept;

		Session_List const& sessions() const noexcept;
		Session_List& sessions() noexcept;
	private:
		user_id id_;
		int 	policy_ = 0;

		Info		info_;
		Subscription_List subscriptions_;
		Session_List sessions_;
};

class Logged{
	public:
		void user(User const*) noexcept;
		User const* user() const noexcept;

		user_id id() const noexcept;

		std::string const& user_agent() const noexcept;
		void user_agent(std::string const&) noexcept;

		std::string const& session_id() const noexcept;

		void policy_rules(int) noexcept;
		int policy_rules() const noexcept;

		bool is_authenticated() const noexcept;
		void authenticate() noexcept;
		void authenticate(std::string const& session_id,
				std::string const& user_agent) noexcept;
	private:
		user_id id_ = invalid_id;

		std::string user_agent_;
		std::string session_id_;

		bool authenticated_ = false;
		User const*	 user_ = nullptr;
};

}//User
}//Agro

#endif /* AGRO_DAEMON_USER_HPP__ */
