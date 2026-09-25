# OS/distribution-specific quirks

## macOS

### Failure to autopeer

```IRC
16:46 < ansuz> sounds like you guys are in srs mode, but that mac no-auto-peering thing only really has one line of documentation
16:46 < ansuz> at some point if someone could fill me in on the why, it would get more
16:47 <@hyperboria> ahh, it's because generating ethernet frames is different per operating system
16:47 <@hyperboria> there's no standardized socket(SO_LINK_LAYER, "eth0")
16:48 <@hyperboria> so ETHInterface_darwin.c just isn't written
16:49 <@hyperboria> now you can copy/paste that into a little txt file and when some poor bastard asks why, tell him and then shout at him to update the documentation
16:49 <@hyperboria> and if he doesn't, shitlist him so no more questions \:D/
16:49 <@Arceliar> or bug people who own a mac to write ETHInterface_darwin.c
```

There you have it, Macs don't autopeer via Ethernet frames.

This is also probably why it doesn't work on windows, either.

## Raspbian

### Failure to start Hyperboria
When running Hyperboria on Raspberry Pi with Raspbian you can run in to problems because IPv6 is not enabled by default. This causes Hyperboria to fail on startup with the following error message:

```
CRITICAL Configurator.c:107 Got error [UDPAddrIface.c:273 call to uv_udp_bind() failed [bad file descriptor]] calling [UDPInterface_new]
```

Enable IPv6 with

```bash
$ sudo modprobe ipv6
```

and Hyperboria should start correctly. To keep IPv6 on after reboot add `ipv6` to the end of the file `/etc/modules`

```bash
$ sudo sh -c 'echo ipv6 >> /etc/modules'
```
