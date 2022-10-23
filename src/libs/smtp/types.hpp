#ifndef SMTP_CLIENT_TYPES_HPP__
#define SMTP_CLIENT_TYPES_HPP__

#include <string>

namespace SMTP{

struct server
{
    server() = default;
    server(const server&) = default;
    server(server&&) = default;
    server& operator=(const server&) = default;

    std::string addr, port, user, password;
    
};

}//SMTP

#endif /* SMTP_CLIENT_TYPES_HPP__ */
