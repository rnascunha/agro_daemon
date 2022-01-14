#include "agro.hpp"
#include "tt/tt.hpp"

namespace Agro{

bool instance::check_user_session_id(
				User::user_id id,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) const noexcept
{
	auto const* user = users_[id];
	if(!user) return false;

	return user->sessions()
			.check_user_session_id(
					session_id,
					user_agent,
					session_time);
}

User::User* instance::get_user(std::string const& username) noexcept
{
	if(username == User::root_username) return &root_;

	auto* user = users_[username];
	if(!user) return nullptr;

	return user;
}

User::User const* instance::get_user(User::user_id id) const noexcept
{
	if(id == User::root_id) return &root_;

	auto const* user = users_[id];
	if(!user) return nullptr;

	return user;
}

bool instance::get_user_password(std::string const& username,
						std::vector<unsigned char>& salt,
						std::vector<unsigned char>& pass) noexcept
{
	return db_.get_password(username, salt, pass);
}

bool instance::add_user(std::string const& username,
						std::string const& name,
						std::string const& password,
						std::string const& email,
						std::string const& telegram_chat_id,
						User::user_id& id) noexcept
{
	id = User::invalid_id;

	User::key_password key;
	User::salt_password salt;
	if(!User::create_password(password, salt, key))
	{
		tt::error("Error hashing password of '%s'!", username.c_str());
		return false;
	}

	if(db_.add_user(username, key, salt, name, email, telegram_chat_id, id) != SQLITE_DONE)
	{
		tt::error("Error adding user '%s' to database!", username.c_str());
		return false;
	}

	if(!users_.add(User::User{id, User::Info{username, name, User::Info::status::active, email, telegram_chat_id}}))
	{
		tt::error("Error adding user '%s'!", username.c_str());
		return false;
	}

	return true;
}

bool instance::add_user_to_group(User::user_id uid, User::group_id gid) noexcept
{
	if(db_.add_user_to_group(uid, gid) != SQLITE_DONE)
	{
		tt::error("Error adding user '%d' to group '%d' at database", uid, gid);
		return false;
	}

	if(!users_.add_user_to_group(uid, gid))
	{
		tt::error("Error adding user '%d' to group '%d'", uid, gid);
		return false;
	}

	return true;
}

void instance::set_user_to_groups(User::user_id uid, std::vector<User::group_id> const& gid_list) noexcept
{
	db_.set_user_to_groups(uid, gid_list);
	users_.add_user_to_groups(uid, gid_list);
}

bool instance::edit_user(User::user_id id,
				std::string const& username,
				std::string const& name,
				std::string const& email,
				std::string const& telegram_chat_id,
				std::vector<User::group_id> const& groups) noexcept
{
	if(db_.edit_user(id, username, name, email, telegram_chat_id, groups) != SQLITE_DONE)
	{
		tt::error("Error editing user '%d' at database", id);
		return false;
	}

	if(!users_.edit_user(id, username, name, email, telegram_chat_id, groups))
	{
		tt::error("Error editing user '%d'", id);
		return false;
	}

	return true;
}

bool instance::delete_user(User::user_id id) noexcept
{
	if(db_.delete_user(id) != SQLITE_DONE)
	{
		tt::error("Error deleteing user '%d' from database", id);
		return false;
	}

	if(!users_.remove(id))
	{
		tt::error("Error deleteing user '%d'", id);
		return false;
	}

	if(db_.delete_user_from_group(id) != SQLITE_DONE)
	{
		tt::error("Error deleteing user '%d' from database groups", id);
		return false;
	}

	users_.remove_user_from_all_groups(id);

	return true;
}

User::Group const* instance::get_group_info(User::group_id id) const noexcept
{
	return users_.groups().get(id);
}

bool instance::add_group(std::string const& name,
				std::string const& description,
				std::vector<User::user_id> const& members,
				User::group_id& id) noexcept
{
	int rc = db_.add_user_group(name, description, id);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error adding group '%s' to database. [err=%d]", name.c_str(), rc);
		return false;
	}

	if(!users_.add_group(User::Group{id, name, description}))
	{
		tt::error("Error adding group '%s'", name.c_str());
		return false;
	}

	db_.add_users_to_group(id, members);
	users_.add_user_to_groups(id, members);

	return true;
}

bool instance::delete_group(User::group_id id) noexcept
{
	int rc = db_.delete_group(id);
	if(db_.delete_group(id) != SQLITE_DONE)
	{
		tt::error("Error deleteing group '%d' from database. [err=%d]", id, rc);
		return false;
	}

	if(!users_.remove_group(id))
	{
		tt::error("Error deleteing group '%d'", id);
		return false;
	}

	return true;
}

bool instance::update_user_session_id(User::user_id id,
				std::string const& session_id,
				std::string const& user_agent) noexcept
{
	auto* user = users_[id];
	if(!user) return false;

	user->sessions().add_or_update(user_agent, session_id);
	return db_.update_user_session_id(id, session_id, user_agent);
}

void instance::clear_session_user_agent(User::user_id id,
			std::string const& user_agent) noexcept
{
	auto* user = users_[id];
	if(!user) return;

	user->subscriptions().clear_subscription(user_agent);
	db_.clear_session_user_agent(id, user_agent);
}

}//Agro
