#include "list.hpp"

namespace Agro{
namespace User{

Info_List& Users::infos() noexcept
{
	return user_list_;
}

Info_List const& Users::infos() const noexcept
{
	return user_list_;
}

Subscription_List& Users::subscriptions() noexcept
{
	return sub_list_;
}

Subscription_List const& Users::subscriptions() const noexcept
{
	return sub_list_;
}

Session_List& Users::sessions() noexcept
{
	return session_list_;
}

Session_List const& Users::sessions() const noexcept
{
	return session_list_;
}

Groups& Users::groups() noexcept
{
	return groups_;
}

Groups const& Users::groups() const noexcept
{
	return groups_;
}

}//User
}//Agro
