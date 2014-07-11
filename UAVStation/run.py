#!/usr/bin/python2.7

#
#   IMPORT
#
import os
import sys
import colorama as clr

#
#   DATA
#
VERSION_MAJOR="0"
VERSION_MINOR="1"

ffmpeg_cmd="LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libv4l/v4l1compat.so ffmpeg " +\
            "-f video4linux2 -vcodec rawvideo -video_size 352x288 -r 10 -i /dev/video0 " +\
            "-f flv -metadata streamName=livestream -an tcp://127.0.0.1:6666/ > /dev/null 2>&1"

modules=[
    { 
        'path':'/lib/modules/3.4.67+/kernel/drivers/media/video/gspca/gspca_main.ko', 
        'name':'gspca_main'
    },
    { 
        'path':'/home/olimex/DRONE/modules/gspca_spca561.ko', 
        'name':'gspca_spca561'
    }
]

progs_start=[
    {
        'name':'crtmpserver',
        'cmd':'/etc/init.d/crtmpserver stop',
        'dofork':False,
        'exit-on-fail':False
    },
    {
        'name':'crtmpserver',
        'cmd':'/etc/init.d/crtmpserver start',
        'dofork':False,
        'exit-on-fail':True
    },
    {
        'name':'ffmpeg',
        'cmd':ffmpeg_cmd,
        'dofork':True,
        'exit-on-fail':True
    }
]

progs_stop=[
    {
        'name':'crtmpserver',
        'cmd':'/etc/init.d/crtmpserver stop',
        'dofork':False,
        'exit-on-fail':False
    },
    {
        'name':'ffmpeg',
        'cmd':'killall ffmpeg',
        'dofork':False,
        'exit-on-fail':False
    }
]


#
#   UTILS
#
def __progsExec(progs):
    # Start all progs
    for p in progs:
        print ""
        print "Executing '" + p['cmd'] + "' ... "

        if p['dofork']:
            # Forking
            pid = os.fork()
            if pid == -1:
                # Error
                print ""
                print ">>> " + clr.Fore.RED + "LAST COMMAND FAILED: "+ clr.Fore.RESET + p['cmd']
                print ">>> " + clr.Fore.RED + "An error occured while forking process." + clr.Fore.RESET
                if p['exit-on-fail']:
                    print "Exiting ..."
                    sys.exit(-1)
                else:
                    continue
            elif pid == 0:
                # Child process
                if os.system(p['cmd']) != 0:
                    print ""
                    print ">>> " + clr.Fore.RED + "LAST COMMAND FAILED: " + clr.Fore.RESET + p['cmd']
                    print ">>> " + clr.Fore.RED + "Last command reported failure." + clr.Fore.RESET
                    sys.exit(-1)
            else:
                # Parent process
                continue
        else:
            # No Forking
            if os.system(p['cmd']) != 0:
                print ""
                print ">>> " + clr.Fore.RED + "LAST COMMAND FAILED: " + clr.Fore.RESET + p['cmd']
                print ">>> " + clr.Fore.RED + "Last command reported failure." + clr.Fore.RESET
                if p['exit-on-fail']:
                    print "Exiting ..."
                    sys.exit(-1)
                else:
                    continue


def __loadModules():
    for m in modules:
        fd = os.popen("lsmod | grep "+m['name'], "r")
        if fd.read(1):
            print "Module '"+m['name']+"' already loaded."
        else:
            print "Loading module '"+m['name']+"' ... ",
            if os.system("insmod "+m['path']) != 0:
                print ""
                print ">>> " + clr.Fore.RED + "CANNOT LOAD MODULE: "+ clr.Fore.RESET + m['name']
                print "Exiting ..."
                sys.exit(-1)
        fd.close()

def __unloadModules():
    for m in modules:
        fd = os.popen("lsmod | grep "+m['name'], "r")
        if fd.read(1):
            print "Unloading module '"+m['name']+"' ... "
            if os.system("rmmod "+m['path']) != 0:
                print ""
                print ">>> " + clr.Fore.RED + "CANNOT UNLOAD MODULE: "+ clr.Fore.RESET + m['name']
                print ""
        fd.close()

#
#   ACTIONS
#
def start():
    print "==== Inserting modules ===="
    __loadModules() 
    
    print ""
    print "==== Starting streaming ===="
    __progsExec(progs_start)


def stop():
    print "==== Stopping streaming ===="
    __progsExec(progs_stop)


def usage():
    print "Version: " + VERSION_MAJOR + "." + VERSION_MINOR
    print "This sets up the environment for the drone to run."
    print "Usage: "+sys.argv[0]+" <action>"
    print ""
    print "Actions:"
    print "  help      Prints this message."
    print "  start     Load all the needed kernel modules and start all the required services."
    print "            In case of failure, you might want to clean everything up running"
    print "            by running the 'stop' action."
    print "  stop      Stops all the daemons. Modules are not unloaded, see 'unload'."
    print "  unload    Unload the kernel modules loaded with 'start'."


#
#   MAIN
#
actions={
    'start':start,
    'stop':stop,
    'help':usage
}
if __name__ == "__main__":
    clr.init()
    actions[sys.argv[1]]()
