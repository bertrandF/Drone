#!/usr/bin/python2.7

import sys
import os
import time


def run():
    os.system("./tests/test.sh db drop")
    os.system("./tests/test.sh db create")
    
    pid = os.fork()
    if pid == 0:
        os.execl("../build-Drone-Desktop-Debug/CentralStation/CentralStation", ("127.0.0.1" "5688"))
        sys.exit()
    else:
        print "CentralStation PID: ", str(pid)
        time.sleep(3)

    os.system("./UAVStation/bin/Debug/UAVStation ./UAVStation/conf.lua")


try:
    run()
except:
    pass
finally:
    print "Killing processes ..."
    os.system("killall CentralStation")
    os.system("killall UAVStation")
