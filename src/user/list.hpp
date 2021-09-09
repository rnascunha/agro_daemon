#ifndef AGRO_DAEMON_USER_LIST_HPP__
#define AGRO_DAEMON_USER_LIST_HPP__

#include "user.hpp"
#include <map>

namespace Agro{

class User_List{
	public:
		bool add(User&&) noexcept;
		bool remove(int) noexcept;

		bool exists(int) const noexcept;

		User& get(int) noexcept;
		User const& get(int) const noexcept;
	private:
		std::map<int, User>	list_;
};

}//Agro

#endif /* AGRO_DAEMON_USER_LIST_HPP__ */
