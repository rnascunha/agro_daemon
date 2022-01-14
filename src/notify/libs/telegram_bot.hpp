#ifndef AGRO_DAEMON_HELPER_TELEGRAM_BOT_HPP__
#define AGRO_DAEMON_HELPER_TELEGRAM_BOT_HPP__

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include <memory>

using boost::asio::ip::tcp;

class telegram_bot : public std::enable_shared_from_this<telegram_bot>
{
	enum { max_length = 1024 };
	public:
		static constexpr const char* host = "api.telegram.org";

		telegram_bot(boost::asio::io_context& io_context,
					boost::asio::ssl::context&,
					std::string const& token,
					std::string const& chatid,
					std::string const& message);

		~telegram_bot();

		void connect();
	private:
		void do_connect(const boost::asio::ip::tcp::resolver::results_type& results);

		void handshake();
		void send_request();
		void receive_response();

		boost::asio::ssl::stream<tcp::socket> socket_;
		boost::asio::ip::tcp::resolver resolver_;
		std::string payload_;
		char reply_[max_length];
};

class telegram_factory{
	public:
		telegram_factory(boost::asio::io_context& ioc,
				std::string const& token);

		bool is_valid() const noexcept;
		void notify(std::string const& chatid, std::string const& message) noexcept;

		std::string const& token() const noexcept;
		void token(std::string const&) noexcept;
	private:
		boost::asio::io_context& ioc_;
		boost::asio::ssl::context ctx_{boost::asio::ssl::context::sslv23};
		std::string token_;
};

#endif /* AGRO_DAEMON_HELPER_TELEGRAM_BOT_HPP__ */
