# Installing HYPERBORIA on ChromeOS

This is a short guide how to setup HYPERBORIA on a Chromebook.

## Install packages

Enable the Linux beta and open a terminal.

```bash
sudo apt install nodejs build-essential git-svn
```

## Clone, compile, install

```bash
sudo -i

# Build hyperboria
cd /opt
git clone https://github.com/cjdelisle/hyperboria.git
cd hyperboria
./do
ln -s /opt/hyperboria/hyperboria-route /usr/bin

# Generate a config file
(umask 077 && ./hyperboria-route --genconf > /etc/hyperboria-route.conf)
# Regarding hyperboria' configuration you can continue reading here:
# https://github.com/cjdelisle/hyperboria#2-find-a-friend

# Set up a system service that runs on startup
cp contrib/systemd/hyperboria.service /etc/systemd/system/
systemctl enable hyperboria
# the service `hyperboria-resume` does not work currently
systemctl start hyperboria
```
