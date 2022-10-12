#ifndef AGRO_MESH_WEBSOCKET_IMPL_HPP__
#define AGRO_MESH_WEBSOCKET_IMPL_HPP__

#include "../websocket.hpp"

#include "boost/beast.hpp"

#include <string>

#include <utility>
#include <fstream>

#include "tt/tt.hpp"

template<bool UseSSL>
Websocket<UseSSL>::~Websocket()
{
	share_->leave(this);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_all(std::string const data) noexcept
{
	share_->exec_to_all(
		std::bind(
			&self_type::write_share,
			std::placeholders::_1,
			std::make_shared<std::string const>(std::move(data))
		)
	);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_all(std::string const data, bool text) noexcept
{
	share_->exec_to_all(
		std::bind(
			&self_type::write_share,
			std::placeholders::_1,
			std::make_shared<std::string const>(std::move(data))
		)
	);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_all_policy(Agro::Authorization::rule rule,
		std::shared_ptr<std::string const> data) noexcept
{
	share_->exec_policy(rule,
			std::bind(
				&self_type::write_share,
				std::placeholders::_1,
				data));
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_policy(Agro::Authorization::rule rule,
				std::shared_ptr<std::string const> data) noexcept
{
	if(Agro::Authorization::can(user_, rule))
	{
		this->write(data);
	}
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_file(binary_type type,
				std::string const& name,
				std::filesystem::path const& path) noexcept
{
	tt::status("Sending file [%d] %s of path %s",
			static_cast<int>(type),
			name.c_str(), path.c_str());

	std::ifstream in(path, std::ios::binary);
	std::stringstream ss;

	std::uint8_t t = static_cast<std::uint8_t>(type);
	ss.write(reinterpret_cast<char const*>(&t), sizeof(t));
	std::uint16_t size = name.size();
	ss.write(reinterpret_cast<char const*>(&size), sizeof(size));
	ss << name;
	ss << in.rdbuf();

	this->write(std::make_shared<std::string>(ss.str()), false);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
write_binary(binary_type type,
				std::string const& name,
				std::string const& data) noexcept
{
	tt::status("Sending binary data [%d/size=%zu] %s",
			static_cast<int>(type), data.size(),
			name.c_str());

	std::stringstream ss;

	std::uint8_t t = static_cast<std::uint8_t>(type);
	ss.write(reinterpret_cast<char const*>(&t), sizeof(t));
	std::uint16_t size = static_cast<std::uint16_t>(name.size());
	ss.write(reinterpret_cast<char const*>(&size), sizeof(size));
	ss << name;
	ss << data;

	this->write(std::make_shared<std::string>(ss.str()), false);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_write(boost::system::error_code ec, std::size_t bytes_transfered) noexcept
{
	on_write_base(ec, bytes_transfered);
	text(true);
}

template<bool UseSSL>
Agro::User::Logged const&
Websocket<UseSSL>::
user() const noexcept
{
	return user_;
}

template<bool UseSSL>
void
Websocket<UseSSL>::
on_open() noexcept
{
	this->text(true);
	share_->join(this);
}

template<bool UseSSL>
void
Websocket<UseSSL>::
fail(boost::system::error_code ec, char const* what) noexcept
{
//	if(ec == boost::asio::error::operation_aborted ||
//	   ec == boost::beast::websocket::error::closed)
//	{
//		return;
//	}

	tt::error("%s[%d]: %s", what, ec.value(), ec.message().c_str());
}

template<bool UseSSL>
std::shared_ptr<share<Websocket<UseSSL>>>
Websocket<UseSSL>::get_share() noexcept
{
	return share_;
}

#endif /* AGRO_MESH_WEBSOCKET_IMPL_HPP__ */
