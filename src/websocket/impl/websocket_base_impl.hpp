#ifndef AGRO_MESH_WEBSOCKET_BASE_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_BASE_IMPL_HPP__

#include "../websocket.hpp"

template<bool UseSSL>
void
Websocket<UseSSL>::
run() noexcept{
	boost::asio::dispatch(stream_.get_executor(),
		boost::beast::bind_front_handler(
			&self_type::on_run,
			this->shared_from_this()
		)
	);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_run() noexcept{
	if constexpr(UseSSL == false){ //Plain
		set_run_options();

		// Accept the websocket handshake
		stream_.async_accept(
			boost::beast::bind_front_handler(
				&self_type::on_accept,
				this->shared_from_this()
			)
		);
	}
	else
	{ //SSL
		// Set the timeout.
		boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

		 // Perform the SSL handshake
		stream_.next_layer().async_handshake(
			boost::asio::ssl::stream_base::server,
			boost::beast::bind_front_handler(
				&self_type::on_handshake<UseSSL>,
				this->shared_from_this()));
	}
}

template<bool UseSSL>
template<bool SSL /* = UseSSL */>
typename std::enable_if<SSL>::type
Websocket<UseSSL>::
on_handshake(boost::system::error_code ec)
{
	if(ec)
	{
		fail(ec, "handshake");
		return;
	}
	// Turn off the timeout on the tcp_stream, because
	// the websocket stream has its own timeout system.
	boost::beast::get_lowest_layer(stream_).expires_never();

	set_run_options();

	// Accept the websocket handshake
	stream_.async_accept(
		boost::beast::bind_front_handler(
			  &self_type::on_accept,
			this->shared_from_this()));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
close(const boost::beast::websocket::close_reason& reason) noexcept
{
	stream_.async_close(reason,
			boost::beast::bind_front_handler(
					&self_type::closed,
					this->shared_from_this()));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
closed(boost::beast::error_code const& ec)
{
	on_close(ec);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_accept(boost::system::error_code ec){
	if(ec)
		return fail(ec, "accept");

	do_read();
	on_open();
}

template<bool UseSSL>
void
Websocket<UseSSL>::
do_read() noexcept{
	stream_.async_read_some(
        boost::asio::dynamic_buffer(buffer_).prepare(buffer_size),
        std::bind(
            &self_type::on_read,
			this->shared_from_this(),
			std::placeholders::_1,	//error_code
			std::placeholders::_2	//bytes transfered
		)
    );
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_read(boost::system::error_code ec, std::size_t bytes_transfered) noexcept
{
    if(ec)
        return fail(ec, "read");

    buffer_.resize(bytes_transfered);
	read_handler(buffer_);
	buffer_.erase(buffer_.begin(), buffer_.end());

    do_read();
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write(std::shared_ptr<std::string const> const& data) noexcept
{
    boost::asio::post(
		stream_.get_executor(),
        std::bind(
            &self_type::writing,
			this->shared_from_this(),
            data)
    );
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write(std::shared_ptr<std::string const> const& data, bool text) noexcept
{
	this->text(text);
	write(data);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write(std::string const data) noexcept
{
	write(std::make_shared<std::string const>(std::move(data)));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write(std::string const data, bool text) noexcept
{
	this->text(text);
	write(std::make_shared<std::string const>(std::move(data)));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_share(std::shared_ptr<std::string const> const& data) noexcept
{
	write(data);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_share2(std::shared_ptr<std::string const> const& data, bool text) noexcept
{
	this->text(text);
	write(data);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
do_write() noexcept
{
	stream_.async_write(
		boost::asio::buffer(*queue_.front()),
		std::bind(
			&self_type::on_write,
			this->shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		)
	);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
writing(std::shared_ptr<std::string const> const& data) noexcept
{
    queue_.push_back(data);

    if(queue_.size() > 1) return;

    do_write();
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_write_base(boost::system::error_code ec, std::size_t) noexcept
{
    if(ec) return fail(ec, "write");

    queue_.erase(queue_.begin());

    if(! queue_.empty()){
        do_write();
	}
}

template<bool UseSSL>
void
Websocket<UseSSL>::
set_run_options()
{
	// Set suggested timeout settings for the websocket
	stream_.set_option(
		boost::beast::websocket::stream_base::timeout::suggested(
			boost::beast::role_type::server));

	// Set a decorator to change the Server of the handshake
	stream_.set_option(boost::beast::websocket::stream_base::decorator(
		[](boost::beast::websocket::response_type& res)
		{
			res.set(boost::beast::http::field::server,
				std::string(BOOST_BEAST_VERSION_STRING) +
					" serial_websocket");
		}));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
binary(bool b) noexcept
{
	stream_.binary(b);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
text(bool b) noexcept
{
	stream_.text(b);
}

template<bool UseSSL>
bool
Websocket<UseSSL>::
binary() noexcept
{
	return stream_.binary();
}

template<bool UseSSL>
bool
Websocket<UseSSL>::
text() noexcept
{
	return stream_.text();
}

#endif /* AGRO_MESH_WEBSOCKET_BASE_IMPL_HPP__ */
