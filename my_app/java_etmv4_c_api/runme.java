import com.arm.debug.trace.decode.jopencsd.*;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
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
    System.out.println("start");

    SWIGTYPE_p_void handle = jopencsd.ocsd_create_dcd_tree(ocsd_dcd_tree_src_t.OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);

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

    SWIGTYPE_p_void cfg_void = jopencsd.ocsd_etmv4_cfg_to_void(cfg);

    SWIGTYPE_p_unsigned_char CSID = jopencsd.new_unsigned_char_ptr();
    ocsd_err_t ret = jopencsd.ocsd_dt_create_decoder(handle, jopencsdConstants.OCSD_BUILTIN_DCD_ETMV4I, jopencsdConstants.OCSD_CREATE_FLG_FULL_DECODER,
                                  cfg_void, CSID);
    System.out.println("ret = " + ret);
    if (ret != ocsd_err_t.OCSD_OK) {
      throw new RuntimeException();
    }

    System.out.println("CSID = " + jopencsd.unsigned_char_ptr_value(CSID));

      Callback2 callback2 = new JavaCallback2();
      SWIGTYPE_p_void nothing = null;
      ret = jopencsd.ocsd_dt_add_callback_trcid_mem_acc_wrapper(handle, BigInteger.valueOf(0x80000000L), BigInteger.valueOf(0x80000023L),
                         ocsd_mem_space_acc_t.OCSD_MEM_SPACE_ANY, callback2, nothing);
      if (ret != ocsd_err_t.OCSD_OK) {
        throw new RuntimeException();
      }
    
      // ret = jopencsd.ocsd_dt_add_callback_trcid_mem_acc(handle, BigInteger.valueOf(0x80000000L), BigInteger.valueOf(0x80000023L),
      //                    ocsd_mem_space_acc_t.OCSD_MEM_SPACE_ANY, my_mem_acc_function, nothing);
      if (ret != ocsd_err_t.OCSD_OK) {
        throw new RuntimeException();
      }

    Callback callback = new JavaCallback();
    ret = jopencsd.ocsd_dt_set_gen_elem_outfn_wrapper(handle, callback, nothing);
    if (ret != ocsd_err_t.OCSD_OK) {
      throw new RuntimeException();
    }

      File etmFile = new File("../simple_juno_trace/etm_dump/ETM_0_6_0.bin");
      byte[] etmFileContent = Files.readAllBytes(etmFile.toPath());
      ByteBuffer etmFileContentBB = ByteBuffer.allocateDirect(etmFileContent.length);
      etmFileContentBB.put(etmFileContent);
      etmFileContentBB.flip();
      ocsd_datapath_resp_t dataPathResp = ocsd_datapath_resp_t.OCSD_RESP_CONT;
      long trace_index = 0;
      SWIGTYPE_p_unsigned_int nUsedThisTime = jopencsd.new_uint32_t_ptr();
      System.out.println("etmFileContent.length = " + etmFileContent.length);
      dataPathResp = jopencsd.ocsd_dt_process_data(handle, ocsd_datapath_op_t.OCSD_OP_DATA, trace_index, etmFileContentBB, nUsedThisTime);

      System.out.println("dataPathResp = " + dataPathResp);
      System.out.println("Number of bytes processed = " + jopencsd.uint32_t_ptr_value(nUsedThisTime));

      jopencsd.ocsd_destroy_dcd_tree(handle);

      System.out.println("done");
  }


}

class JavaCallback extends Callback {

  public ocsd_datapath_resp_t my_decoder_output_processor(SWIGTYPE_p_void p_context, long index_sop, short trc_chan_id,
      ocsd_generic_trace_elem elem) {
    String str = "Idx:" + index_sop + "; ID:" + trc_chan_id + "; ";
    str += jopencsd.ocsd_generic_trace_elem_to_string(elem);
    System.out.println(str);
    return ocsd_datapath_resp_t.OCSD_RESP_CONT;
  }

}

class JavaCallback2 extends Callback2 {

  public long my_mem_acc_function(SWIGTYPE_p_void p_context, java.math.BigInteger address, ocsd_mem_space_acc_t mem_space,
                  short trcID, long reqBytes, SWIGTYPE_p_unsigned_char byteBuffer) {
      System.out.println("my_mem_acc_function");

      File memFile = new File("../simple_juno_trace/juno_snapshot/mem_Cortex-A53_0_0_EXEC.bin");
      byte[] memFileContent;
      try {
        memFileContent = Files.readAllBytes(memFile.toPath());
      } catch (IOException e) {
        e.printStackTrace();
        return 0;
      }

    int memFilePos = java.lang.Math.toIntExact(address.longValueExact() - 0x80000000L);
    System.out.println("memFilePos = " + memFilePos);

    int numBytes = 0;
    for (numBytes = 0; numBytes < reqBytes && numBytes < memFileContent.length; numBytes++) {
      System.out.println("writing 0x" + Long.toHexString(memFileContent[memFilePos + numBytes] & 0xFFL));
      jopencsd.uint8_t_Array_setitem(byteBuffer, numBytes, (short)(memFileContent[memFilePos + numBytes] & 0xFF));
    }
    
    System.out.println("returning numBytes = " + numBytes);
    return numBytes;
  }

}