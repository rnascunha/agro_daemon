#include "smtp/client.hpp"
#include <memory>
#include <iostream>

int main()
{
	using SMTP_Client = SMTP::Client<false /* Use Pipeline */, 10 /* timeout */>;

    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23};

    ctx.set_default_verify_paths();
    ctx.set_verify_mode(ctx.verify_fail_if_no_peer_cert);

    auto client = std::make_shared<SMTP_Client>(ioc, ctx);

    SMTP::data data;
    SMTP::server server;

    /**
     * Use the server and port of your SMTP server
     */
    //If you use Yahoo
//    server.server   = "smtp.mail.yahoo.com";
//    server.port     = "465";

    //If you use Gmail
    server.server   = "smtp.gmail.com";
    server.port     = "465";

    //Your credentions
    server.user     = "<your_email@company.com>";
    server.password = "<your_password>";

    make_mail_data(data,
    		//From
    		{"<your_email>@company.com", "<Your (any) name>"},
			//To, array of who will be sent
    		{{"<some_email>@gmail.com", "<Some Name>"}, {"<other_email>@yahoo.com"}},
			//Subject
			"Subject of the email",
			//Body message
			"Body of the email"
			);

    client->set_mail_server(server);
    client->set_mail_data(data);

    client->connect();

    boost::system::error_code ec;
    ioc.run(ec);

    if (ec)
    {
        std::cout << "Error: [" << ec.value() << "] " << ec.message() << "\n";
    }
    return 0;
}
