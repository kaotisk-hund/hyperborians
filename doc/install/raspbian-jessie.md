# Installing hyperboria on raspbian jessie

This is a short guide how to setup a raspbian jessie hyperboria box.

## Install packages

```bash
sudo apt install nodejs build-essential git-svn
```

## Clone, compile, install

```bash
sudo -i

# Upgrade Node the official way
# https://nodejs.org/en/download/package-manager/#debian-and-ubuntu-based-linux-distributions
curl -sL https://deb.nodesource.com/setup_7.x | sudo -E bash -
apt install -y nodejs

# Build hyperboria
cd /opt
git clone https://github.com/cjdelisle/hyperboria.git
cd hyperboria
NO_TEST=1 Seccomp_NO=1 ./do
ln -s /opt/hyperboria/hyperboria-route /usr/bin

# Generate a config file
(umask 077 && ./hyperboria-route --genconf > /etc/hyperboria-route.conf)
# Regarding hyperboria' configuration you can continue reading here:
# https://github.com/cjdelisle/hyperboria#2-find-a-friend

# Set up a system service that runs on startup
cp contrib/systemd/hyperboria.service /etc/systemd/system/
systemctl daemon-reload
systemctl enable hyperboria
systemctl start hyperboria
```
