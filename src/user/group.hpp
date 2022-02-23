#ifndef AGRO_DAEMON_USER_GROUP_HPP__
#define AGRO_DAEMON_USER_GROUP_HPP__

#include "info.hpp"
#include <vector>
#include <string>

namespace Agro{
namespace User{

using group_id = int;

class Group{
	public:
		Group(group_id, std::string const& name, std::string description = "");

		group_id id() const noexcept;

		std::string const& name() const noexcept;
		std::string const& description() const noexcept;

		bool add(user_id) noexcept;
		bool remove(user_id) noexcept;

		bool contains(user_id) const noexcept;
		bool operator==(Group const&) const noexcept;

		std::size_t size() const noexcept;

		std::vector<int>::const_iterator begin() const { return users_.begin(); }
		std::vector<int>::const_iterator end() const { return users_.end(); }
		std::vector<int>::const_iterator cbegin() const { return users_.cbegin(); }
		std::vector<int>::const_iterator cend() const { return users_.cend(); }
	private:
		group_id id_;
		std::string name_;
		std::string description_;
		std::vector<int>	users_;
};

class Groups{
	public:
		bool add(Group&&) noexcept;
		bool remove(group_id) noexcept;

		bool add_user(group_id, user_id) noexcept;
		void add_user(group_id gid, std::vector<user_id> const& uid_list) noexcept;
		void add_user_to_groups(user_id uid, std::vector<group_id> const& gid_list) noexcept;

		void remove_user_from_all(user_id) noexcept;

		bool contains(group_id) const noexcept;
		bool contains(std::string const&) const noexcept;

		bool contains_user(group_id, user_id) const noexcept;

		Group const* get(group_id) const noexcept;
		Group* get(group_id id) noexcept;

		std::size_t size() const noexcept;

		std::vector<Group>::const_iterator begin() const { return groups_.begin(); }
		std::vector<Group>::const_iterator end() const { return groups_.end(); }
		std::vector<Group>::const_iterator cbegin() const { return groups_.cbegin(); }
		std::vector<Group>::const_iterator cend() const { return groups_.cend(); }
	private:
		std::vector<Group> groups_;
};

}//User
}//Agro

#endif /* AGRO_DAEMON_USER_GROUP_HPP__ */
