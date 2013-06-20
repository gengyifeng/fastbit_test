#!/usr/bin/env python
import os,subprocess
import time
def run():
#    files=["ll2048", "hl2048", "lr2048", "hr2048"]
#    files=["ll2048", "hl2048"]
#    files=["lr2048", "hr2048"]
    files=["hr2048"]
    bounds=["2", "4", "8", "16", "32", "64"]
#    bounds=["2", "4", "8", "16", "32", "64"]
#    bounds=["16", "32", "64"]
#    bounds=["2", "4", "8"]
#    args=' -v v1 "[0,0.5]"'
    clearcmd="echo 3 > /proc/sys/vm/drop_caches"
    args=' -d d1 "[0,1023]" -d d2 "[0,511] -d d3 "[0,255]"'
#    rfile=open("./result",'w');
    for file in files:
        for bound in bounds:
            cmd="bbsearch "+file+"_"+bound+args+" "
#            cmd="ls -l result"
            print  cmd;
            retval=subprocess.call(clearcmd,shell=True);
            retval=subprocess.call(cmd,shell=True);
if __name__=="__main__":
    run()

