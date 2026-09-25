# Building for Windows

* If you're trying to build **on** windows, forget it, you wouldn't build software for an iPhone
**on** an iPhone...

## Build Prerequisites

Install mingw32

For Ubuntu 14.04:

    sudo apt-get install mingw-w64
    
For older Ubuntu:

    sudo apt-get install gcc-mingw32


## Build Process

Cross-compile hyperboria with the following command:

    SYSTEM=win32 CROSS_COMPILE=i686-w64-mingw32- ./cross-do

## Run-time Dependencies

On your Windows machine, you need the TAP driver installed to allow hyperboria to create its virtual network interface. You can get it from the OpenVPN project at their [main download page](https://openvpn.net/index.php/open-source/downloads.html), under "Tap-windows", or use [this direct link to version 9.9.2_3](http://swupdate.openvpn.org/community/releases/tap-windows-9.9.2_3.exe).
    
Check name of your new virtual connection it must contain only english letters or numbers
## Installation

Once the TAP driver is installed, copy the `hyperboria-route.exe` file over to your windows machine.

Generate a configuration file with:

    hyperboria-route --genconf > hyperboria-route.conf
    
You probably want to uncomment the `"logTo":"stdout"` line so that you can see any error messages in your terminal.

Then, execute hyperboria from an elevated command prompt:

    hyperboria-route < hyperboria-route.conf

The first time you start it, a Windows firewall dialog will probably pop up. *Make sure to allow hyperboria to accept connections from the Internet.*

## Next Steps

Congratulations! You are now running hyperboria on Windows! Go find some peers!

