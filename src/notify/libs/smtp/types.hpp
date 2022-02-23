#ifndef SMTP_CLIENT_TYPES_HPP__
#define SMTP_CLIENT_TYPES_HPP__

#include <string>

namespace SMTP{

struct server
{
    std::string server, port, user, password;
};

}//SMTP

#endif /* SMTP_CLIENT_TYPES_HPP__ */
