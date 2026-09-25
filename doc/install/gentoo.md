# Gentoo:

hyperboria is not yet in the main Gentoo repository, so you will have to use an overlay.
The easiest way is to use Layman but you can do it by hand, too.

## Layman:

First, you need to install layman.

    emerge layman

If layman is installed correctly, you can add the overlay

    layman -f
    layman -a weuxel

For future update of the overlay use

    layman -S

Now you can install hyperboria

    emerge hyperboria

## By hand:

You will have to clone the overlay repository

    cd /opt
    git clone https://github.com/Weuxel/portage-weuxel.git

Now tell portage to use this repo

    cd /etc/portage/repos.conf/

Create a file `portage-weuxel.conf` containing

    [weuxel]
    location = /opt/portage-weuxel
    masters = gentoo
    auto-sync = yes

Now sync

    emerge --sync

And install hyperboria

    emerge hyperboria

## Automatic crash detection and restart

Copy the the openrc init script from `contrib/openrc` to `/etc/init.d/` and modify the `CONFFILE` and `command` parameter to your needs.
Then start hyperboria by issuing

    /etc/init.d/hyperboria start

Configure the init system to autostart hyperboria

    rc-update add hyperboria default

Copy the service_restart script `contrib/gentoo/service_restart.sh` to any convenient directory on
your system and modify the eMail address. If you do not wish to be notified, comment out the whole line.
Now add an crontab entry like this

    # Restart crashed Services
    * * * * *       root	/path/to/script/service_restart.sh
