FROM ubuntu:17.10

RUN apt-get update && apt-get install -y cmake clang++-5.0 make qtbase5-dev

ENV SRC_DIR /root/tp_http_server/

ADD . $SRC_DIR

RUN mkdir $SRC_DIR/build

WORKDIR $SRC_DIR/build

ENV PATH $PATH:$SRC_DIR/build

RUN cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-5.0 \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CXX_FLAGS="-O3" \
          -DCMAKE_MAKE_PROGRAM=/usr/bin/make \
          -G "CodeBlocks - Unix Makefiles" \
          ..

RUN cmake --build $SRC_DIR/build --target all -- -j 2

RUN mkdir -p /var/www/html

EXPOSE 80

CMD tp_http_server
