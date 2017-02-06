from __future__ import print_function
import sys
import yarp

yarp.Network.init()

#rf = yarp.ResourceFinder()
#rf.setVerbose(True)
#rf.setDefaultContext("myContext")
#rf.setDefaultConfigFile("default.ini")

p = yarp.BufferedPortSound()
#p.open(10)
p.open("/audio/in")

if not yarp.NetworkBase_connect("/grabber/audio", p.getName()):
    print("cannot connect to audio port")
    sys.exit(0)

for i in range(1,10):
    snd = p.read()
    if not snd:
        print("cannot read the port")

    print("frequency:", snd.getFrequency())
    print("samples:", snd.getSamples())
    print("data size:", snd.getRawDataSize())

p.close()

yarp.Network.fini()
