FROM base/archlinux

RUN pacman -Sy cmake clang qt5-base --noconfirm
RUN pacman -Sy make --noconfirm
ENV SRC_DIR /root/tp_http_server/

ADD . $SRC_DIR

RUN mkdir $SRC_DIR/build

WORKDIR $SRC_DIR/build

ENV PATH $PATH:$SRC_DIR/build

RUN cmake -DCMAKE_C_COMPILER=/usr/bin/clang \
          -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CXX_FLAGS="-O3" \
          -DCMAKE_MAKE_PROGRAM=/usr/bin/make \
          -G "CodeBlocks - Unix Makefiles" \
          ..

RUN cmake --build $SRC_DIR/build --target all -- -j 2

RUN mkdir -p /var/www/html

EXPOSE 80

CMD tp_http_server
