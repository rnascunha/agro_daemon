#ifndef AGRO_DAEMON_USER_INFO_HPP__
#define AGRO_DAEMON_USER_INFO_HPP__

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
		Info(std::string const& username, std::string const& name,
				status stat, std::string const& email,
				std::string const& telegram_chat_id);
		Info(const char* username, const char* name,
				status stat, const char* email,
				const char* telegram_chat_id);

		bool is_valid() const noexcept;

		void set(std::string const& username, std::string const& name,
				status stat, std::string const& email,
				std::string const& telegram_chat_id) noexcept;

		std::string const& username() const noexcept;
		std::string const& name() const noexcept;
		status get_status() const noexcept;
		std::string const& email() const noexcept;
		std::string const& telegram_chat_id() const noexcept;

		operator bool() const noexcept;
	private:
		std::string username_;
		std::string name_;
		status		status_ = status::invalid;
		std::string	email_;
		std::string telegram_chat_id_;
};

}//User
}//Agro

#endif /* AGRO_DAEMON_USER_INFO_HPP__ */
