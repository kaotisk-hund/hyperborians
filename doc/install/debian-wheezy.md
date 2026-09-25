# Installing hyperboria on debian wheezy

This is a short guide how to setup a debian wheezy hyperboria box.

## Enable backports

	echo "deb http://http.debian.net/debian wheezy-backports main" >> /etc/apt/sources.list

## Install systemd

	aptitude install systemd-sysv

## Install packages

	aptitude install nodejs build-essential git

## Clone, compile, install

	cd /opt
	git clone https://github.com/cjdelisle/hyperboria.git
	cd hyperboria
	./do
	ln -s /opt/hyperboria/hyperboria-route /usr/bin
	(umask 077 && ./hyperboria-route --genconf > /etc/hyperboria-route.conf)
	cp contrib/systemd/hyperboria.service /etc/systemd/system/
	systemctl enable hyperboria

