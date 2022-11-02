[![Agro Build](https://github.com/rnascunha/agro_daemon/actions/workflows/main.yml/badge.svg)](https://github.com/rnascunha/agro_daemon/actions/workflows/main.yml)

# Agro Daemon

Daemon that will receive data from devices.

## Building from source

### Dependencies

* git - clone repository;
* conan - package manager to deal with dependencies;
* cmake - Configure and build;
* C++17 compiler - compile code (GCC/Clang/MSVC);

### Building 

```bash
# Clone repository
$ git clone https://github.com/rnascunha/agro_daemon
$ cd agro_daemon

# Create building directory and enter
$ mkdir build && cd build

# Download dependencies
$ conan install ..

# Configure and build
$ cmake ..
$ cmake --build .
```

If you want to install at `some folder`:
```bash
$ cmake --install . --prefix <some_folder>
```

## Running application

First, initiate database:
```bash
$ ./init_db
```

Provide all information requested. Now execute daemon at `some port`:
```bash
$ ./agro_daemon <some_port>
```

To see all options:
```
$ ./agro_daemon -h
```

### Using Docker

To build the container:
```bash
# Clone repository
$ git clone https://github.com/rnascunha/agro_daemon
$ cd agro_daemon

# Build cotainer
$ docker build -t agro_daemon .
```

For the first use, you must create the database:
```
$ docker run --rm -it --entrypoint init_db -v $PWD:/app agro_daemon
```

This will output a `agro.db` file at your local directory. Now, run the `agro_daemon` application:
```
$ docker run --rm -v $PWD:/app -it --network host agro_daemon
```

You can also run from the image from the repository:
```bash
# Configure DB
$ docker run --rm -it --entrypoint init_db -v $PWD:/app rnascunha/agro_daemon:latest
# Run application
$ docker run --rm -v $PWD:/app -it --network host rnascunha/agro_daemon:latest
```

### Using Docker compose

If you want to run the **daemon** and the [web interface](https://github.com/rnascunha/agro_web) at the same server, you can use the docker compose option.

First, create the database at a named volume `agro_db`:
```bash
$ docker run --rm -it --entrypoint init_db -v agro_db:/app rnascunha/agro_daemon:v1.0-alpine
```

Then, at the root directory of the project:
```bash
$ docker compose up
```

This will run the interface at port `8080`, CoAP port at `5683` and Websocket at `8081`.