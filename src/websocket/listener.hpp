#ifndef AGRO_DAEMON_WEBSOCKET_LISTENER_HPP__
#define AGRO_DAEMON_WEBSOCKET_LISTENER_HPP__

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#endif /* USE_SSL == 1 */

#include <type_traits>
#include <memory>
#include <utility>

#include "share.hpp"

namespace Agro{

template<typename Session>
class Listener : public std::enable_shared_from_this<Listener<Session>>
{
		boost::asio::io_context& ioc_;
		boost::asio::ip::tcp::acceptor acceptor_;

		using empty = struct{};
		//Check if the session is SSL. If not, exclude ctx_ attribute
		using ssl_context = typename std::conditional<Session::use_ssl, boost::asio::ssl::context&, empty>::type;
		ssl_context ctx_;

		std::shared_ptr<share<Session>> share_;
	public:
		static constexpr const bool use_ssl = Session::use_ssl;

		Listener(
			boost::asio::io_context& ioc,
			std::shared_ptr<share<Session>> const& share)
			: ioc_(ioc)
			, acceptor_(boost::asio::make_strand(ioc))
			, share_(share){}
//#if USE_SSL == 1
		Listener(
				boost::asio::io_context& ioc,
				ssl_context ctx,
				std::shared_ptr<share<Session>> const& share)
				: ioc_(ioc), acceptor_(boost::asio::make_strand(ioc))
				, ctx_(ctx)
				, share_(share){}
//#endif
		~Listener(){}

		void
		close(boost::system::error_code&) noexcept
		{
			boost::asio::post(
				acceptor_.get_executor(),
					std::bind(&Listener::closing,
							this->shared_from_this()));
		}

		void
		init(boost::asio::ip::tcp::endpoint endpoint,
				boost::system::error_code& ec) noexcept
		{
			// Open the acceptor
			acceptor_.open(endpoint.protocol(), ec);
			if(ec)
				return fail(ec, "open");

			// Allow address reuse
			acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
			if(ec) return fail(ec, "set_option");

			// Bind to the server address
			acceptor_.bind(endpoint, ec);
			if(ec) return fail(ec, "bind");

			// Start listening for connections
			acceptor_.listen(
				boost::asio::socket_base::max_listen_connections, ec);
			if(ec) return fail(ec, "listen");

			do_accept();
		}

	protected:
		void
		fail(boost::system::error_code ec, char const* what) noexcept
		{
			std::cerr << what << ": " << ec.message() << "\n";
		}

		void closing()
		{
			boost::system::error_code ec;
			acceptor_.cancel(ec);
			if(ec) return fail(ec, "cancel");
			acceptor_.close(ec);
			if(ec) return fail(ec, "close");
		}

		void
		do_accept() noexcept
		{
			// The new connection gets its own strand
			acceptor_.async_accept(
				boost::asio::make_strand(ioc_),
				boost::beast::bind_front_handler(
					&Listener<Session>::on_accept,
					this->shared_from_this()));
		}

		void on_accept(boost::system::error_code ec,
							boost::asio::ip::tcp::socket socket) noexcept
		{
			if(ec)
				return fail(ec, "accept");

			if constexpr (Session::use_ssl == false)
			{
				//Plain
				std::make_shared<Session>(std::move(socket), share_)->run();
			}
			else
			{
//#if USE_SSL == 1
				//SSL
				std::make_shared<Session>(std::move(socket), ctx_, share_)->run();
//#endif
			}

			// Accept another connection
			do_accept();
		}
};

template<class Session>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ip::address const& address,
		unsigned short int port,
		std::shared_ptr<share<Session>> const& share,
    	boost::system::error_code& ec)
{
	static_assert(Session::use_ssl == false,
			"Session must be plain type");

	auto l = std::make_shared<Listener<Session>>(ioc, share);

	l->init(boost::asio::ip::tcp::endpoint{address, port}, ec);
	return l;
}

template<class Session>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::address const& address,
		unsigned short int port,
		std::shared_ptr<share<Session>> const& share,
    	boost::system::error_code& ec)
{
	static_assert(Session::use_ssl == true,
			"Session must be plain type");

	auto l = std::make_shared<Listener<Session>>(ioc, ctx, share);

	l->init(boost::asio::ip::tcp::endpoint{address, port}, ec);
	return l;
}

}//Agro

#endif /* AGRO_DAEMON_WEBSOCKET_LISTENER_HPP__ */
