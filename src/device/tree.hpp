#ifndef AGRO_DAEMON_DEVICE_TREE_MANAGER_HPP__
#define AGRO_DAEMON_DEVICE_TREE_MANAGER_HPP__

#include <map>
#include "device.hpp"
#include "list.hpp"
#include "../coap_engine.hpp"

namespace Agro{
namespace Device{

//#define AGRO_DEVICE_TREE_PRINT_FUNCTIONS 1

void check_root(mesh_addr_t const&) noexcept;

class Tree{
	public:
		static constexpr const int undefined = -2;
		static constexpr const int daemon = -1;

		Tree(Device_List&);

		void read_device_list() noexcept;

		struct node{
			node() = default;
			node(mesh_addr_t const&);
			node(mesh_addr_t const&, int layer);

			mesh_addr_t addr;
			int			layer = undefined;
			node*		next = nullptr;
			node* 		parent = nullptr;
			node*		children = nullptr;

			bool operator==(node const&) const noexcept;
			bool operator!=(node const&) const noexcept;

			bool remove_child(mesh_addr_t const&) noexcept;
			void undo_branch() noexcept;
			bool is_descendent(mesh_addr_t const&)const noexcept;
		};

		struct tree_endpoint{
			tree_endpoint(mesh_addr_t const& addr, endpoint const&);

			mesh_addr_t	addr;
			endpoint	ep;
			bool		checking = false;
		};

		bool update(Device const&) noexcept;
		bool remove_node(mesh_addr_t const&) noexcept;

		std::vector<mesh_addr_t> unconnected() const noexcept;
		node const& root() const noexcept;

		std::map<mesh_addr_t const, tree_endpoint> const&
		get_endpoint() const noexcept;
		std::map<mesh_addr_t const, tree_endpoint>&
		get_endpoint() noexcept;
		bool uncheck_endpoint(mesh_addr_t const&) noexcept;

#if AGRO_DEVICE_TREE_PRINT_FUNCTIONS == 1
		void print_endpoints() const noexcept;
		void print() const noexcept;

		void print_all() const noexcept;

		static void print(node const&, int layer) noexcept;
#endif /* AGRO_DEVICE_TREE_PRINT_FUNCTIONS == 1 */
	private:
		node& get_node(mesh_addr_t const&) noexcept;
		bool add_child(node& parent, node& child) noexcept;
		bool add_remove_descendent(node&,
				std::vector<mesh_addr_t> const& children) noexcept;
		bool remove_descendent(node&,
				std::vector<mesh_addr_t> const& children) noexcept;
		bool add_descendent(node&,
				std::vector<mesh_addr_t> const& children) noexcept;

		bool remove_node(node& device, mesh_addr_t const&) noexcept;

		/**
		 * Endpoints
		 */
		bool add_endpoint(mesh_addr_t const&, endpoint const&) noexcept;
		bool remove_endpoint(mesh_addr_t const&) noexcept;
		bool manage_device_root_endpoint(node const&, endpoint const&) noexcept;

		void update_endpoints(Device const&) const noexcept;
		void update_device_endpoint(mesh_addr_t const&, endpoint const&) const noexcept;
		void update_device_endpoint(std::vector<mesh_addr_t> const&, endpoint const&) const noexcept;

		bool bug_inconsistency_fix() noexcept;

		Device_List&		dev_list_;
		std::map<mesh_addr_t const, node>
							nodes_;
		std::map<mesh_addr_t const, tree_endpoint>
							endpoints_;
		node				root_;
};

}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_TREE_MANAGER_HPP__ */
