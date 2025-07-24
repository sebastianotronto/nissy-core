# This Dockerfile can be use to build a container to test nissy in different
# platforms. For example you can build the image with:
#
# docker build localhost/nissy-dev/arm .
#
# And then run unit tests in an arm64 container with
#
# docker run --platform=linux/arm64 -t localhost/nissy-dev/arm ./build test

FROM debian:12

# Install necessary packages
RUN apt update
RUN apt install --yes xz-utils git gcc g++ clang python3 python3-dev

# Install emscripten.
RUN git clone https://github.com/emscripten-core/emsdk.git
WORKDIR /emsdk
RUN ./emsdk install 4.0.11
RUN ./emsdk activate 4.0.11
ENV EMSDK="/emsdk"
ENV EMSDK_NODE_BIN="$EMSDK/node/22.16.0_64bit/bin"
ENV EMSDK_NODE="$EMSDK_NODE_BIN/node"
ENV PATH="$EMSDK:$EMSDK/upstream/emscripten:$EMSDK_NODE_BIN:$PATH"

# Copying the nissy-core repository and switching to that folder.
WORKDIR /
COPY . ./nissy-core
WORKDIR /nissy-core
