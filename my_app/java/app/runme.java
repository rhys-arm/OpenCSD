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

  public static void main(String argv[]) {
    System.out.println("hello");

    //DecodeTree *pTree = DecodeTree::CreateDecodeTree(OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);
    //ocsd_dcd_tree_src_t.OCSD_TRC_SRC_SINGLE

    DecodeTree dt = DecodeTree.CreateDecodeTree(ocsd_dcd_tree_src_t.OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);

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

    

    // EtmV4Config configObj(&config);     // initialise decoder config class 
    // std::string decoderName(OCSD_BUILTIN_DCD_ETMV4I);  // use built in ETMv4 instruction decoder.
    // int decoderCreateFlags = OCSD_CREATE_FLG_FULL_DECODER; // decoder type to create - OCSD_CREATE_FLG_PACKET_PROC for packet processor only
    // ocsd_err_t err = pTree->createDecoder(decoderName, decoderCreateFlags, &configObj);

    // ocsdMsgLogger logger;
    // logger.setLogOpts(ocsdMsgLogger::OUT_STDOUT);
    // logger.LogMsg("Testing stdout");

    // ocsdDefaultErrorLogger err_log;
    // err_log.initErrorLogger(OCSD_ERR_SEV_INFO);
    // err_log.setOutputLogger(&logger);
    // pTree->setAlternateErrorLogger(&err_log);

    // ItemPrinter *pPrinter;
    // pTree->addPacketPrinter(0, false, &pPrinter);
  }
}
