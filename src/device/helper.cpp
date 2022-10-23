#include "helper.hpp"
//#include <iostream>
#include "../helper/sha256.hpp"

static constexpr const unsigned app_max_name_size = 12;
struct app_packet{
	unsigned 	size = 0;
	sha256_hash	hash;
	char		name[app_max_name_size + 1];
};

static unsigned string_to_mac_impl(const char* str, unsigned size, mesh_addr_t& addr) noexcept;

bool string_to_mac(const char* str, unsigned size, mesh_addr_t& addr) noexcept
{
	return string_to_mac_impl(str, size, addr) == 6;
}

/**
 *
 */
unsigned string_to_mac_impl(const char* str, unsigned size, mesh_addr_t& addr) noexcept
{
	unsigned count = 0, count_part = 0;
	uint8_t value = 0;
	for(unsigned i = 0; i <= size; i++)
	{
		if(str[i] == ':' || i == size)
		{
			if(count_part == 0) addr.addr[count] = 0;
			else if (count_part == 1 || count_part == 2) addr.addr[count] = value;
			else return count;

			count_part = 0;
			count++;
			value = 0;

			continue;
		}
		if(count_part >= 2)
		{
			return count;
		}

		if(str[i] >= '0' && str[i] <= '9')
		{
			value = (str[i] - '0') | (value << (count_part * 4));
		}
		else if(str[i] >= 'a' && str[i] <= 'f')
		{
			value = (str[i] - 'a' + 10) | (value << (count_part * 4));
		}
		else if(str[i] >= 'A' && str[i] <= 'F')
		{
			value = (str[i] - 'A' + 10) | (value << (count_part * 4));
		}
		else
		{
			return count;
		}
		count_part++;
	}
	return count;
}

void read_packet_app_list(Agro::Device::Device& device,
						const void* data,
						std::size_t size) noexcept
{
	device.clear_apps();

	const std::uint8_t* data8 = static_cast<uint8_t const*>(data);
	for(std::size_t i = 0; i < size; i += sizeof(app_packet))
	{
		const app_packet* mapp = reinterpret_cast<const app_packet*>(data8);

		device.add_app(mapp->name, mapp->size, mapp->hash);
		data8 += sizeof(app_packet);
	}
}

mesh_addr_t mac_ap_to_addr(mesh_addr_t const& addr) noexcept
{
	mesh_addr_t mac{addr};
	mac.addr[5]--;

	return mac;
}
