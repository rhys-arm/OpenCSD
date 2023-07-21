import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.math.BigInteger;

public class runme {

  public static final long NO_FORMATTER_FLAGS = 0;

  static {
    try {
      System.loadLibrary("jopencsd");
    } catch (UnsatisfiedLinkError e) {
      System.err.println(
          "Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n"
              + e);
      System.exit(1);
    }
  }

  public static void main(String argv[]) throws IOException {
    System.out.println("hello");

    //DecodeTree *pTree = DecodeTree::CreateDecodeTree(OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);
    //ocsd_dcd_tree_src_t.OCSD_TRC_SRC_SINGLE

    DecodeTree tree = DecodeTree.CreateDecodeTree(ocsd_dcd_tree_src_t.OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);

    //jopencsd.ocsd_etmv4_cfg cfg = new jopencsd.ocsd_etmv4_cfg();
    ocsd_etmv4_cfg cfg = new ocsd_etmv4_cfg();

    cfg.setReg_idr0(0x28000EA1L);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr0()).toUpperCase());
    cfg.setReg_idr1(0x4100F403);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr1()).toUpperCase());
    cfg.setReg_idr2(0x00000488);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr2()).toUpperCase());
    cfg.setReg_idr8(0x00000000);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr8()).toUpperCase());
    cfg.setReg_idr9(0x00000000);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr9()).toUpperCase());
    cfg.setReg_idr10(0x00000000);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr10()).toUpperCase());
    cfg.setReg_idr11(0x00000000);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr11()).toUpperCase());
    cfg.setReg_idr12(0x00000000);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr12()).toUpperCase());
    cfg.setReg_idr13(0x00000000);
    System.out.println("0x" + Long.toHexString(cfg.getReg_idr13()).toUpperCase());
    cfg.setReg_configr(0x00000001);
    System.out.println("0x" + Long.toHexString(cfg.getReg_configr()).toUpperCase());
    cfg.setReg_traceidr(0x00000006);
    System.out.println("0x" + Long.toHexString(cfg.getReg_traceidr()).toUpperCase());
    cfg.setArch_ver(ocsd_arch_version_t.ARCH_V8);
    System.out.println(cfg.getArch_ver());
    cfg.setCore_prof(ocsd_core_profile_t.profile_CortexA);
    System.out.println(cfg.getCore_prof());

    EtmV4Config configObj = new EtmV4Config(cfg);
    String decoderName = jopencsdConstants.OCSD_BUILTIN_DCD_ETMV4I;
    int decoderCreateFlags = jopencsdConstants.OCSD_CREATE_FLG_FULL_DECODER;

    ocsd_err_t err = tree.createDecoder(decoderName, decoderCreateFlags, configObj);

    ocsdMsgLogger logger = new ocsdMsgLogger();
    int OUT_STDOUT = 4;
    logger.setLogOpts(OUT_STDOUT);
    logger.LogMsg("Testing stdout\n");

    ocsdDefaultErrorLogger err_log = new ocsdDefaultErrorLogger();
    err_log.initErrorLogger(ocsd_err_severity_t.OCSD_ERR_SEV_INFO);
    err_log.setOutputLogger(logger);
    DecodeTree.setAlternateErrorLogger(err_log);

    ocsd_err_t ret = tree.createMemAccMapper();
    if (ret != ocsd_err_t.OCSD_OK) {
        throw new RuntimeException();
    }

    File file = new File("../../../my_app/simple_juno_trace/juno_snapshot/mem_Cortex-A53_0_0_EXEC.bin");
    byte[] fileContent = Files.readAllBytes(file.toPath());

    ret = tree.addBufferMemAcc(BigInteger.valueOf(0x80000000L), ocsd_mem_space_acc_t.OCSD_MEM_SPACE_ANY, fileContent, fileContent.length);
    if (ret != ocsd_err_t.OCSD_OK) {
        throw new RuntimeException();
    }

    DecoderOutputProcessor decoderOutputProcessor = new DecoderOutputProcessor();
    tree.setGenTraceElemOutI(decoderOutputProcessor);

    ocsd_datapath_resp_t dataPathResp = ocsd_datapath_resp_t.OCSD_RESP_CONT;
    int trace_index = 0;
    int nUsedThisTime = 0;


    DecodeTree.DestroyDecodeTree(tree);

    // string filepath = "simple_juno_trace/etm_dump/ETM_0_6_0.bin";
    // std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    // if (!file) {
    //     cerr << "Could not open file: " << filepath << endl;
    //     return -1;
    // }
    // std::streamsize size = file.tellg();
    // file.seekg(0, std::ios::beg);
    // std::vector<char> buffer(size);
    // if (file.read(buffer.data(), size)) {
    //     dataPathResp = pTree->TraceDataIn(
    //                                 OCSD_OP_DATA,
    //                                 trace_index,
    //                                 size,
    //                                 (uint8_t *) buffer.data(),
    //                                 &nUsedThisTime);
    // } else {
    //     cerr << "Failed to read file" << endl;
    //     return -1;
    // }


    System.out.println("done");
  }

  private static class DecoderOutputProcessor extends ITrcGenElemIn {

    public ocsd_datapath_resp_t TraceElemIn(long index_sop, short trc_chan_id, OcsdTraceElement elem) {
      System.out.println("TODO");
      return ocsd_datapath_resp_t.OCSD_RESP_CONT;
    }
    
  }

  // class DecoderOutputProcessor : public ITrcGenElemIn
  // {
  // public:
  //     DecoderOutputProcessor() {};
  //     virtual ~DecoderOutputProcessor() {};
  
  //     virtual ocsd_datapath_resp_t TraceElemIn(const ocsd_trc_index_t index_sop,
  //         const uint8_t trc_chan_id,
  //         const OcsdTraceElement &elem)
  //     {
  //         // must fully process or make a copy of data in here.
  //         // element reference only valid for scope of call.
  
  //         // for the example program we will stringise and print -
  //         // but this is a client program implmentation dependent.
  //         std::string elemStr;
  //         std::ostringstream oss;
  //         oss << "Idx:" << index_sop << "; ID:" << std::hex << (uint32_t)trc_chan_id << "; ";
  //         elem.toString(elemStr);
  //         oss << elemStr << std::endl;
  //         cout << oss.str() << endl;
  //         return OCSD_RESP_CONT;
  //     }
  // };


}
