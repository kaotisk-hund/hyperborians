#!/usr/bin/env bash

set -e

CONF_DIR="/etc/hyperboria"

if [ ! -f "$CONF_DIR/hyperboria-route.conf" ]; then
  echo "generate $CONF_DIR/hyperboria-route.conf"
  conf=$(hyperboria-route --genconf | hyperboria-route --cleanconf)
  echo $conf > "$CONF_DIR/hyperboria-route.conf"
fi

hyperboria-route --nobg < "$CONF_DIR/hyperboria-route.conf"
exit $?
