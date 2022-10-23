#ifndef AGRO_MESH_WEBSOCKET_HPP__
#define AGRO_MESH_WEBSOCKET_HPP__

#include <string>
#include <filesystem>
#include <type_traits>

#include "boost/asio.hpp"
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
		public std::enable_shared_from_this<Websocket<UseSSL>>
{
		using self_type = Websocket<UseSSL>;
		using shared_type = share<Websocket<UseSSL>>;
	public:
		static constexpr const bool use_ssl = UseSSL;
		static constexpr const std::size_t buffer_size = 2048;

		//Plain constructors
		template<bool SSL = UseSSL>
	    explicit
		Websocket(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket,
			std::shared_ptr<share<Websocket<UseSSL>>> const& share,
			typename std::enable_if<!SSL>::type* = 0)
		  	  : stream_(std::move(socket)),
				timer_(stream_.get_executor()),
				share_(share){}

	    //SSL constructors
		template<bool SSL = UseSSL>
		explicit
		Websocket(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket,
			boost::asio::ssl::context& ctx,
			std::shared_ptr<share<Websocket<UseSSL>>> const& share,
			typename std::enable_if<SSL>::type* = 0)
			  : stream_(std::move(socket), ctx),
				timer_(stream_.get_executor()),
				share_(share){}

		~Websocket();

		/**
		 *
		 */
		void run() noexcept;

		void close(const boost::beast::websocket::close_reason&) noexcept;

		void write(std::shared_ptr<std::string const> const& data) noexcept;
		void write(std::shared_ptr<std::string const> const& data, bool text) noexcept;
		void write(std::string const data) noexcept;
		void write(std::string const data, bool text) noexcept;

		/*Essa funcao foi criada para desambiguar a funcao write chamada em um bind...*/
		void write_share(std::shared_ptr<std::string const> const&) noexcept;
		void write_share2(std::shared_ptr<std::string const> const&, bool text) noexcept;

		void binary(bool) noexcept;
		void text(bool) noexcept;
		bool binary() noexcept;
		bool text() noexcept;

		/**
		 *
		 * @param data
		 */

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
		using Stream = typename std::conditional<UseSSL == false,
			boost::beast::websocket::stream<boost::beast::tcp_stream>,
			boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>>>::
			type;

		Stream stream_;

		boost::asio::steady_timer	timer_;
		std::shared_ptr<share<Websocket<UseSSL>>>	share_;

		Agro::User::Logged user_;

		std::vector<std::shared_ptr<std::string const>> queue_;
		std::string buffer_;

		/**
		 *
		 */
		void on_run() noexcept;

		//Do SSL handshake
		template<bool SSL = UseSSL>
		typename std::enable_if<SSL>::type
		on_handshake(boost::system::error_code ec);

		void on_accept(boost::system::error_code ec);

		void do_read() noexcept;
		void on_read(boost::system::error_code ec, std::size_t bytes_transferred) noexcept;

		void do_write() noexcept;
		void writing(std::shared_ptr<std::string const> const& data) noexcept;

		void set_run_options();

		void closed(boost::beast::error_code const&);
		void on_close(boost::beast::error_code) noexcept{}
		/**
		 *
		 */

		void on_open() noexcept;
		void read_handler(std::string data) noexcept;
		void fail(boost::system::error_code ec, char const* what) noexcept;
		void on_write(boost::system::error_code ec, std::size_t) noexcept;
		void on_write_base(boost::system::error_code ec, std::size_t) noexcept;

		void get_file(const char* first_block, std::size_t block_size) noexcept;
		void get_image_file(const char* first_block, std::size_t block_size) noexcept;
		void get_app_file(const char* first_block, std::size_t block_size) noexcept;

		void error_auth(std::error_code const& ec, const char* what = nullptr) noexcept;
		bool check_authenticate(std::string const& data) noexcept;

		void send_initial_data() noexcept;

		void clear_read_buffer() noexcept;
};

#include "impl/websocket_base_impl.hpp"
#include "impl/websocket_impl.hpp"
#include "impl/read_impl.hpp"
#include "impl/binary_data_impl.hpp"
#include "impl/authenticate_impl.hpp"

#endif /* AGRO_MESH_WEBSOCKET_HPP__ */
