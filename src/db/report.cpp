#include "db.hpp"
#include "../helper/time_helper.hpp"

namespace Agro{

int DB::read_all_reports(std::vector<Message::report>& reports, User::user_id id, int limit /* = 0 */) noexcept
{
	sqlite3::statement res;
	int rc;
	if(limit <= 0)
	{
		rc = db_.prepare_bind("SELECT command,type,reference,message,argument,time) "
					"FROM report "
					"WHERE userid = ? OR userid = ?", res, id, Message::report_all_user);
	}
	else
	{
		rc = db_.prepare_bind("SELECT COUNT(*) FROM report WHERE userid = ?", res, id);
		if(rc != SQLITE_OK)
		{
			return rc;
		}

		res.step();
		int size = res.interger(0),
			offset = size > limit ? size - limit : 0;
		res.finalize();

		rc = db_.prepare_bind("SELECT command,type,reference,message,argument,time "
							"FROM report "
							"WHERE userid = ? OR userid = ?"
							"ORDER BY time "
							"LIMIT ? OFFSET ?", res, id, Message::report_all_user, limit, offset);
	}

	if(rc != SQLITE_OK)
	{
		return rc;
	}

	while(res.step() == SQLITE_ROW)
	{
		reports.emplace_back(static_cast<Message::report_commands>(res.interger(0)),
						static_cast<Message::report_type>(res.interger(1)),
						res.text(2),
						res.text(3),
						res.text(4),
						res.long_interger(5));
	}

	return SQLITE_DONE;
}

int DB::add_report(Message::report_commands command,
				Message::report_type type,
				std::string const& reference,
				std::string const& message,
				std::string const& arg,
				User::user_id id) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO report(userid,command,type,reference,message,argument,time) VALUES(?,?,?,?,?,?,?)",
			res,
			id,
			static_cast<int>(command),
			static_cast<int>(type),
			reference,
			message,
			arg,
			time_epoch_miliseconds());
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

}//Agro
