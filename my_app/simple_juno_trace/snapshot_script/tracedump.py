from java.lang import Math
from java.io import BufferedOutputStream
from java.io import FileOutputStream
from java.io import FileNotFoundException
from java.io import IOException
from com.arm.debug.dtsl.interfaces import ITraceCapture
from com.arm.debug.dtsl import DTSLException
import sys
import os
import jarray


class TraceDump(object):
    """
    Class to dump the content of a trace capture device to a file
    """
    def __init__(self):
        """Construction
        """
        pass

    def dumpAllRaw(self, captureDevice, streamID, startPos, maxSize, filename):
        """Dumps all the trace content to a file in pure binary format 
        Parameters:
            filename the name of the file to dump the trace to
            progress the object to which we report our progress/state/errors
        """
        # If capture device is empty - do not overwrite any previous file
        try:
            captureSize = captureDevice.getCaptureSize()
            if (captureSize == 0):
                print ("Trace capture device %s is empty" % (captureDevice.getName()))
                return
        except DTSLException, e:
            print ("Failed to read from capture device %s (%s)" % (captureDevice.getName(), e.getLocalizedMessage()))
            return
        if streamID == 0:
            streamID = ITraceCapture.RAW_DATA
        if maxSize:
            dumpSize = min(captureSize, maxSize)
        else:
            dumpSize = captureSize
        if startPos + dumpSize > captureSize:
            dumpSize = captureSize - startPos
        try:
            # We use the Java file IO system to write to the file 
            out = BufferedOutputStream(FileOutputStream(filename))
        except FileNotFoundException, e:
            print ("Failed to create output file %s (%s)" % (filename, e.getLocalizedMessage()))
            return
        try:
            # big chunk size ensures reasonable performance from large capture devices
            MAX_CHUNK = 2 * 1024 * 1024
            # Create a byte array as a data buffer
            captureBuf = jarray.zeros(MAX_CHUNK, 'b')
            if (dumpSize < 1024):
                units = "bytes"
                divisor = 1.0
            elif (dumpSize < 1024*1024):
                units = "KB"
                divisor = 1024.0
            else:
                units = "MB"
                divisor = 1024.0*1024.0
            bytesLeft = dumpSize
            pcComplete = 0
            # This is used to tell us where to read from next
            nextPos = jarray.zeros(1, 'l')
            nextPos[0] = startPos
            while (bytesLeft > 0):
                bytesThisChunk = Math.min(bytesLeft, MAX_CHUNK)
                extractedBytes = captureDevice.getSourceData(
                                    streamID, 
                                    nextPos[0], bytesThisChunk, captureBuf, nextPos)
                out.write(captureBuf, 0, extractedBytes)
                bytesLeft -= bytesThisChunk
        except IOException, e:
            print ("Failed to write to output file %s (%s)" % (filename, e.getLocalizedMessage()))
        except DTSLException, e:
            print ("Failed to read from capture device %s (%s)" % (captureDevice.getName(), e.getLocalizedMessage()))
        finally:
            out.close()
            
    def listCaptureDevices(self, dtslConfiguration):
        """Prints out a list of all trace capture devices and shows the one currently in use
        """
        # Find the requested trace capture device
        captureDevices = dtslConfiguration.getTraceCaptureInterfaces()
        if (captureDevices == None):
            print "No trace capture devices found"
        else:
            print "Registered trace capture devices:"
            for deviceName in captureDevices.keySet():
                traceCaptureDevice = captureDevices.get(deviceName)
                state = "INACTIVE"
                if (traceCaptureDevice.isActive()):
                    state = "ACTIVE"
                print "    %s [%s]" % (deviceName, state)
            selectedTraceDeviceOptionName = "options.tracecapture.capture"
            try:
                selectedTraceDevice = dtslConfiguration.getOptionValue(selectedTraceDeviceOptionName)
                if (selectedTraceDevice != None):
                    print "Selected trace capture device: %s" %(selectedTraceDevice)
                else:
                    print "Selected trace capture device has not been set"
                selectedTraceDevice = dtslConfiguration.getSelectedTraceCaptureDevice()
                if (selectedTraceDevice != None):
                    print "Selected trace capture device: %s" %(selectedTraceDevice)
                else:
                    print "Selected trace capture device has not been set"
            except DTSLException, e:
                print "Failed to read currently selected trace capture device from %s" % (selectedTraceDeviceOptionName)
                
