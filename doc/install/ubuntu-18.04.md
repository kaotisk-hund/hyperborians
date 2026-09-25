# Installing hyperboria on Ubuntu 16.04

This is a short guide how to setup an Ubuntu hyperboria box.

## Install packages

	sudo apt-get install nodejs git build-essential python2.7 make

## Clone, compile, install

	cd /opt
	git clone https://github.com/cjdelisle/hyperboria.git
	cd hyperboria
	./do
	ln -s /opt/hyperboria/hyperboria-route /usr/bin/
	(umask 077 && ./hyperboria-route --genconf > /etc/hyperboria-route.conf)
	cp contrib/systemd/hyperboria.service contrib/systemd/hyperboria-resume.service /lib/systemd/system/
	systemctl enable hyperboria
	systemctl start hyperboria

