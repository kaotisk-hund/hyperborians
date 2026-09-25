#!/bin/sh
# A script to start hyperboria-route with its config as an argument
# Maintainer: Jack L. Frost <fbt@fleshless.org>

# Functions
echo() { printf '%s\n' "$*"; }
usage() { echo "Usage: run-hyperboria-route [/path/to/hyperboria-route.conf] (/etc/hyperboria-route.conf by default)"; }

err() { echo "$1" >&2; }

main() {
	[ "$1" = '-h' -o "$1" = '--help' ] && { usage; return 0; }

	cjdroute_config=${1:-"/etc/hyperboria-route.conf"}
	exec hyperboria-route < $cjdroute_config
}

# DO SOMETHING
main "$@"
