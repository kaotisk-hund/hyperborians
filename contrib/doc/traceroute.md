hyperboria-traceroute(1) -- trace hyperboria packet routing
=============================================

## SYNOPSIS

`hyperboria-traceroute` <host_or_ip>

## DESCRIPTION

Because hyperboria is end to end encrypted, the standard traceroute always
shows a direct connection.  Hyperboria-traceroute queries hyperboria-route to discover
what route would be used to send a packet to the destination IP.
Note that just as with the standard traceroute, there is no guarantee
that that precise route will actually be used with the next packet.

Hyperboria-traceroute requires admin privilege.  The cjdnsadmin lib will 
try to read `/etc/hyperboria-route.conf` for the admin password.  This will 
normally succeed only for root.  Otherwise, it tries `~/.hyperboriaadmin`

## USAGE

## FILES

`~/.hyperboriaadmin`
`/etc/hyperboria-route.conf`

## BUGS
Hyperboria-traceroute throws an ugly exception if you forget
to pass an IP or have the wrong admin password.

## SEE ALSO

hyperboria-route(1)
