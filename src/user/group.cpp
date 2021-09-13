#include "group.hpp"
#include <algorithm>

namespace Agro{
namespace User{

Group::Group(group_id id,
		std::string const& name,
		std::string description /* = "" */)
	: id_(id), name_(name), description_(description){}

group_id Group::id() const noexcept
{
	return id_;
}

std::string const& Group::name() const noexcept
{
	return name_;
}

std::string const& Group::description() const noexcept
{
	return description_;
}

bool Group::add(user_id id) noexcept
{
	if(std::find(users_.begin(), users_.end(), id) != users_.end())
	{
		return false;
	}

	users_.push_back(id);
	return true;
}

bool Group::remove(user_id id) noexcept
{
	int i = 0;
	for(auto& u : users_)
	{
		if(u == id)
		{
			users_.erase(users_.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}

bool Group::contains(user_id id) const noexcept
{
	return std::find(users_.begin(), users_.end(), id) != users_.end();
}

bool Group::operator==(Group const& rhs) const noexcept
{
	return id_ == rhs.id();
}

std::size_t Group::size() const noexcept
{
	return users_.size();
}

/**
 *
 */
bool Groups::add(Group&& group) noexcept
{
	for(auto& g : groups_)
	{
		if(g.id() == group.id() || g.name() == group.name())
			return false;
	}
	groups_.push_back(group);

	return true;
}

bool Groups::remove(group_id id) noexcept
{
	int i = 0;
	for(auto& u : groups_)
	{
		if(u.id() == id)
		{
			groups_.erase(groups_.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}

bool Groups::add_user(group_id gid, user_id uid) noexcept
{
	int i = 0;
	for(auto& g : groups_)
	{
		if(g.id() == gid)
		{
			return g.add(uid);
		}
		i++;
	}
	return false;
}

bool Groups::contains(group_id id) const noexcept
{
	for(auto& g : groups_)
	{
		if(g.id() == id) return true;
	}
	return false;
}

bool Groups::contains(std::string const& name) const noexcept
{
	for(auto& g : groups_)
	{
		if(g.name() == name) return true;
	}
	return false;
}

bool Groups::contains_user(group_id gid, user_id uid) const noexcept
{
	Group const* g = get(gid);
	if(!g) return false;

	return g->contains(uid);
}

Group const* Groups::get(group_id id) const noexcept
{
	for(auto const& g : groups_)
	{
		if(g.id() == id)
		{
			return &g;
		}
	}
	return nullptr;
}

Group* Groups::get(group_id id) noexcept
{
	for(auto& g : groups_)
	{
		if(g.id() == id)
		{
			return &g;
		}
	}
	return nullptr;
}

std::size_t Groups::size() const noexcept
{
	return groups_.size();
}

}//User
}//Agro
