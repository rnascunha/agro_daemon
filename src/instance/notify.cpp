#include "agro.hpp"
#include "../sensor/helper.hpp"

namespace Agro{

bool instance::push_notify_is_valid() const noexcept
{
	return notify_.push_is_valid();
}

std::string_view const& instance::get_notify_public_key() const noexcept
{
	return notify_.public_key();
}

void instance::push_subscribe_user(User::user_id id,
			std::string const& user_agent,
			std::string const& endpoint,
			std::string const& p256dh,
			std::string const& auth) noexcept
{
	auto* user = users_[id];
	if(!user) return;

	user->subscriptions().add_or_update(user_agent, endpoint, p256dh, auth);
	db_.push_subscribe_user(id, user_agent, endpoint, p256dh, auth);
}

void instance::push_unsubscribe_user(User::user_id id,
			std::string const& user_agent) noexcept
{
	auto* user = users_[id];
	if(!user) return;

	user->subscriptions().remove(user_agent);
	db_.push_unsubscribe_user(id, user_agent);
}

void instance::notify_all(std::string const& data) noexcept
{
	for(auto const& [uid, u] : users_)
	{
		notify_.notify(u, data);
	}
}

void instance::notify_all_policy(Authorization::rule rule, std::string const& data) noexcept
{
	for(auto const& [uid, u] : users_)
	{
		if(Authorization::can(u, rule))
		{
			notify_.notify(u, data);
		}
	}
}

void instance::notify_all_policy(Authorization::rule rule,
		Notify::general_type type,
		std::string const& data) noexcept
{
	for(auto const& [uid, u] : users_)
	{
		if(Authorization::can(u, rule) && u.notify().can(type))
		{
			notify_.notify(u, data);
		}
	}
}

void instance::notify_all_policy(Authorization::rule rule,
		Notify::device_type type,
		std::vector<std::reference_wrapper<Device::Device const>> const& list,
		const char* status) noexcept
{
	for(auto& [uid, u] : users_)
	{
		if(Authorization::can(u, rule))
		{
			std::vector<std::reference_wrapper<Device::Device const>> ll;
			for(auto const& d : list)
			{
				if(u.notify().can(d.get().id(), type))
				{
					ll.push_back(d);
				}
			}
			if(ll.size())
			{
				notify_.notify(u, Notify::Message::make_status_devices(ll, status));
			}
		}
	}
}

void instance::notify_all_policy(Authorization::rule rule,
		Device::Device const& device,
		Sensor::sensor_type const& type,
		Sensor::sensor_description const* sdesc) noexcept
{
	for(auto& [uid, u] : users_)
	{
		if(Authorization::can(u, rule))
		{
			float value = Sensor::get_sensor_value(sdesc, type.value);
			auto vsn = u.notify().can(
					device.id(),
					type.type,
					type.index,
					value);

			for(auto const& s : vsn)
			{
				notify_.notify(u, Notify::Message::make_notify_sensor_data(device, type, sdesc, s, value));
			}
		}
	}
}

bool instance::update_general_notify(User::User& user, Notify::general_type type) noexcept
{
	if(db_.update_general_notify(user.id(), type) != SQLITE_DONE)
	{
		tt::error("Error updating general notify of user %d", user.id());
		return false;
	}

	user.notify().set(type);
	return true;
}

bool instance::update_device_notify(User::User& user,
		Device::device_id id,
		Notify::device_type type) noexcept
{
	if(db_.update_device_notify(user.id(), id, type) != SQLITE_DONE)
	{
		tt::error("Error updating device notify of user %d", user.id());
		return false;
	}

	user.notify().set(id, type);
	return true;
}

bool instance::update_sensor_notify(User::User& user,
				Device::device_id dev_id,
				std::vector<Notify::sensor_temp_notify> const& sensors) noexcept
{
	int rc = db_.update_sensor_notify(user.id(), dev_id, sensors);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error updating sensor notify list [uid=%d/did=%d/err=%d/%s]", user.id(), dev_id, rc, db_.error());
		return false;
	}

	user.notify().set(dev_id, sensors);
	return true;
}

}//Agro
