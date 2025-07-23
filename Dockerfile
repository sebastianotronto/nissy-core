# This Dockerfile can be use to build a container to test nissy in different
# platforms. For example you can build the image with:
#
# docker build localhost/nissy-dev/arm .
#
# And then run unit tests in an arm64 container with
#
# docker run --platform=linux/arm64 -t localhost/nissy-dev/arm ./build test

# TODO: add C++ compiler, emscripten

FROM alpine:3.22.1
RUN apk update && apk add \
	gcc \
	clang \
	python3 \
	python3-dev

COPY . ./
