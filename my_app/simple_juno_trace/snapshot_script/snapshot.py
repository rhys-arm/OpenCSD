from arm_ds.debugger_v1 import Debugger
from arm_ds.debugger_v1 import DebugException

from tracedump import TraceDump

from com.arm.debug.dtsl import ConnectionManager
from com.arm.debug.dtsl.snapshot import RegisterValue
from com.arm.debug.dtsl.snapshot import SnapshotMetadataBuilder
from com.arm.debug.dtsl.snapshot import Snapshot
from com.arm.debug.dtsl.interfaces import ITraceCapture
import struct
import re
import sys
import os
import getopt


def usage():
    print "usage: snapshot.py [ -c core_to_dump ] [ -d trace_to_dump ] [ -x start..end ] output_dir"
    print "    -c core_to_dump: only include state of specified cores. May occur multiple times"
    print "    -d trace_to_dump: only include trace data from specified devices. May occur multiple times"
    print "                      may be given as:"
    print "                        \"trace_capture\" dump buffer contents of trace capture as CoreSight frames"
    print "                        \"trace_capture:trace_source\": dump only data for trace_source from the buffer"
    print "    -x start..end: Add memory region start..end to dump. May occur multiple times"

def getDTSLConfiguration(debugger):
    dtslConnectionConfigurationKey = debugger.getConnectionConfigurationKey()
    dtslConnection = ConnectionManager.openConnection(dtslConnectionConfigurationKey)
    return dtslConnection.getConfiguration()


def parseCoreInfo(coreInfo):
    clusters = { }
    currentClusterCores = []
    currentCluster = '<none>'
    for l in coreInfo.split('\n'):
        m = re.match('^Cluster (.*):', l)
        if m:
            if currentClusterCores:
                clusters[currentCluster] = currentClusterCores
                currentClusterCores = []
            currentCluster = m.group(1)
        m = re.match(r'^[0-9 *]+\s+\| ([0-9a-zA-Z_-]+)\s+\| .*', l)
        if m:
            coreName = m.group(1)
            # If we've been given a list of cores to look at then ignore others
            if not dump_cores or coreName in dump_cores:
                currentClusterCores.append(m.group(1))

    if currentClusterCores:
        clusters[currentCluster] = currentClusterCores
        currentClusterCores = []
    print clusters
    return clusters

def dumpRegisters(ec, snapshotBuilder):
    rs = ec.getRegisterService()
    regNames = rs.getRegisterNames()
    regVals = []
    for r in regNames:
        try:
            canRead = rs.getProperty(r, "readAccess")
            if not canRead:
                continue

            v = rs.getValue(r)
            sz = rs.getSize(r)
            if sz <= 32:
                sz = 32
                v = struct.unpack('<i', struct.pack('<I', int(v)))
            elif sz <= 64:
                sz = 64
                v = struct.unpack('<ii', struct.pack('<Q', long(v)))
            elif sz <= 128:
                continue
                v = struct.unpack('<iiii', struct.pack('<QQ',
                                                       long(v) & 0xFFFFFFFFFFFFFFFFl,
                                                       long(v) >> 64))
            n = r.split('::')[-1]
            regVals.append(RegisterValue(n, -1, sz, v))
        except DebugException, e:
            # print r, e.getMessage()
            continue

    snapshotBuilder.addCoreRegisters(ec.getName(), regVals)


def parseImage(ec):
    '''Return list of (name, start address, end address) for sections in images'''
    fileInfo = ec.executeDSCommand('info files')
    imgInfo = []
    imgNum = 0
    for l in fileInfo.split('\n'):
        m = re.match(r'\s*([:\w]+) - ([:\w]+) is ([\w_.]+)', l)
        if m:
            imgInfo.append( ('%d_%s' % (imgNum, m.group(3)), m.group(1), m.group(2)) )
            imgNum += 1
    return imgInfo


def dumpSection(debugger, dumpContexts, snapshotBuilder, dumpDir, name, start, end):
    ec = debugger.getCurrentExecutionContext()
    es = ec.getExecutionService()
    ms = ec.getMemoryService()

    # dump to file
    dumpFile = "mem_" + ec.getName() + "_" + name + ".bin"
    dumpPath = os.path.join(dumpDir, dumpFile)
    if os.path.exists(dumpPath):
        os.unlink(dumpPath)
    ms.dump(dumpPath, "binary", start, end)
    if ':' in str(start):
        prefix, addr = str(start).split(':')
    else:
        prefix, addr = '', start

    addr = long(addr, 16)
    # convert to signed long 
    if addr > 0x7FFFFFFFFFFFFFFFL:
        addr -= 0x8000000000000000L

    print start, end, prefix, addr

    # add to all contexts
    for ec in dumpContexts:
        snapshotBuilder.addCoreMemoryRegion(ec.getName(), prefix, addr, dumpFile)


def dumpMemoryAroundPC(ec, snapshotBuilder, dumpDir):
    es = ec.getExecutionService()
    pc = es.getExecutionAddress()
    ms = ec.getMemoryService()
    dumpFile = "mem_" + ec.getName() + ".bin"
    dumpPath = os.path.join(dumpDir, dumpFile)
    if os.path.exists(dumpPath):
        os.unlink(dumpPath)
    dumpAddr = pc - 4096
    ms.dump(dumpPath, "binary", dumpAddr, pc + 4095)
    if ':' in str(pc):
        prefix = str(pc).split(':')[0]
    else:
        prefix = ''
    snapshotBuilder.addCoreMemoryRegion(ec.getName(), prefix, long(dumpAddr), dumpFile)


PART_MAP = {
    0xC20: 'Cortex-M0',
    0xC60: 'Cortex-M0+',
    0xC21: 'Cortex-M1',
    0xC23: 'Cortex-M3',
    0xC24: 'Cortex-M4',
    0xC27: 'Cortex-M7',
    0xC05: 'Cortex-A5',
    0xC07: 'Cortex-A7',
    0xC08: 'Cortex-A8',
    0xC09: 'Cortex-A9',
    0xC0E: 'Cortex-A17',
    0xC0F: 'Cortex-A15',
    0xD03: 'Cortex-A53',
    0xD07: 'Cortex-A57',
    0xD08: 'Cortex-A72',
    0xD0F: 'ARMAEMv8-A_',
}

def getCoreTypeFromIDRegister(ec):
    rs = ec.getRegisterService()
    for idRegName in [ 'System::ID::MIDR_EL1',
                       'System::ID::MIDR',
                       'CP15::System::MIDR',
                       'CP15::ID::MIDR',
                       'System::ID::CPUID',
                       'SystemControlID::SystemControl::CPUID' ]:
        try:
            idRegVal = rs.getValue(idRegName)
            partNo = (long(idRegVal) & 0xFFF0) >> 4
            print hex(partNo)
            return PART_MAP.get(partNo, '')
        except DebugException, e:
            # print e.getMessage()
            continue
    return ''


def dumpCoreRegisters(debugger, dumpContexts, snapshotBuilder):
    ec = debugger.getCurrentExecutionContext()
    coreInfo = ec.executeDSCommand('info cores')
    clusters = parseCoreInfo(coreInfo)

    coreTypes = {}
    for ec in dumpContexts:
        coreType = getCoreTypeFromIDRegister(ec)
        coreTypes[ec.getName()] = coreType

    print coreTypes

    # build a set of all sub-cores in clusters
    clusterCores = set()
    if len(clusters) > 1:
        for name, cores in clusters.items():
            snapshotBuilder.addCluster(name, coreTypes.get(cores[0], ''), cores)
            for c in cores:
                clusterCores.add(c)

    for ec in dumpContexts:
        print 'Dumping state of %s' % ec.getName()

        # may have already been seen in cluster
        if not ec.getName() in clusterCores:
            snapshotBuilder.addCore(ec.getName(), coreTypes.get(ec.getName()))

        dumpRegisters(ec, snapshotBuilder)


def dumpMemory(debugger, dumpContexts, extra_sections, snapshotBuilder, dumpDir):
    ec = debugger.getCurrentExecutionContext()
    imgInfo = parseImage(ec)
    print imgInfo
    if imgInfo:
        for name, start, end in imgInfo:
            try:
                print "Dumping section %s" % name
                dumpSection(debugger, dumpContexts, snapshotBuilder, dumpDir, name, start, end)
            except DebugException, e:
                print e.getMessage()
                pass

    else:
        for ec in dumpContexts:
            dumpMemoryAroundPC(ec, snapshotBuilder, dumpDir)

    for addr, end in extra_sections:
        try:
            name = 'mem_%s' % addr
            print name
            dumpSection(debugger, dumpContexts, snapshotBuilder, dumpDir, name, addr, end)
        except DebugException, e:
            print e.getMessage()
            pass


def dumpTrace(dtslConfiguration, dump_trace_devices, snapshotBuilder):
    traceDump = TraceDump()

    traceCaptures = list(dtslConfiguration.getTraceCaptureInterfaces().keySet())
    traceSources = dict((s.getName(), s) for s in dtslConfiguration.getAllTraceSources())

    if not dump_trace_devices:
        dump_trace_devices = traceCaptures
    else:
        print "Looking for trace capture device %s in DTSL list %s" % (dump_trace_devices,traceCaptures)

    for devName in dump_trace_devices:
        if devName in traceCaptures:
            print "Dumping trace capture %s" % devName
            tc = dtslConfiguration.getTraceCaptureInterfaces().get(devName)
            stream = ITraceCapture.EXPORT_DATA
        else:
            print "Dumping trace source %s" % devName
            tcName, devName = devName.split(':', 1)
            tc = dtslConfiguration.getTraceCaptureInterfaces().get(tcName)
            source = traceSources.get(devName)
            stream = source.getStreamID()
        traceFile = devName + ".bin"
        dumpPath = os.path.join(dumpDir, traceFile)
        captureSize = tc.getCaptureSize()
        if captureSize > 0:
            traceDump.dumpAllRaw(tc, stream, 0, captureSize, dumpPath)
            snapshotBuilder.addTraceDump(devName, tc, stream, traceFile)


def createSnapshot(dump_cores, dump_trace_devices, extra_sections, dumpDir):
    print 'Creating snapshot in %s' % dumpDir
    if not os.path.exists(dumpDir):
        os.makedirs(dumpDir)

    debugger = Debugger()
    dtslConfiguration = getDTSLConfiguration(debugger)
    snapshotBuilder = SnapshotMetadataBuilder(dtslConfiguration)

    dumpContexts = []
    for i in range(debugger.getExecutionContextCount()):
        ec = debugger.getExecutionContext(i)
        if ec.getKind() == 'CORE' and \
                (not dump_cores or ec.getName() in dump_cores):
            dumpContexts.append(ec)

    dumpCoreRegisters(debugger, dumpContexts, snapshotBuilder)

    dumpMemory(debugger, dumpContexts, extra_sections, snapshotBuilder, dumpDir)

    dumpTrace(dtslConfiguration, dump_trace_devices, snapshotBuilder)

    Snapshot.write(snapshotBuilder.getMetadata(), dumpDir)


if __name__ == '__main__':

    dump_cores = []
    dump_trace_devices = []
    extra_sections = [ ]

    opts, args = getopt.getopt(sys.argv[1:], 'd:c:x:h')
    for o, a in opts:
        if o == '-d':
            dump_trace_devices.append(a)
        elif o == '-c':
            dump_cores.append(a)
        elif o == '-x':
            if not '..' in a:
                usage()
                sys.exit(1)
            s, e = a.split('..')
            extra_sections.append((s, e))
        elif o == '-h':
            usage()
            sys.exit(0)

    if len(args) < 1:
        usage()
        sys.exit(1)

    dumpDir = args[0]

    createSnapshot(dump_cores, dump_trace_devices, extra_sections, dumpDir)
