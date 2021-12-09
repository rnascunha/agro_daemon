#ifndef AGRO_DAEMON_NOTIFY_FACTORY_HPP__
#define AGRO_DAEMON_NOTIFY_FACTORY_HPP__

#include "../user/user.hpp"

#include "libs/push_notify.hpp"
#include "libs/telegram_bot.hpp"
#include "libs/mail.hpp"

#include <memory>
#include <unordered_map>

namespace Agro{
namespace Notify{

class notify_impl{
	public:
		virtual void notify(User::User const&, std::string const& message) noexcept = 0;
		virtual ~notify_impl(){}
};

class push : public notify_impl{
	public:
		push(boost::asio::io_context&,
				pusha::key&&,
				std::string_view const& subscriber);
		void notify(User::User const&, std::string const& message) noexcept override;

		std::string_view const& public_key() const noexcept;
		bool is_valid() const noexcept;
	private:
		push_factory push_;
};

class telegram : public notify_impl{
	public:
		telegram(boost::asio::io_context&, std::string const& token);
		void notify(User::User const&, std::string const& message) noexcept override;
	private:
		telegram_factory tele_;
};

class mail : public notify_impl{
	public:
		mail(boost::asio::io_context&,
			SMTP::server const&,
			std::string const&);
		void notify(User::User const&, std::string const& message) noexcept override;
	private:
		mail_factory mail_;
};

class Factory{
	public:
		Factory();
		Factory(Factory&&) = default;

		bool push_is_valid() const noexcept;
		std::string_view const& public_key() const noexcept;

		bool add(std::string const& name, std::unique_ptr<notify_impl>) noexcept;

		void notify(User::User const&, std::string const& message) noexcept;
	private:
		std::unordered_map<std::string, std::unique_ptr<notify_impl>> notifys_;
};

}//Notify
}//Agro

#endif /* AGRO_DAEMON_NOTIFY_FACTORY_HPP__ */
