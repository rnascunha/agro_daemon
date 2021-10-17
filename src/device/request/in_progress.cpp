#include "in_progress.hpp"
#include <algorithm>

namespace Agro{
namespace Device{
namespace Request{

request_in_progress::request_in_progress(mesh_addr_t const& address,
			CoAP::Message::code code,
			type req,
			Agro::User::user_id uid)
	: addr(address), method(code), request(req), id(uid){}

bool request_in_progress::is_equal(mesh_addr_t const& address,
		CoAP::Message::code code,
		type req) const noexcept
{
	return addr == address && method == code && request == req;
}

bool Request_in_Pogress_List::has(mesh_addr_t const& address,
		CoAP::Message::code method,
		type req) const noexcept
{
	for(auto const& r : list_)
	{
		if(r.is_equal(address, method, req))
			return true;
	}
	return false;
}

bool Request_in_Pogress_List::add(mesh_addr_t const& address,
		CoAP::Message::code method,
		type req,
		Agro::User::user_id uid) noexcept
{
	if(has(address, method, req))
	{
		return false;
	}

	list_.emplace_back(address, method, req, uid);

	return true;
}

bool Request_in_Pogress_List::remove(mesh_addr_t const& address,
		CoAP::Message::code method,
		type req) noexcept
{
	auto p = std::remove_if(list_.begin(), list_.end(),
			[&address, &method, &req](request_in_progress const& rp){
		return rp.is_equal(address, method, req);
	});

	if(p == list_.end())
	{
		return false;
	}

	list_.erase(p, list_.end());

	return true;
}

}//Request
}//Device
}//Agro
