FROM alpine:3.6
MAINTAINER Lars Gierth <larsg@systemli.org>

RUN apk add --update nodejs bash python git build-base linux-headers

ADD . /src

WORKDIR /src

RUN adduser -D -h /etc/hyperboria -u 1000 hyperboria \
    && rm -rf build_* && ./do \
    && cp hyperboria-route /usr/bin \
    && cp -r tools/* /usr/bin \
    && cp makekeys \
          mkpasswd \
          privatetopublic \
          publictoip6 \
          randombytes \
          sybilsim /usr/bin \
    && cp contrib/docker/entrypoint.sh / \
    && rm -rf /src /var/cache/apk/* \
    && apk del --purge python build-base linux-headers

VOLUME [ "/etc/hyperboria" ]

ENTRYPOINT [ "/entrypoint.sh" ]
