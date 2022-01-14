#include "tree.hpp"

#include <iostream> //for the print functions

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
		children->children = nullptr;
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
			c->children = nullptr;
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
Tree::Tree(Device_List& list)
	: dev_list_{list}{}

void Tree::read_device_list() noexcept
{
	for(auto const& l : dev_list_.list())
	{
		get_node(l.second.mac());
	}
}

bool Tree::update(Device const& device) noexcept
{
	node& dev = get_node(device.mac());
	dev.layer = device.layer();

	node& parent = get_node(dev.layer == 1 ? device.parent() : mac_ap_to_addr(device.parent()));
	parent.layer = device.layer() - 1;

	bool change = false;

	update_endpoints(device);
	change = add_child(parent, dev) || change;

	if(parent.layer == 0)
	{
		change = add_child(root_, parent) || change;
	}

	if(parent.layer == 1)
	{
		change = add_endpoint(parent.addr, device.get_endpoint()) || change;
	}

	change = add_remove_descendent(dev, device.children_table()) || change;
	//Endpoints
	if(device.layer() == 1)
	{
		change = add_endpoint(device.mac(), device.get_endpoint()) || change;
	}
	else
	{
		change = remove_endpoint(device.mac()) || change;
	}

	return change;
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
		if(remove_node(*c, addr))
			return true;
	}
	return false;
}

bool Tree::add_remove_descendent(node& device,
				std::vector<mesh_addr_t> const& children) noexcept
{
	bool change = remove_descendent(device, children);
	return add_descendent(device, children) || change;
}

bool Tree::remove_descendent(node& device,
				std::vector<mesh_addr_t> const& children) noexcept
{
	bool change = false;
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
		change = remove_descendent(*c, children) || change;
		if(!is_child)
		{
			device.remove_child(c->addr);
			change = true;
		}
	}
	return change;
}

bool Tree::add_descendent(node& device,
				std::vector<mesh_addr_t> const& children) noexcept
{
	bool change = false;
	for(auto const& c : children)
	{
		if(!device.is_descendent(c))
		{
			node& ch = get_node(c);
			add_child(device, ch);
			change = true;
		}
	}

	return change;
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

bool Tree::add_child(node& parent, node& child) noexcept
{
	bool change = false;
	child.parent = &parent;
	node* children = parent.children;

	child.layer = parent.layer + 1;
	if(!children)
	{
		parent.children = &child;
		child.next = nullptr;
		change = true;
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
				change = true;
				break;
			}
		}
	}
	return change;
}

std::vector<mesh_addr_t> Tree::unconnected() const noexcept
{
	std::vector<mesh_addr_t> un;

	for(auto const& [addr, node] : nodes_)
	{
		if(!root_.is_descendent(addr))
		{
			un.push_back(addr);
		}
	}

	return un;
}

Tree::node const& Tree::root() const noexcept
{
	return root_;
}

bool Tree::add_endpoint(mesh_addr_t const& addr, endpoint const& ep) noexcept
{
	auto t = endpoints_.find(addr);
	if(t == endpoints_.end())
	{
		endpoints_.emplace(addr, tree_endpoint{addr, ep});
		return true;
	}

	return false;
}

bool Tree::remove_endpoint(mesh_addr_t const& addr) noexcept
{
	return endpoints_.erase(addr);
}

bool Tree::uncheck_endpoint(mesh_addr_t const& addr) noexcept
{
	auto t = endpoints_.find(addr);
	if(t == endpoints_.end())
	{
		return false;
	}

	t->second.checking = false;
	return true;
}

void Tree::update_endpoints(Device const& device) const noexcept
{
	if(device.layer() >= 2)
	{
		update_device_endpoint(device.parent(), device.get_endpoint());
	}
	update_device_endpoint(device.children_table(), device.get_endpoint());
}

void Tree::update_device_endpoint(mesh_addr_t const& addr, endpoint const& ep) const noexcept
{
	auto* dev = dev_list_[addr];
	if(!dev) return;

	dev->update_endpoint(ep);
}

void Tree::update_device_endpoint(std::vector<mesh_addr_t> const& list,
		endpoint const& ep) const noexcept
{
	for(auto const& a : list)
	{
		update_device_endpoint(a, ep);
	}
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

void Tree::print() const noexcept
{
	print(root_, -1);
}

void Tree::print_all() const noexcept
{
	std::cout << "Tree:\n";
	print();
	std::cout << "Endpoints:\n";
	print_endpoints();
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
