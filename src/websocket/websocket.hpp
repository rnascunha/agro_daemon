#ifndef AGRO_MESH_WEBSOCKET_HPP__
#define AGRO_MESH_WEBSOCKET_HPP__

#include <string>
#include <filesystem>

#include "boost/asio.hpp"
#include "my_async.hpp"
#include "share.hpp"

#include "../coap_engine.hpp"
#include "../user/user.hpp"

enum class binary_type{
	json = 0,
	image = 1,
	app = 2,
	file = 3
};

template<bool UseSSL>
class Websocket final :
		public My_Async::Websocket::Session_Base<Websocket<UseSSL>, UseSSL, std::string>
		, public std::enable_shared_from_this<Websocket<UseSSL>>
{
		using self_type = Websocket<UseSSL>;
		using base_type = My_Async::Websocket::Session_Base<
				Websocket<UseSSL>, UseSSL, std::string>;
		using shared_type = share<Websocket<UseSSL>>;
	public:
		//Plain constructors
	    explicit
		Websocket(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket,
			std::shared_ptr<share<Websocket<UseSSL>>> const& share)
		  	  : base_type(std::move(socket)),
				timer_(base_type::stream_.get_executor()),
				share_(share){}

	    //SSL constructors
		explicit
		Websocket(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket,
			boost::asio::ssl::context& ctx,
			std::shared_ptr<share<Websocket<UseSSL>>> const& share)
			  : base_type(std::move(socket), ctx),
				timer_(base_type::stream_.get_executor()),
				share_(share){}

		~Websocket();

		void write_all(std::string const data) noexcept;
		void write_all(std::string const data, bool text) noexcept;
		void write_all_policy(Agro::Authorization::rule rule,
				std::shared_ptr<std::string const> data) noexcept;
		void write_policy(Agro::Authorization::rule rule,
				std::shared_ptr<std::string const> data) noexcept;
		void write_file(binary_type type,
				std::string const&,
				std::filesystem::path const&) noexcept;
		void write_binary(binary_type type,
				std::string const& name,
				std::string const& data) noexcept;

		Agro::User::Logged const& user() const noexcept;

		std::shared_ptr<share<Websocket<UseSSL>>> get_share() noexcept;
	protected:
		boost::asio::steady_timer	timer_;
		std::shared_ptr<share<Websocket<UseSSL>>>	share_;

		Agro::User::Logged user_;

		void on_open() noexcept override;
		void read_handler(std::string data) noexcept override;
		void fail(boost::system::error_code ec, char const* what) noexcept override;
		void on_write(boost::system::error_code ec, std::size_t) noexcept override;

		void get_file(const char* first_block, std::size_t block_size) noexcept;
		void get_image_file(const char* first_block, std::size_t block_size) noexcept;
		void get_app_file(const char* first_block, std::size_t block_size) noexcept;

		void error_auth(std::error_code const& ec, const char* what = nullptr) noexcept;
		bool check_authenticate(std::string const& data) noexcept;
};

#include "impl/websocket_impl.hpp"
#include "impl/binary_data_impl.hpp"
#include "impl/authenticate_impl.hpp"

#endif /* AGRO_MESH_WEBSOCKET_HPP__ */
