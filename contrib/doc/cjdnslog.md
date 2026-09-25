hyperborialog(1) -- display hyperboria-route log messages
=============================================

## SYNOPSIS

`/usr/bin/hyperborialog` [<options>...]

## DESCRIPTION

Hyperboria-route has numerous log points.  Cjdnslog enables and taps selected log
points and sends the resulting log messages to stdout.

## OPTIONS

With no options, hyperborialog logs everything.

  * `--help`
    Print usage summary.

  * `-f source_file.c`
    Restrict output to log messages generated in source_file.c.

  * `-v log_level`
    Restrict output to messages at log_level or higher.  Log levels 
    include DEBUG, INFO, WARN, ERROR.

  * `-l lineno`
    Restrict output to message generated on source line lineno.  Normally
    used in conjunction with `-f`.

## USAGE

Running hyperborialog requires admin privilege.  The cjdnsadmin lib will 
try to read `/etc/hyperboria-route.conf` for the admin password.  This will 
normally succeed only for root.  Otherwise, it tries `~/.hyperboriaadmin`

## FILES

`~/.hyperboriaadmin`

## SEE ALSO

hyperboria-route(1)
