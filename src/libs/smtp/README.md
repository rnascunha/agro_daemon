#SMTP Client

Simple library to send SMTP packet with string as payload, over a relay server.

## Depends

Depends: 
* Boost.Asio;
* OpenSSL.

To compile:
* CMake;
* C++ 17 compiler.

## Compile

At root directory, create a build folder:

```
$ mkdir build
$ cd build
```
To compile:

```
$ cmake -DWITH_EXAMPLES=1 ..
$ cmake --build .
```
This will output the library file (`libsmtp.a` at Linux). If you don't want to compile the examples, just remove `-DWITH_EXAMPLES=1` from the command above. You can also use `-DDEBUG=1` to enable debug output from the library.

## Using the library

Link to the library and follow the example `examples/smtp_client.cpp` steps. First, include the header:

```
#include "smtp/client.hpp"
```
And then instantiate the class:

```C++
	using SMTP_Client = SMTP::Client<false /* Use Pipeline */, 10 /* timeout in seconds */>;

    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23};

    ctx.set_default_verify_paths();
    ctx.set_verify_mode(ctx.verify_fail_if_no_peer_cert);

    auto client = std::make_shared<SMTP_Client>(ioc, ctx);
```
At the first line, the first template parameter sets if it will be used or not pipeline.

>	*Pipeline* is when you send all data request at once, without waiting the server response, saving processing/network bandwidth. At my tests, *pipeline* didn't work at **Gmail** (even it says it accepts), but worked at **Yahoo**.

The second parameter is the timeout, in seconds (defaulted to 10 seconds). If the time expires, all operation is cancelled.

After that, supply the *address*, *port*, *user* and *password* of the SMTP relay server you are using:

```C++
    /**
     * Use the server and port of your SMTP server
     */
    //If you use Yahoo
	//server.server   = "smtp.mail.yahoo.com";
	//server.port     = "465";

    //If you use Gmail
    server.server   = "smtp.gmail.com";
    server.port     = "465";

    //Your credentions
    server.user     = "<your_email@company.com>";
    server.password = "<your_password>";

	/** Setting server data */
    client->set_mail_server(server);
```
And the email parameters: *from*, *to* (email and name, can be a list of receivers), *subject* and *body* of the email.

```C++
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

	/** Setting mail data */
    client->set_mail_data(data);
```
Finally, send the SMTP packet:

```
	client->connect();

    boost::system::error_code ec;
    ioc.run(ec);
```

## Tests

This was tested with `gmail.com` and `yahoo.com` emails successfuly. `gmail` doesn't support pipeline (even announcing that it support at `ehlo` command). Both need to create a app `password`, see links below:

* [Yahoo](https://help.yahoo.com/kb/learn-generate-third-party-passwords-sln15241.html)
* [GMail](https://support.google.com/accounts/answer/3466521);