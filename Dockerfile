# syntax=docker/dockerfile:1
FROM cpp_conan_devel:latest as build
WORKDIR /app
ARG DAEMON_BRANCH=
ARG BUILD_TYPE=
ARG WITH_SSL=
RUN set -eux; \
    git clone -b ${DAEMON_BRANCH:-devel} https://github.com/rnascunha/agro_daemon && \
    cd agro_daemon && \
    conan install . -s build_type=${BUILD_TYPE:-Release} -if ./build -pr default -b missing && \
    cmake -G "Unix Makefiles" -DWITH_SSL=${WITH_SSL:-0} -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release} -B ./build && \
    cmake --build build/ && \
    cmake --install build/ --prefix build/install_dir
CMD [ "/bin/bash" ]

FROM ubuntu:latest
COPY --from=build /app/agro_daemon/build/install_dir/* /agro
ENV PATH=${PATH}:/agro
WORKDIR /app
ENTRYPOINT [ "/agro/agro_daemon" ]
CMD [ "8080" ]
