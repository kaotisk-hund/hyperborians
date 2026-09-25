#!/usr/bin/env python2

"""
Automagically generate a .hyperboriaadmin file.

Searches around for hyperboria-route.conf and the hyperboria executable, cleans the config
into proper JSON, and saves just the RPC admin info to a file. By default this
is ~/.hyperboriaadmin, but you can specify any file you want.
"""

import json
import os
import sys
import subprocess


# possibly search for running hyperboria-route processes and check the same folder as they're in
# and/or running find on the home folder

## Wanted: Everyone's favorite place to store their shit.
conflocations = ["/etc/hyperboria-route.conf",
    "~/hyperboria-route.conf",
    "~/hyperboria/hyperboria-route.conf",
    "/usr/local/opt/hyperboria/hyperboria-route.conf"]

cjdroutelocations = ["/opt/hyperboria",
    "~/hyperboria",
    "~/hyperboria-git",
    "/usr/local/opt/hyperboria"]

cjdroutelocations += os.getenv("PATH").split(":")

if len(sys.argv) == 0:
    # Write the file in the default location
    cjdnsadmin_path = os.path.expanduser("~/.hyperboriaadmin") 
else:
    # Write the file in some other location
    cjdnsadmin_path = sys.argv[1]


def ask(question, default):
    while True:
        r = raw_input("%s " % question).lower() or default

        if r in "yn":
            return r == "y"
        else:
            print "Invalid response, please enter either y or n"


def find_hyperboria-route_bin():
    for path in cjdroutelocations:
        path = os.path.expanduser(path) + "/hyperboria-route"
        if os.path.isfile(path):
            return path

    print "Failed to find hyperboria-route"
    print "Please tell me where it is"
    return raw_input("ie. <hyperboria git>/hyperboria-route: ")


def find_hyperboria-route_conf():
    for path in conflocations:
        path = os.path.expanduser(path)
        if os.path.isfile(path):
            return path

    return raw_input("Can't find hyperboria-route.conf, please give the path to it here: ")


def load_hyperboria-route_conf(conf):
    print "Loading " + conf
    try:
        with open(conf) as conffile:
            return json.load(conffile)
    except ValueError:
        return cleanup_config(conf)
    except IOError:
        print "Error opening " + conf + ". Do we have permission to access it?"
        print "Hint: Try running this as root"
        sys.exit(1)


def cleanup_config(conf):
    print "Making valid JSON out of " + conf
    print "First, we need to find the cleanconfig program"
    hyperboria-route = find_hyperboria-route_bin()
    print "Using " + hyperboria-route
    process = subprocess.Popen([hyperboria-route, "--cleanconf"], stdin=open(conf), stdout=subprocess.PIPE)
    try:
        return json.load(process.stdout)
    except ValueError:
        print "Failed to parse! Check:"
        print "-" * 8
        print "{} --cleanconf < {}".format(hyperboria-route, conf)
        print "-" * 8
        sys.exit(1)


try:
    with open(cjdnsadmin_path) as cjdnsadmin_file:
        json.load(cjdnsadmin_file)

    if not ask("%s appears to be a valid JSON file. Update? [Y/n]" % cjdnsadmin_path, "y"):
        sys.exit()
except ValueError:
    if not ask("%s appears to be a file. Overwrite? [y/N]" % cjdnsadmin_path, "n"):
        sys.exit()
except IOError:
    print "This script will attempt to create " + cjdnsadmin_path


conf = find_hyperboria-route_conf()
cjdrouteconf = load_hyperboria-route_conf(conf)

addr, port = cjdrouteconf['admin']['bind'].split(":")

cjdnsadmin = {}
cjdnsadmin["addr"] = addr
cjdnsadmin["port"] = int(port)
cjdnsadmin["password"] = cjdrouteconf['admin']['password']
cjdnsadmin["config"] = conf
with open(cjdnsadmin_path, "w+") as adminfile:
    json.dump(cjdnsadmin, adminfile, indent=4)
print "Done! Give it a shot, why dont ya"
