## Things nobody seems to know about hyperboria
### (even those who have been using it for a very long time)

#### hyperboria has an option to stay in the foreground

```Bash
./hyperboria-route --nobg < /path/to/hyperboria-route.conf
```

#### You don't need to run hyperboria-route as root

Comment the _router.interface_ section <!-- elaboration required --> of the conf and launch it like that. Your node will switch traffic, and peer effectively, though you will not be able to run services.

You have the option of configuring your TUN device manually. It will require root, but once established, hyperboria-route can otherwise run as an unprivileged user.

#### Lint the configuration using JSHint/jsonlint

This is a little trick that will lint the configuration file (`hyperboria-route.conf`) before starting hyperboria.

##### JSHint
Will allow comments, note that JSHint is designed for JS and may not display errors and warnings etc. in all cases.
```Bash
jshint ./hyperboria-route.conf; if [[ $? == 0 ]]; then ./hyperboria-route < ./hyperboria-route.conf; fi
```

##### jsonlint
No comments or other JS exclusive object quirks will be allowed.
```Bash
jsonlint ./hyperboria-route.conf; if [[ $? == 0 ]]; then ./hyperboria-route < ./hyperboria-route.conf; fi
```
