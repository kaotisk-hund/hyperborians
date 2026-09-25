hyperboria-route(1) -- Hyperboria packet switch
=============================================

## SYNOPSIS

`/usr/sbin/hyperboria-route` [<options>...]

## DESCRIPTION

Hyperboria implements an encrypted IPv6 network using public-key cryptography for
address allocation and a distributed hash table for routing. This provides
near-zero-configuration networking, and prevents many of the security and
scalability issues that plague existing networks.

hyperboria-route runs in the background and either decrypts packets addressed to
this node, or sends them on to the next node in the route.

## OPTIONS

  * `--help`:
    Print usage summary.

  * `--genconf` [--no-eth]:
    Generate a configuration file, write it to stdout.
    If --no-eth is specified then ethernet beaconing will be disabled.

  * `--bench`:
    Run some cryptography performance benchmarks.

  * `--version`:             
    Print hyperboria-route version and the protocol version which this node speaks.

  * `--cleanconf` < conf:
    Print a clean (valid json) version of the config.

  * `--nobg`                
    Never fork to the background no matter the config.

## USAGE

To get the router up and running:

  * Step 1:
    Generate a new configuration file if one doesn't already exist.

      hyperboria-route --genconf > /etc/hyperboria-route.conf

    You can also simply:

      systemctl start hyperboria

    which will tell hyperboria-route to generate a new config if needed.

  * Step 2:
    Find somebody to connect to.
    Check out the IRC channel or https://hyperboria.net/
    for information about how to meet new people and make connect to them.
    Read more here: https://github.com/cjdelisle/hyperboria/#2-find-a-friend

    By default, hyperboria-route will find any hyperboria nodes on your local LAN
    without any configuration.

  * Step 3:
    Add that somebody's node to your hyperboria-route.conf file.
    https://github.com/cjdelisle/hyperboria/#3-connect-your-node-to-your-friends-node

  * Step 4:
    Fire it up!

      systemctl start hyperboria

    Or if you had already started hyperboria:

      systemctl restart hyperboria

For more information about other functions and non-standard setups, see README.md
