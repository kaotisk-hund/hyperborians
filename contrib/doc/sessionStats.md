sessionStats(1) -- show hyperboria crypto sessions
=============================================

## SYNOPSIS

`sessionStats`

## DESCRIPTION

Hyperboria-route always talks to other nodes over a "session" similar to
a TLS session.  A session is required not only to talk to each peer,
but also when talking to intermediate nodes to ask them about their peers
when planning a route.  

The hyperboria protocol offers Perfect Forward Secrecy.  Each session has a random
symmetric key that encrypts that session.  If a node is compromised, 
the attacker can get the session keys and read current and future sessions.
However, the session keys are securely erased when a session is destroyed,
so that past sessions (before the compromise) cannot be read.  The lifetime
of a session is somewhat random.  Hyperboria-route caches recently active sessions,
and destroys sessions that have not been used for a while.

## USAGE

Running sessionStats requires admin privilege.  The cjdnsadmin lib will 
try to read `/etc/hyperboria-route.conf` for the admin password.  This will 
normally succeed only for root.  Otherwise, it tries `~/.hyperboriaadmin`

## FILES

`~/.hyperboriaadmin`
`/etc/hyperboria-route.conf`

## BUGS
If you get a TypeError, you probably have the wrong admin password.

## SEE ALSO

hyperboria-route(1), peerStats(1)
