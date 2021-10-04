#include "tree.hpp"
#include <iostream>
#include "../coap_engine.hpp"
#include "../websocket/types.hpp"

namespace Agro{
namespace Device{

Tree::node::node(mesh_addr_t const& addr_)
 : addr(addr_){}

Tree::node::node(mesh_addr_t const& addr_, int layer_)
 : addr(addr_), layer(layer_){}

bool Tree::node::operator==(node const& rhs) const noexcept
{
	return addr == rhs.addr;
}

bool Tree::node::remove_child(mesh_addr_t const& caddr) noexcept
{
	if(!children) return false;
	if(children->addr == caddr)
	{
		children->parent = nullptr;
		node* next = children->next;
		children->next = nullptr;
		children = next;
		return true;
	}

	node* before = children;
	for(node* c = children->next; c; c = c->next)
	{
		if(c->addr == caddr)
		{
			before->next = c->next;
			c->parent = nullptr;
			c->next = nullptr;
			return true;
		}
		before = c;
	}
	return false;
}

bool Tree::node::is_descendent(mesh_addr_t const& child)const noexcept
{
	for(node* c = children; c; c = c->next)
	{
		if(c->addr == child) return true;
		if(c->is_descendent(child)) return true;
	}
	return false;
}

/**
 *
 */
Tree::tree_endpoint::tree_endpoint(mesh_addr_t const& addr_,
		endpoint const& ep_)
	: addr(addr_), ep(ep_){}

/**
 *
 */
Tree::Tree(Device_List const& list)
{
	for(auto const& l : list.list())
	{
		get_node(l.second.mac());
	}
}

void Tree::update(Device const& device) noexcept
{
	node& dev = get_node(device.mac());
	dev.layer = device.layer();

	node& parent = get_node(dev.layer == 1 ? device.parent() : mac_ap_to_addr(device.parent()));
	parent.layer = device.layer() - 1;

	add_child(parent, dev);

	if(parent.layer == 0)
	{
		add_child(root_, parent);
	}

	add_remove_descendent(dev, device.children_table());

	std::cout << "Size: " << nodes_.size() << "\n";
	print(root_, -1);

	//Endpoints
	if(device.layer() == 1)
	{
		add_endpoint(device.mac(), device.get_endpoint());
	}
	else
	{
		remove_endpoint(device.mac());
	}

	std::cout << "Endpoints:\n";
	print_endpoints();
}

std::map<mesh_addr_t const, Tree::tree_endpoint> const&
Tree::get_endpoint() const noexcept
{
	return endpoints_;
}

std::map<mesh_addr_t const, Tree::tree_endpoint>&
Tree::get_endpoint() noexcept
{
	return endpoints_;
}

bool Tree::remove_node(mesh_addr_t const& addr) noexcept
{
	return remove_node(root_, addr);
}

bool Tree::remove_node(node& device, mesh_addr_t const& addr) noexcept
{
	for(node* c = device.children; c; c = c->next)
	{
		if(c->addr == addr)
		{
			device.remove_child(addr);
			remove_endpoint(addr);
			return true;
		}
		remove_node(*c, addr);
	}

	return false;
}

void Tree::add_remove_descendent(node& device,
				std::vector<mesh_addr_t> const& children) noexcept
{
	remove_descendent(device, children);
//	add_descendent(device, children);
}

void Tree::remove_descendent(node& device,
				std::vector<mesh_addr_t> const& children) noexcept
{
	for(node* c = device.children; c; c = c->next)
	{
		bool is_child = false;
		for(auto const& caddr : children)
		{
			if(c->addr == caddr)
			{
				is_child = true;
				break;
			}
		}
		remove_descendent(*c, children);
		if(!is_child)
		{
			std::cout << "Removing " << c->addr.to_string() << "\n";
			device.remove_child(c->addr);
		}
	}
}

void Tree::add_descendent(node& device,
				std::vector<mesh_addr_t> const& children) noexcept
{
	for(auto const& c : children)
	{
		node& ch = get_node(c);
		if(!device.is_descendent(c))
		{
			add_child(device, ch);
		}
	}
}

Tree::node& Tree::get_node(mesh_addr_t const& addr) noexcept
{
	auto n = nodes_.find(addr);
	if(n == nodes_.end())
	{
		return nodes_.emplace(addr, node{addr}).first->second;
	}

	return n->second;
}

void Tree::add_child(node& parent, node& child) noexcept
{
	child.parent = &parent;
	node* children = parent.children;

	if(!children)
	{
		parent.children = &child;
	}
	else
	{
		for(node* c = children; c; c = c->next)
		{
			if(c && *c == child)
			{
				break;
			}
			else if(c->next == nullptr)
			{
				c->next = &child;
				child.next = nullptr;
				break;
			}
		}
	}
}

void Tree::add_endpoint(mesh_addr_t const& addr, endpoint const& ep) noexcept
{
	auto t = endpoints_.find(addr);
	if(t == endpoints_.end())
	{
		endpoints_.emplace(addr, tree_endpoint{addr, ep});
		return;
	}
}

void Tree::remove_endpoint(mesh_addr_t const& addr) noexcept
{
	endpoints_.erase(addr);
}

void Tree::print_endpoints() const noexcept
{
	for(auto const& [addr, ep] : endpoints_)
	{
		char str[INET_ADDRSTRLEN];
		std::cout << addr.to_string() << ": "
				<< ep.ep.address(str, INET_ADDRSTRLEN) << "\n";
	}
}

mesh_addr_t Tree::mac_ap_to_addr(mesh_addr_t const& addr) noexcept
{
	mesh_addr_t mac{addr};
	mac.addr[5]--;

	return mac;
}

void Tree::print(node const& node, int layer /* = -1 */) noexcept
{
	std::cout << layer << "|" << node.layer
			<< "[" << node.addr.to_string() << "]: ";
	for(auto* c = node.children; c; c = c->next)
	{
		std::cout << c->addr.to_string();
		if(c->next) std::cout << ", ";
	}
	std::cout << "\n";

	++layer;
	for(auto* c = node.children; c; c = c->next)
	{
		print(*c, layer);
	}
}

}//Device
}//Agro
