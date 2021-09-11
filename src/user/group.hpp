#ifndef AGRO_DAEMON_USER_GROUP_HPP__
#define AGRO_DAEMON_USER_GROUP_HPP__

#include <vector>
#include <unordered_set>

namespace Agro{

class User_Group{
	public:
		bool add(int) noexcept;
		bool remove(int) noexcept;

		bool contains(int id) const noexcept
		{
			return user_ids_.find(id) != user_ids_.end();
		}

		int id() const noexcept;
	private:
		int id_;
		std::unordered_set<int>	user_ids_;
};

class User_Group_List{
	public:
		bool add(User_Group&&) noexcept;
		bool remove(int) noexcept;

		std::vector<User_Group>::const_iterator begin() const { return groups_.begin(); }
		std::vector<User_Group>::const_iterator end() const { return groups_.end(); }
		std::vector<User_Group>::const_iterator cbegin() const { return groups_.cbegin(); }
		std::vector<User_Group>::const_iterator cend() const { return groups_.cend(); }
	private:
		std::vector<User_Group> groups_;
};

}//Agro

#endif /* AGRO_DAEMON_USER_GROUP_HPP__ */
