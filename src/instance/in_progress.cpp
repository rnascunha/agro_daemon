#include "agro.hpp"
#include "../device/request/request_list.hpp"

namespace Agro{

bool instance::has_request_in_progress(mesh_addr_t const& address,
		CoAP::Message::code method,
		Device::Request::type req) const noexcept
{
	return requests_.has(address, method, req);
}

bool instance::add_request_in_progress(mesh_addr_t const& address,
		CoAP::Message::code method,
		Device::Request::type req,
		User::user_id id) noexcept
{
	return requests_.add(address, method, req, id);
}

bool instance::remove_request_in_progress(mesh_addr_t const& address,
		CoAP::Message::code method,
		Device::Request::type req) noexcept
{
	return requests_.remove(address, method, req);
}

}//Agro
