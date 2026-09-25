# Installing hyperboria on debian jessie

This is a short guide how to setup a debian jessie hyperboria box.

## Install packages

	apt install nodejs build-essential git

## Clone, compile, install

	cd /opt
	git clone https://github.com/cjdelisle/hyperboria.git
	cd hyperboria
	./do
	ln -s /opt/hyperboria/hyperboria-route /usr/bin
	(umask 077 && ./hyperboria-route --genconf > /etc/hyperboria-route.conf)
	cp contrib/systemd/*.service /etc/systemd/system/
	systemctl enable hyperboria
	systemctl start hyperboria

