#!/bin/sh

# A simple script to run commands (e.g. build commands) in a Docker container.
# It can be used for testing on platforms different from the host. For example,
# the command
# 
# ./container-run.sh arm ./build test
#
# builds nissy and runs the unit tests in an ARM container.
# The container images are based on Debian Linux, see Dockerfile for details.
# The images are built with a tag starting with 'localhost/nissy/'.
# See below for a list of options.

usage() {
	echo "$1"
	echo ""
	echo "usage: $0 PLATFORM [COMMAND]"
	echo ""
	echo "If COMMAND is unspecified, an interactive shell will be opened."
	echo ""
	echo "Available platforms:"
	echo "x86   (equivalent to: x86_64, amd64)"
	echo "arm   (equivalent to: arm64)"
	echo ""
	echo "Examples:"
	echo "$0 ram ./build test   # Run unit tests in arm container"
	exit 1
}

if [ -z "$1" ]; then
	usage "No platform given."
fi

case "$1" in
x86|x86_64|amd64)
	p=amd64
	shift
	;;
arm|arm64)
	p=arm64
	shift
	;;
*)
	usage "Platform '$1' not available."
	;;
esac

platform="--platform=linux/$p"
image="localhost/nissy/debian-$p"

docker build "$platform" -t "$image" .

if [ -z "$2" ]; then 
	docker run --rm "$platform" -it "$image" /bin/sh
else
	docker run --rm "$platform" -t "$image" $@
fi
