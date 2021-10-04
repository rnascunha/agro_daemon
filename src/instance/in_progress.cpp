#include "agro.hpp"

namespace Agro{

bool instance::has_request_in_progress(mesh_addr_t const& address,
		CoAP::Message::code method,
		::Message::requests req) const noexcept
{
	return requests_.has(address, method, req);
}

bool instance::add_request_in_progress(mesh_addr_t const& address,
		CoAP::Message::code method,
		::Message::requests req,
		User::user_id id) noexcept
{
	return requests_.add(address, method, req, id);
}

bool instance::remove_request_in_progress(mesh_addr_t const& address,
		CoAP::Message::code method,
		::Message::requests req) noexcept
{
	return requests_.remove(address, method, req);
}

}//Agro
