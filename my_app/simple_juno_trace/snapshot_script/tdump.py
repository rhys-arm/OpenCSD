#! /usr/bin/env python

from arm_ds.debugger_v1 import Debugger
from com.arm.debug.dtsl.configurations import DTSLv1
from com.arm.debug.dtsl import ConnectionManager
from optparse import OptionParser  
from progress import Progress
import sys
from tracedump import TraceDump
from progress import Progress

def getDebugger():
    debugger = Debugger()
    if not debugger.isConnected():
        sys.exit()
    return debugger

def getDTSLConfiguration(debugger):
    dtslConnectionConfigurationKey = debugger.getConnectionConfigurationKey()
    dtslConnection = ConnectionManager.openConnection(dtslConnectionConfigurationKey)
    return dtslConnection.getConfiguration()

def dumpTrace(device, streamID, startPos, maxSize, addRVTHeader, filename):
    debugger = getDebugger()
    dtslConfiguration = getDTSLConfiguration(debugger)
    progress = Progress()
    tracedump = TraceDump()
    tracedump.dumpTraceToFile(device, dtslConfiguration, streamID, startPos, maxSize, addRVTHeader, filename, progress)

def listCaptureDevices():
    debugger = getDebugger()
    dtslConfiguration = getDTSLConfiguration(debugger)
    tracedump = TraceDump()
    tracedump.listCaptureDevices(dtslConfiguration)

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-d", "--device", action="store", type="string", dest="device", default="")
    parser.add_option("-i", "--stream", action="store", type="int", dest="stream", default="0")
    parser.add_option("-p", "--position", action="store", type="int", dest="position", default="0")
    parser.add_option("-s", "--size", action="store", type="int", dest="size", default="0")
    parser.add_option("-f", "--outputfile", action="store", type="string", dest="filename", default="tracedump.bin")
    parser.add_option("-r", "--rtvheader", action="store_true", dest="header", default=False)
    parser.add_option("-l", "--list", action="store_true", dest="list")
    (options, args) = parser.parse_args()
    if (options.list):
        listCaptureDevices()
    if (options.device != ""):
        dumpTrace(options.device, options.stream, options.position, options.size, options.header, options.filename)
