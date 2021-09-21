#ifndef AGRO_DAEMON_USER_HPP__
#define AGRO_DAEMON_USER_HPP__

#include <vector>
#include <string>

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
		Info(user_id id, std::string const& username, std::string const& name,
				status stat, std::string const& email);
		Info(user_id id, const char* username, const char* name,
				status stat, const char* email);

		bool is_valid() const noexcept;

		void set(int id, std::string const& username, std::string const& name,
				status stat, std::string const& email) noexcept;

		user_id id() const noexcept;
		std::string const& username() const noexcept;
		std::string const& name() const noexcept;
		status get_status() const noexcept;
		std::string const& email() const noexcept;

		operator bool() const noexcept;
	private:
		user_id		id_ = invalid_id;
		std::string username_;
		std::string name_;
		status		status_ = status::invalid;
		std::string	email_;
};

class Info_List{
	public:
		bool add(Info&&) noexcept;
		bool remove(user_id) noexcept;

		user_id get_id(std::string const& username) const noexcept;
		Info const* get(std::string const& username) const noexcept;
		Info const* get(user_id) const noexcept;

		bool update(user_id id,
				std::string const& username,
				std::string const& name,
				std::string const& email) noexcept;

		std::size_t size() const noexcept;

		std::vector<Info>::const_iterator begin() const { return list_.begin(); }
		std::vector<Info>::const_iterator end() const { return list_.end(); }
		std::vector<Info>::const_iterator cbegin() const { return list_.cbegin(); }
		std::vector<Info>::const_iterator cend() const { return list_.cend(); }
	private:
		std::vector<Info> list_;
};

class Subscription{
	public:
		Subscription(user_id, std::string const user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const auth);

		user_id id() const noexcept;
		std::string const& user_agent() const noexcept;

		std::string const& endpoint() const noexcept;
		std::string const& p256dh() const noexcept;
		std::string const& auth() const noexcept;

		void update(std::string const& endpoint,
				std::string const& p256dh,
				std::string const auth) noexcept;
	private:
		user_id		id_;
		std::string user_agent_;

		std::string	endpoint_;
		std::string	p256dh_;
		std::string auth_;
};

class Subscription_List{
	public:
		void add_or_update(user_id, std::string const& user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;
		bool remove(user_id, std::string const& user_agent) noexcept;

		void clear_subscription(user_id, std::string const& user_agent) noexcept;

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
		Session(user_id,
				std::string const& user_agent,
				std::string const& session_id,
				long session_time);

		Session(user_id,
				std::string const& user_agent,
				std::string const& session_id);

		user_id id() const noexcept;
		std::string const& user_agent() const noexcept;

		std::string const& session_id() const noexcept;
		long session_time() const noexcept;
		long login_time() const noexcept;
		long logout_time() const noexcept;

		bool check(User::user_id id,
				std::string const& session_id,
				std::string const& user_agent) const noexcept;

		bool check_time(long session_time) const noexcept;

		bool is_logged() const noexcept;

		void login(std::string const& session_id) noexcept;
		void logout() noexcept;

		void update(std::string const& session_id) noexcept;
	private:
		user_id 	id_;
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
		void add_or_update(user_id,
				std::string const& session_id,
				std::string const& user_agent) noexcept;
		bool remove(std::string const& session_id) noexcept;

		bool check_user_session_id(
						User::user_id id,
						std::string const& session_id,
						std::string const& user_agent,
						long& session_time) const noexcept;

		std::size_t size() const noexcept;
	private:
		std::vector<Session> list_;
};

class Logged{
	public:
		void info(Info const*) noexcept;
		Info const* info() const noexcept;

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
		std::string user_agent_;
		std::string session_id_;
		int			policy_rules_ = 0;

		bool authenticated_ = false;
		Info const*	 info_ = nullptr;
};

}//User
}//Agro

#endif /* AGRO_DAEMON_USER_HPP__ */
