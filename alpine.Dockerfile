# syntax=docker/dockerfile:1
FROM rnascunha/cpp_conan:v1.0-alpine as build
WORKDIR /app
ARG DAEMON_BRANCH=
ARG BUILD_TYPE=
ARG WITH_SSL=
RUN set -eux; \
    git clone -b ${DAEMON_BRANCH:-devel} https://github.com/rnascunha/agro_daemon && \
    cd agro_daemon && \
    conan install . -s build_type=${BUILD_TYPE:-Release} -if ./build -pr default -b missing -b openssl && \
    cmake -G "Unix Makefiles" -DWITH_SSL=${WITH_SSL:-0} -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release} -B ./build && \
    cmake --build build/ && \
    cmake --install build/ --prefix build/install_dir

FROM alpine:latest
RUN apk add --update --no-cache libstdc++
COPY --from=build /app/agro_daemon/build/install_dir/* /agro
ENV PATH=${PATH}:/agro
WORKDIR /app
ENTRYPOINT [ "/agro/agro_daemon" ]
CMD [ "8081" ]