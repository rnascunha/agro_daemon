#ifndef AGRO_DAEMON_USER_HPP__
#define AGRO_DAEMON_USER_HPP__

#include <vector>
#include <string>

namespace Agro{
namespace User{

using user_id = int;
static constexpr const user_id invalid_id = -1;

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

		std::size_t size() const noexcept;
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

class Users{
	public:
		Info_List& infos() noexcept;
		Info_List const& infos() const noexcept;

		Subscription_List& subscriptions() noexcept;
		Subscription_List const& subscriptions() const noexcept;

		Session_List& sessions() noexcept;
		Session_List const& sessions() const noexcept;
	private:
		Info_List				user_list_;
		Subscription_List	sub_list_;
		Session_List		session_list_;
};

class Logged{
	public:
		void info(Info const*) noexcept;
		Info const* info() const noexcept;

		std::string const& user_agent() const noexcept;
		std::string const& session_id() const noexcept;

		bool is_authenticated() const noexcept;
		void authenticate() noexcept;
		void authenticate(std::string const& session_id,
				std::string const& user_agent) noexcept;
	private:
		std::string user_agent_;
		std::string session_id_;

		bool authenticated_ = false;
//		Session* session_ = nullptr;
		Info const*	 info_ = nullptr;
};

}//User
}//Agro

#endif /* AGRO_DAEMON_USER_HPP__ */