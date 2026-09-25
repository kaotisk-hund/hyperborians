peerStats(1) -- show hyperboria peers
=============================================

## SYNOPSIS

`peerStats`

## DESCRIPTION

Hyperboria-route talks to a number of immediate peers configured in
`/etc/hyperboria-route.conf`.  These can be discovered dynamically on local
networks if "beacon" is enabled.  Peers are enabled to connect
to hyperboria-route by adding a login in "authorizedPasswords".  Connecting
out to peers is configured in "connectTo" in either the IPv4 or IPv6 section.

No admin privilege is needed to run peerStats, but it looks in `~/.hyperboriaadmin`
for the IP and admin port of hyperboria-route.

## FILES

`~/.hyperboriaadmin`

## SEE ALSO

hyperboria-route(1), sessionStats(1)
