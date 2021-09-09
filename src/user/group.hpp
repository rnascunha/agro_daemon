#ifndef AGRO_DAEMON_USER_GROUP_HPP__
#define AGRO_DAEMON_USER_GROUP_HPP__

#include <unordered_set>

namespace Agro{

class User_Group{
	public:
		bool add(int) noexcept;
		bool remove(int) noexcept;

		bool existis(int) const noexcept;
	private:
		std::unordered_set<int>	user_ids_;
};

}//Agro

#endif /* AGRO_DAEMON_USER_GROUP_HPP__ */
