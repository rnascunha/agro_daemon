#include "types.hpp"
#include "helper.hpp"
#include <cstring>
#include <cstdio>

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

mesh_addr_t::mesh_addr_t(const char* str, unsigned size, Error& ec)
{
	if(!set(str, size))
	{
		ec = Error::invalid_value;
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

mesh_addr_t& mesh_addr_t::operator=(Resource::mesh_addr_t const& rhs) noexcept
{
	std::memcpy(addr, &rhs, sizeof(this->addr));
	return *this;
}


char* mesh_addr_t::to_string(char* str, unsigned size) const noexcept
{
	std::snprintf(str, size, MACSTR, MAC2STR(addr));

	return str;
}
