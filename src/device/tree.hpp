#ifndef AGRO_DAEMON_DEVICE_TREE_MANAGER_HPP__
#define AGRO_DAEMON_DEVICE_TREE_MANAGER_HPP__

#include <map>
#include "device.hpp"
#include "list.hpp"
#include "../coap_engine.hpp"

namespace Agro{
namespace Device{

void check_root(mesh_addr_t const&) noexcept;

class Tree{
	public:
		Tree(Device_List const&);

		struct node{
			node() = default;
			node(mesh_addr_t const&);
			node(mesh_addr_t const&, int layer);

			mesh_addr_t addr;
			int			layer = -1;
			node*		next = nullptr;
			node* 		parent = nullptr;
			node*		children = nullptr;

			bool operator==(node const&) const noexcept;

			bool remove_child(mesh_addr_t const&) noexcept;
			bool is_descendent(mesh_addr_t const&)const noexcept;
		};

		struct tree_endpoint{
			tree_endpoint(mesh_addr_t const& addr, endpoint const&);

			mesh_addr_t	addr;
			endpoint	ep;
			bool		checking = false;
		};

		void update(Device const&) noexcept;
		bool remove_node(mesh_addr_t const&) noexcept;

		std::map<mesh_addr_t const, tree_endpoint> const&
		get_endpoint() const noexcept;
		std::map<mesh_addr_t const, tree_endpoint>&
		get_endpoint() noexcept;

		void print_endpoints() const noexcept;

		static mesh_addr_t mac_ap_to_addr(mesh_addr_t const&) noexcept;
		static void print(node const&, int layer = -1) noexcept;
	private:
		node& get_node(mesh_addr_t const&) noexcept;
		void add_child(node& parent, node& child) noexcept;
		void add_remove_descendent(node&,
				std::vector<mesh_addr_t> const& children) noexcept;
		void remove_descendent(node&,
				std::vector<mesh_addr_t> const& children) noexcept;
		void add_descendent(node&,
				std::vector<mesh_addr_t> const& children) noexcept;

		bool remove_node(node& device, mesh_addr_t const&) noexcept;

		void add_endpoint(mesh_addr_t const&, endpoint const&) noexcept;
		void remove_endpoint(mesh_addr_t const&) noexcept;

		std::map<mesh_addr_t const, node>
							nodes_;
		std::map<mesh_addr_t const, tree_endpoint>
							endpoints_;
		node				root_;
};

}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_TREE_MANAGER_HPP__ */
