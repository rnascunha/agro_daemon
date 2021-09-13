#ifndef AGRO_DAEMON_USER_LIST_HPP__
#define AGRO_DAEMON_USER_LIST_HPP__

#include "user.hpp"
#include "group.hpp"

namespace Agro{
namespace User{

class Users{
	public:
		Info_List& infos() noexcept;
		Info_List const& infos() const noexcept;

		Subscription_List& subscriptions() noexcept;
		Subscription_List const& subscriptions() const noexcept;

		Session_List& sessions() noexcept;
		Session_List const& sessions() const noexcept;

		Groups& groups() noexcept;
		Groups const& groups() const noexcept;
	private:
		Info_List			user_list_;
		Subscription_List	sub_list_;
		Session_List		session_list_;
		Groups				groups_;
};


}//User
}//Agro

#endif /* AGRO_DAEMON_USER_LIST_HPP__ */
