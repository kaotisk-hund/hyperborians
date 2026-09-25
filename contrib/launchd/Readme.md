# com.cjdelisle.hyperboria.plist

LaunchDaemon for hyperboria on OS X.

## Usage

Copy to /Library/LaunchDaemons

Set permissions and ownership with:

```
sudo chmod 644 /Library/LaunchDaemons/com.cjdelisle.hyperboria.plist
sudo chown root:wheel /Library/LaunchDaemons/com.cjdelisle.hyperboria.plist
```

Edit <string>/usr/local/bin/hyperboria-route</string> in the plist to match where you have hyperboria-route (/usr/local/bin/hyperboria-route is recommended).
Edit <string>/etc/hyperboria-route.conf</string> in the plist to match where you have the config file (/etc/hyperboria-route.conf is recommended).

Make sure config file /etc/hyperboria-route.conf exists and "noBackground":1 is set so hyperboria-route doesn't daemonize automatically.

Then load the plist with launchctl:

```
sudo launchctl load /Library/LaunchDaemons/com.cjdelisle.hyperboria.plist
```