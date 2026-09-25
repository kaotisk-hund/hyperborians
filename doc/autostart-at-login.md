Autostart hyperboria when you log-in
===============================

You can configure your session to autostart hyperboria-route when you log-in to your
computer. Just add a file in `~/.config/autostart/cjdoute.desktop` containing:

    [Desktop Entry]
    Comment=
    Terminal=false
    Name=hyperboria-route
    Exec=xterm -class hyperboria-route -e bash -c '...path/to/hyperboria/hyperboria-route <~/.config/hyperboria/hyperboria-route.conf; echo "Terminated ($?)"; read'
    Categories=Network
    Keywords=hyperboria
    StartupWMClass=hyperboria-route
    Type=Application
    Icon=modem
    Version=1.0

As you can see, you should put your hyperboria-route configuration in
`~/.config/hyperboria/hyperboria-route.conf`.
