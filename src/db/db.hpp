#ifndef AGRO_DAEMON_DB_HPP__
#define AGRO_DAEMON_DB_HPP__

#include "sqlite3.hpp"
#include <system_error>
#include <string>
#include <vector>

#include "../user/list.hpp"
#include "../user/policy.hpp"
#include "../user/password.hpp"

#include "../device/list.hpp"
#include "../device/net.hpp"

namespace Agro{

class DB{
	public:
		DB(std::string const& db_name, std::error_code& ec);

		/**
		 * User interface
		 */
		bool read_users_groups(User::User_List&) noexcept;
		bool read_user_sessions(User::user_id, User::Session_List&) noexcept;
		bool read_user_subscriptions(User::user_id, User::Subscription_List&) noexcept;
		bool read_user_all_db(User::User_List&) noexcept;

		bool read_policy_types(Authorization::Policy_Types&) noexcept;
		bool read_policies(Authorization::Policies&) noexcept;

//		bool read_permission_list(Authorization::Permission_List&) noexcept;

		User::Info get_user(User::user_id id) noexcept;
		User::Info get_user(std::string const& username) noexcept;

		bool get_root_password(std::vector<unsigned char>& salt,
							std::vector<unsigned char>& pass) noexcept;
		bool get_password(std::string const& username,
				std::vector<unsigned char>& salt,
				std::vector<unsigned char>& pass) noexcept;

		int add_user(std::string const& username,
				User::key_password const password,
				User::salt_password const salt,
				std::string const& name,
				std::string const& email,
				User::user_id& id,
				User::Info::status status = User::Info::status::active) noexcept;

		int edit_user(User::user_id id,
				std::string const& username,
				std::string const& name,
				std::string const& email,
				std::vector<User::group_id> const& groups) noexcept;

		int delete_user(User::user_id) noexcept;
		int delete_user_from_group(User::user_id) noexcept;
//		int delete_permissions(Authorization::type,
//				Authorization::ref_type, Authorization::ref_id) noexcept;

		int delete_group(User::group_id) noexcept;

		int remove_policy_group(User::group_id) noexcept;

		Agro::User::group_id add_user_group(std::string const& name,
				std::string const& description,
				User::group_id&) noexcept;

		int add_user_to_group(User::group_id, User::user_id) noexcept;
		void add_user_to_group(User::group_id, std::vector<User::user_id> const&) noexcept;

		int set_user_to_groups(User::user_id, std::vector<User::group_id> const&) noexcept;
		int remove_all_user_groups(User::user_id) noexcept;

		bool update_user_session_id(User::user_id,
				std::string const&,
				std::string const& user_agent) noexcept;

		bool check_user_session_id(User::user_id,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) noexcept;

		bool clear_all_sessions() noexcept;
		bool clear_session(User::user_id) noexcept;
		bool clear_session_user_agent(User::user_id,
				std::string const& user_agent) noexcept;

		bool push_subscribe_user(User::user_id,
				std::string const& user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;

		bool push_unsubscribe_user(User::user_id,
				std::string const& user_agent) noexcept;

		std::string notify_private_key() noexcept;

		/**
		 * Device interface
		 */
		bool read_devices_net(Device::Net_List&) noexcept;
		bool read_devices(Device::Device_List&, Device::Net_List const&) noexcept;

		int add_device(mesh_addr_t const&, Device::device_id&) noexcept;
		int add_net(mesh_addr_t const&, Device::net_id&, std::string const& = "") noexcept;

		int update_device(Device::Device const&) noexcept;
		int update_device_route(Device::Device const&) noexcept;

		const char* error(){ return db_.error(); };
	private:
		sqlite3 db_;
};

}//Agro

#endif /* AGRO_DAEMON_DB_HPP__ */
