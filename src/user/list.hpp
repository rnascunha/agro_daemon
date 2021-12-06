#ifndef AGRO_DAEMON_USER_LIST_HPP__
#define AGRO_DAEMON_USER_LIST_HPP__

#include "user.hpp"
#include "group.hpp"
#include "policy.hpp"

namespace Agro{
namespace User{

class User_List{
	public:
		User* add(User&&) noexcept;
		bool remove(user_id) noexcept;

		User const* get(user_id) const noexcept;
		User* get(user_id) noexcept;

		User const* get(std::string const&) const noexcept;
		User* get(std::string const&) noexcept;

		User const* operator[](user_id) const noexcept;
		User* operator[](user_id) noexcept;

		User const* operator[](std::string const&) const noexcept;
		User* operator[](std::string const&) noexcept;

//		Groups& groups() noexcept;
		Groups const& groups() const noexcept;

		bool add_group(Group&&) noexcept;
		bool remove_group(group_id) noexcept;
		void remove_user_from_all_groups(user_id) noexcept;

		Authorization::Policies const& policies() const noexcept;
		Authorization::Policies& policies() noexcept;

		bool add_user_to_group(user_id, group_id gid) noexcept;
		void add_user_to_groups(user_id, std::vector<group_id> const&) noexcept;
		void set_user_to_groups(user_id uid, std::vector<group_id> const& gid_list) noexcept;
		bool edit_user(user_id id,
				std::string const& username,
				std::string const& name,
				std::string const& email,
				std::string const& telegram_chat_id,
				std::vector<group_id> const&) noexcept;

		std::unordered_map<user_id, User>::iterator begin() { return list_.begin(); }
		std::unordered_map<user_id, User>::iterator end() { return list_.end(); }

		std::unordered_map<user_id, User>::const_iterator begin() const { return list_.begin(); }
		std::unordered_map<user_id, User>::const_iterator end() const { return list_.end(); }
		std::unordered_map<user_id, User>::const_iterator cbegin() const { return list_.cbegin(); }
		std::unordered_map<user_id, User>::const_iterator cend() const { return list_.cend(); }
	private:
		std::unordered_map<user_id, User>	list_;
		Groups								groups_;
		Authorization::Policies				policy_rules_;

		void update_user_policy(user_id) noexcept;
		void update_user_policy(User& user) noexcept;
		void update_user_policy() noexcept;
};


}//User
}//Agro

#endif /* AGRO_DAEMON_USER_LIST_HPP__ */
