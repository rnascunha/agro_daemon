#ifndef AGRO_MESH_WEBSOCKET_HPP__
#define AGRO_MESH_WEBSOCKET_HPP__

#include "boost/asio.hpp"
#include "my_async.hpp"
#include "my_async/util/static_shareable.hpp"
#include <string>

#include "../agro.hpp"
#include "../coap_engine.hpp"
//#include "../device/list.hpp"
//#include "../db/db.hpp"

#include "../user/user.hpp"

template<bool UseSSL>
class Websocket final :
		public My_Async::Websocket::Session_Base<Websocket<UseSSL>, UseSSL, std::string>
		, public My_Async::Util::Static_Shareable<Websocket<UseSSL>, false>
		, public std::enable_shared_from_this<Websocket<UseSSL>>
{
		using self_type = Websocket<UseSSL>;
		using base_type = My_Async::Websocket::Session_Base<
				Websocket<UseSSL>, UseSSL, std::string>;
		using shared_type = My_Async::Util::Static_Shareable<Websocket<UseSSL>, false>;
	public:
		//Plain constructors
	    explicit
		Websocket(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket)
		  : base_type(std::move(socket)), timer_(base_type::stream_.get_executor()){}

		~Websocket();

		static void data(Agro::instance& instance) noexcept
		{
			instance_ = &instance;
		}
		static engine& coap_engine() noexcept { return instance_->coap_engine; }

		static void write_all(std::string const data) noexcept;
		static void write_all(std::string const data, bool text) noexcept;
	protected:
		static Agro::instance*	instance_;
		boost::asio::steady_timer timer_;

		Agro::User user_;

		void on_open() noexcept override;
		void read_handler(std::string data) noexcept override;
		void fail(boost::system::error_code ec, char const* what) noexcept override;

		void error_auth(std::error_code const& ec, const char* what = nullptr) noexcept;
		bool check_authenticate(std::string const& data) noexcept;
};

template<bool UseSSL>
Agro::instance* Websocket<UseSSL>::instance_ = nullptr;

#include "impl/websocket_impl.hpp"
#include "impl/authenticate_impl.hpp"

#endif /* AGRO_MESH_WEBSOCKET_HPP__ */
