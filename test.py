import os

if os.system("sudo rmmod -f modnuke.ko")!=0:
	print("Remove error")
	#raise SystemExit

os.system("rm modnuke.ko")
if os.system("make")!=0:
	print("Build error, abort")
	raise SystemExit

if os.system("sudo insmod modnuke.ko")!=0:
	print("Insert error, abort")
	raise SystemExit

print("Testing nuke on self...")


import random

data=[332,489,231,554,123,432,123,643,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
data.extend([random.randrange(0,100000000) for i in range(10000)])

print(sum(data)%1000)

import subprocess
import time
subprocess.Popen("sudo bash -c \"sleep 1 && echo %d > /proc/nuke\""%os.getpid(),shell=True)

while True:
	print("%f %d %d"%(time.time(),sum(data),sum(data)%1000))
	os.system("cat /proc/nuke")
	print("")
	time.sleep(0.5)

