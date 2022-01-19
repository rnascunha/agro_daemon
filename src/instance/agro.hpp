#ifndef AGRO_DAEMON_AGRO_HPP__
#define AGRO_DAEMON_AGRO_HPP__

#include <memory>
#include <vector>

#include "../db/db.hpp"
#include "../coap_engine.hpp"
#include "../websocket/types.hpp"

#include "../device/list.hpp"
#include "../device/net.hpp"
#include "../device/tree.hpp"

#include "../sensor/sensor_type_list.hpp"

#include "../notify/factory.hpp"

#include "../user/list.hpp"
#include "../user/policy.hpp"

#include "../image/image.hpp"
#include "../app/app.hpp"

#include "../message/user_types.hpp"
#include "../message/report.hpp"
#include "../device/request/in_progress.hpp"

static constexpr const char* images_path = "images";
static constexpr const char* apps_path = "apps";

namespace Agro{

class instance{
	public:
		instance(boost::asio::io_context& ioc,
					DB&&,
					Notify::Factory&&,
					udp_conn::endpoint& ep,
					boost::asio::ip::tcp::endpoint const& epl,
		#if USE_SSL == 1
					std::string const& ssl_key,
					std::string const& ssl_cert,
		#endif /**/
					std::error_code& ec);

		template<unsigned TimeoutMs>
		void run(int num_threads, CoAP::Error& ecp) noexcept;

		/**
		 * User interface
		 */
		bool check_user_session_id(
						User::user_id id,
						std::string const& session_id,
						std::string const& user_agent,
						long& session_time) const noexcept;

		User::User* get_user(std::string const& username) noexcept;
		User::User const* get_user(User::user_id) const noexcept;
		bool get_user_password(std::string const& username,
						std::vector<unsigned char>& salt,
						std::vector<unsigned char>& pass) noexcept;

		bool add_user(std::string const& username,
						std::string const& name,
						std::string const& password,
						std::string const& email,
						std::string const& telegram_chat_id,
						User::user_id& id) noexcept;
		bool add_user_to_group(User::user_id, User::group_id) noexcept;
		void set_user_to_groups(User::user_id, std::vector<User::group_id> const&) noexcept;

		bool edit_user(User::user_id,
				std::string const& username,
				std::string const& name,
				std::string const& email,
				std::string const& telegram_chat_id,
				std::vector<User::group_id> const&) noexcept;

		bool delete_user(User::user_id) noexcept;

		User::Group const* get_group_info(User::group_id) const noexcept;
		bool add_group(std::string const& name,
						std::string const& description,
						std::vector<User::user_id> const&,
						User::group_id&) noexcept;
		bool delete_group(User::group_id) noexcept;

		bool push_notify_is_valid() const noexcept;
		std::string_view const& get_notify_public_key() const noexcept;

		bool update_user_session_id(User::user_id,
						std::string const& session_id,
						std::string const& user_agent) noexcept;

		void push_subscribe_user(User::user_id,
				std::string const& user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;

		void push_unsubscribe_user(User::user_id,
				std::string const& user_agent) noexcept;

		void clear_session_user_agent(User::user_id,
				std::string const& user_agent) noexcept;

		void notify_all(std::string const& data) noexcept;
		void notify_all_policy(Authorization::rule rule, std::string const& data) noexcept;
		void notify_all_policy(Authorization::rule rule,
				Notify::general_type type,
				std::string const& data) noexcept;
		void notify_all_policy(Authorization::rule rule,
				Notify::device_type type,
				std::vector<std::reference_wrapper<Device::Device const>> const&,
				const char* status) noexcept;
		void notify_all_policy(Authorization::rule,
				Device::Device const&,
				Sensor::sensor_type const&,
				Sensor::sensor_description const*) noexcept;

		bool update_push_credential(bool enable) noexcept;
		bool update_telegram_credential(std::string const&, bool enable) noexcept;
		bool update_mail_credential(SMTP::server const&, bool enable) noexcept;

		bool update_general_notify(User::User&, Notify::general_type) noexcept;
		bool update_device_notify(User::User& user,
				Device::device_id id,
				Notify::device_type type) noexcept;
		bool update_sensor_notify(User::User& user,
				Device::device_id,
				std::vector<Notify::sensor_temp_notify> const&) noexcept;

		/**
		 *
		 */
		bool process_device_request(engine::message const&,
				Device::Device**,
				CoAP::Message::Option::option&) noexcept;
		bool update_db_device(Device::Device const&) noexcept;
		Device::Net* get_or_add_net(mesh_addr_t const&) noexcept;

		/**
		 *
		 */
		bool has_request_in_progress(mesh_addr_t const&,
				CoAP::Message::code,
				Device::Request::request_type) const noexcept;
		bool add_request_in_progress(mesh_addr_t const&,
				CoAP::Message::code,
				Device::Request::request_type,
				User::user_id) noexcept;
		bool remove_request_in_progress(mesh_addr_t const&,
				CoAP::Message::code,
				Device::Request::request_type) noexcept;

		/**
		 * Sensor
		 */
		bool remove_sensor_type(std::string const&) noexcept;
		bool add_sensor_type(Sensor::sensor_description const&) noexcept;
		bool update_sensor_type(Sensor::sensor_description const&) noexcept;

		std::size_t update_sensor_value(Device::Device&, const void*, std::size_t) noexcept;
		bool update_sensor_value(Device::Device&, Sensor::sensor_type const&) noexcept;

		std::string get_sensor_value(Device::device_id,
				unsigned type, unsigned index,
				value_time init, value_time end) noexcept;

		/**
		 * Image
		 */
		Image_Path const& image_path() const noexcept;
		bool add_image(std::string const& image_name,
				User::user_id /* uploader */,
				std::string const& description = "") noexcept;
		void delete_image(std::vector<std::string> const&) noexcept;
		bool read_image(std::string const&, Image&) noexcept;
		bool update_image(std::string const& image_name,
						std::string const& description) noexcept;
		void send_all_image_list() noexcept;

		/**
		 * App
		 */
		App_Path const& app_path() const noexcept;
		bool add_app(std::string const& app_name,
				User::user_id /* uploader */,
				std::string const& description = "") noexcept;
		void delete_app(std::vector<std::string> const&) noexcept;
		bool read_app(std::string const&, App&) noexcept;
		bool update_app(std::string const& app_name,
						std::string const& description) noexcept;
		void send_all_app_list() noexcept;

		/**
		 * Tree
		 */
		bool update_tree(Device::Device const&) noexcept;
		bool remove_node_from_tree(mesh_addr_t const&) noexcept;
		void check_device_state() noexcept;

		/**
		 * Report
		 */
		bool read_all_reports(std::vector<Message::report>&, User::user_id, int limit = 0) noexcept;
		std::shared_ptr<std::string> make_report(Message::report_type,
				mesh_addr_t const&,
				std::string const& /* message */,
				std::string const& /* arg */,
				User::user_id) noexcept;
		std::shared_ptr<std::string> make_report(
				Message::report_commands command,
				Message::report_type type,
				std::string const& ref,
				std::string const& message,
				std::string const& arg,
				User::user_id id) noexcept;
		/**
		 *
		 */
		Device::Device_List const& device_list() const noexcept;
		Device::Device_List& device_list() noexcept;

		engine const& coap_engine() const noexcept;
		engine& coap_engine() noexcept;

		User::User_List const& users() const noexcept;

		Device::Tree& tree() noexcept;

		Notify::Factory const& notify() const noexcept;

		Sensor::Sensor_Type_List const& sensor_list() const noexcept;

		bool authenticate(User::Logged&,
				rapidjson::Document const&,
				::Message::user_commands&,
				std::error_code&) noexcept;

		bool create_session_id(User::Logged& user,
				rapidjson::Document const& doc,
				std::error_code& ec) noexcept;

		share_ptr share() noexcept;
	private:
		void initiate_check_roots() noexcept;
		void check_network_roots() noexcept;
		bool check_root(mesh_addr_t const& addr) noexcept;

		Image_Path			image_;
		App_Path			app_;

		boost::asio::io_context& ioc_;
		DB					db_;
		engine				coap_engine_;
		Notify::Factory		notify_;

		share_ptr			share_;

		Device::Device_List	device_list_;
		Device::Net_List	net_list_;
		Device::Tree		tree_{device_list_};
		std::vector<mesh_addr_t> unconnected_;

		Sensor::Sensor_Type_List	sensor_list_;

		User::User_List		users_;

		User::User root_{User::root_id,
							User::Info{
							User::root_username,
							User::root_name,
							User::Info::status::active,
							"" /* email */,
							"" /* telegram_chat_id */}};

		Device::Request::Request_in_Pogress_List requests_;

		std::vector<engine::resource_node> vresource_;

		boost::asio::steady_timer check_root_timer_{ioc_};
#if USE_SSL == 1
		boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12};
#endif /* USE_SSL == 1 */
};

}//Agro

#include "impl/agro_impl.hpp"

#endif /* AGRO_DAEMON_AGRO_HPP__ */
