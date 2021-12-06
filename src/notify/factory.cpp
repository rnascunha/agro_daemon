#include "factory.hpp"

namespace Agro{
namespace Notify{

Factory::Factory(boost::asio::io_context& ioc,
				//push
				pusha::key&& ec_key,
				std::string_view const& subscriber,
				//telegram
				std::string const& token)
	: push_fac_{ioc, std::move(ec_key), subscriber},
	  tele_fac_{ioc, token}{}

bool Factory::push_is_valid() const noexcept
{
	return push_fac_.is_valid();
}

std::string_view const& Factory::public_key() const noexcept
{
	return push_fac_.public_key();
}

void Factory::notify(User::User const& user, std::string const& message) noexcept
{
	if(push_fac_.is_valid())
	{
		for(auto const& s : user.subscriptions() )
		{
			push_fac_.notify(s, message);
		}
	}

	if(tele_fac_.is_valid())
	{
		tele_fac_.notify(user.info().telegram_chat_id(), message);
	}
}

}//Notify
}//Agro
