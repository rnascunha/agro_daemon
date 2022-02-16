#include "tree.hpp"

#if AGRO_DEVICE_TREE_PRINT_FUNCTIONS == 1
#include <iostream> //for the print functions
#endif /* AGRO_DEVICE_TREE_PRINT_FUNCTIONS */

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

bool Tree::node::operator!=(node const& rhs) const noexcept
{
	return !(*this == rhs);
}

void Tree::node::undo_branch() noexcept
{
	for(node* c = children; c; c = c->next)
	{
		c->undo_branch();
	}
	children = nullptr;
	next = nullptr;
	parent = nullptr;
	layer = -1;
}

bool Tree::node::remove_child(mesh_addr_t const& caddr) noexcept
{
	node* before = nullptr;
	for(node* c = children; c; before = c, c = c->next)
	{
		if(c->addr == caddr)
		{
			if(before)
			{
				before->next = c->next;
			}
			else
			{
				children = c->next;
			}
			c->undo_branch();
			return true;
		}
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

	node& parent = get_node(dev.layer == 1 ? device.parent() : Tree::mac_ap_to_addr(device.parent()));
	parent.layer = dev.layer - 1;

	bool change = false;

//	change = remove_descendent(dev, device.children_table()) || change;

	/**
	 * Update parent and children endpoint of device
	 */

	update_endpoints(device);
	/**
	 * Add device node as child of parent
	 */
	change = add_child(parent, dev) || change;
	if(parent.layer == 0)
	{
		/**
		 * If parent is a router (layer 0), add root as it parent (root is the daemon)
		 */
		change = add_child(root_, parent) || change;
	}

	change = remove_descendent(dev, device.children_table()) || change;
	change = add_descendent(dev, device.children_table()) || change;

	manage_device_root_endpoint(dev, device.get_endpoint());

	return change;
}

bool Tree::manage_device_root_endpoint(node const& node, endpoint const& ep) noexcept
{
	bool change = false;
	if(node.parent && node.parent->layer == 1)
	{
		/**
		 * If parent is a device root (layer 1)
		 */
		change = add_endpoint(node.parent->addr, ep);
	}
	else if(node.layer == 1)
	{
		/**
		 * If node is a device root (layer 1)
		 */
		return add_endpoint(node.addr, ep);
	}
	else
	{
		/**
		 * Else, remove node as root
		 */
		change = remove_endpoint(node.addr);
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
		{
			return true;
		}
	}
	return false;
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
			if(ch.parent)
			{
				ch.parent->remove_child(c);
			}
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
			if(*c == child)
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

mesh_addr_t Tree::mac_ap_to_addr(mesh_addr_t const& addr) noexcept
{
	mesh_addr_t mac{addr};
	mac.addr[5]--;

	return mac;
}

#if AGRO_DEVICE_TREE_PRINT_FUNCTIONS == 1
/**
 * Print functions
 */
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

//void Tree::print() const noexcept
//{
//	for(auto const& [addr, node] : nodes_)
//	{
//		std::cout << (node.parent ? node.parent->layer : -2) << std::flush;
//		std::cout << "|" << (node.parent ? node.parent->addr.to_string() : "null")  << std::flush;
//		std::cout << " -> " << std::flush;
//		std::cout << node.layer << "|" << node.addr << "\n";
//	}
//}

void Tree::print_all() const noexcept
{
	std::cout << "Tree:\n";
	print();
	std::cout << "Endpoints:\n";
	print_endpoints();
}

void Tree::print(node const& node, int layer /* = -1 */) noexcept
{
	std::cout << layer << "|" << node.layer
			<< "[" << node.addr << "]: ";
	for(auto* c = node.children; c; c = c->next)
	{
		std::cout << c->addr;
		if(c->next) std::cout << ", ";
	}
	std::cout << "\n";

	++layer;
	for(auto* c = node.children; c; c = c->next)
	{
		print(*c, layer);
	}
}
#endif /* AGRO_DEVICE_TREE_PRINT_FUNCTIONS == 1 */

}//Device
}//Agro
