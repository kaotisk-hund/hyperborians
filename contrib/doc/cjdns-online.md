hyperboria-online(1) -- check whether hyperboria tunnel devices are available
=============================================

## SYNOPSIS

`hyperboria-online` [<options>...]

## DESCRIPTION

Hyperboria-online waits for hyperboria to make its tunnel device available so 
that services that listen on the hyperboria IP can start.  If you have
configured services to listen on the hyperboria IP, then you can use:

    systemctl enable hyperboria-wait-online

to wait until this IP is available before starting network services.
This will increase your boot time somewhat, but is needed to launch
thttpd at boot, for example.

## OPTIONS

  * `-t`, `--timeout` <timeout_value>:	
    time to wait in seconds, default 30

  * `-x`, `--exit`:		
    Exit immediately if hyperboria is not online

  * `-q`, `--quiet`:
    Don't print anything

  * `-s`, `--wait-for-startup`:
    Wait for full startup instead of just tun dev.  This is not implemented.

