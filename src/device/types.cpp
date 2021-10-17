#include "types.hpp"
#include "helper.hpp"
#include <cstring>
#include <cstdio>
#include <chrono>

value_time get_time() noexcept
{
	return static_cast<value_time>(std::chrono::duration_cast<std::chrono::seconds>(
								std::chrono::system_clock::now().time_since_epoch()).count());
}

mesh_addr_t::mesh_addr_t()
{
	std::memset(addr, 0, sizeof(this->addr));
}

mesh_addr_t::mesh_addr_t(mesh_addr_t const& mac)
{
	std::memcpy(addr, &mac, sizeof(addr));
}

mesh_addr_t::mesh_addr_t(std::uint8_t const* addr)
{
	std::memcpy(this->addr, addr, sizeof(this->addr));
}

mesh_addr_t::mesh_addr_t(const char* str, unsigned size, std::error_code& ec)
{
	if(!set(str, size))
	{
		ec = make_error_code(Error::invalid_value);
		std::memset(addr, 0, sizeof(this->addr));
	}
}

bool mesh_addr_t::set(const char* str, unsigned size) noexcept
{
	return string_to_mac(str, size, *this);
}

bool mesh_addr_t::operator==(mesh_addr_t const& rhs) const noexcept
{
	return std::memcmp(addr, &rhs, sizeof(this->addr)) == 0;
}

bool mesh_addr_t::operator!=(mesh_addr_t const& rhs) const noexcept
{
	return !(*this == rhs);
}

bool mesh_addr_t::operator<(mesh_addr_t const& rhs) const noexcept
{
	return std::memcmp(addr, &rhs, sizeof(this->addr)) < 0;
}

mesh_addr_t& mesh_addr_t::operator=(mesh_addr_t const& rhs) noexcept
{
	std::memcpy(addr, &rhs, sizeof(this->addr));
	return *this;
}

mesh_addr_t& mesh_addr_t::operator=(Agro::Device::Resource::mesh_addr_t const& rhs) noexcept
{
	std::memcpy(addr, &rhs, sizeof(this->addr));
	return *this;
}


char* mesh_addr_t::to_string(char* str, unsigned size) const noexcept
{
	std::snprintf(str, size, MACSTR, MAC2STR(addr));

	return str;
}

std::string mesh_addr_t::to_string() const noexcept
{
	char buf[18];
	return to_string(buf, 18);
}
