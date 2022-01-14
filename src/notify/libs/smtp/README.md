#SMTP Client

## Depends

Depends on Boost.asio and C++17.

## Compile and execute example

Put your credentials at file `smtp_client.cpp` at `main` function.

```
$ g++ smtp_client.cpp smtp/client.cpp -o smtp_client -L<path/to/boost> -lcrypto -lssl -pthread -std=gnu++17 -DDEBUG=<0|1>
```

To execute:

```
$ ./smtp_client
```

## Tests

This was tested with `gmail.com` and `yahoo.com` emails successfuly. `gmail` doesn't support pipeline (even announcing that it support at `ehlo` command). Both need to create a app `password`, see links below:

* [Yahoo](https://help.yahoo.com/kb/learn-generate-third-party-passwords-sln15241.html)
* [GMail](https://support.google.com/accounts/answer/3466521);