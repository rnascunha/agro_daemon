#ifndef AGRO_DAEMON_NOTIFY_FACTORY_HPP__
#define AGRO_DAEMON_NOTIFY_FACTORY_HPP__

#include "../user/user.hpp"

#include "libs/push_notify.hpp"
#include "libs/telegram_bot.hpp"

namespace Agro{
namespace Notify{

class Factory{
	public:
		Factory(boost::asio::io_context&,
				//push
				pusha::key&& ec_key,
				std::string_view const& subscriber,
				//telegram
				std::string const& token);

		bool push_is_valid() const noexcept;
		std::string_view const&  public_key() const noexcept;

		void notify(User::User const&, std::string const& message) noexcept;
	private:
		push_factory	push_fac_;
		telegram_factory tele_fac_;
};

}//Notify
}//Agro

#endif /* AGRO_DAEMON_NOTIFY_FACTORY_HPP__ */
