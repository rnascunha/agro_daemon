#ifndef AGRO_MESH_WEBSOCKET_HPP__
#define AGRO_MESH_WEBSOCKET_HPP__

#include "boost/asio.hpp"
#include "coap_engine.hpp"
#include "my_async.hpp"
#include "my_async/util/static_shareable.hpp"
#include <string>

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
		using base_type::Session_Base;
		~Websocket();

		static void coap_engine(engine& eng) noexcept { coap_engine_ = &eng; }
		static engine& coap_engine() noexcept { return *coap_engine_; }

		static void write_all(std::string const data) noexcept;
		static void write_all(std::string const data, bool text) noexcept;
	protected:
		static engine* coap_engine_;

		void on_open() noexcept override;
		void read_handler(std::string data) noexcept override;
		void fail(boost::system::error_code ec, char const* what) noexcept override;
};

template<bool UseSSL>
engine* Websocket<UseSSL>::coap_engine_ = nullptr;

#include "impl/websocket_impl.hpp"

#endif /* AGRO_MESH_WEBSOCKET_HPP__ */
