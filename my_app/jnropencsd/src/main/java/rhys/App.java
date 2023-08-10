package rhys;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;

import jnr.ffi.*;
import jnr.ffi.Runtime;
import jnr.ffi.annotations.Delegate;
import jnr.ffi.byref.IntByReference ;
import jnr.ffi.util.EnumMapper;
import rhys.App.ocsd_mem_space_acc_t;

public class App {
// typedef uint32_t (* Fn_MemAccID_CB)(const void *p_context, const ocsd_vaddr_t address, const ocsd_mem_space_acc_t mem_space, const uint8_t trcID,
// const uint32_t reqBytes, uint8_t *byteBuffer);
    public interface Fn_MemAccID_CB {
        @Delegate
        long invoke(Pointer p_context, long address, ocsd_mem_space_acc_t mem_space, short trcID, long reqBytes, Pointer byteBuffer);
    }
    public static class Fn_MemAccID_CB_imp implements Fn_MemAccID_CB {
        @Delegate
        public long invoke(Pointer p_context, long address, ocsd_mem_space_acc_t mem_space, short trcID, long reqBytes, Pointer byteBuffer) {
            System.out.println("Hello from Fn_MemAccID_CB_imp");
            System.out.println("address = " + address);
            System.out.println("mem_space = " + mem_space);
            System.out.println("trcID = " + trcID);
            System.out.println("reqBytes = " + reqBytes);
            System.out.println("byteBuffer = " + byteBuffer);

            File memFile = new File("../simple_juno_trace/juno_snapshot/mem_Cortex-A53_0_0_EXEC.bin");
            byte[] memFileContent;
            try {
              memFileContent = Files.readAllBytes(memFile.toPath());
            } catch (IOException e) {
              e.printStackTrace();
              return 0;
            }
      
          int memFilePos = java.lang.Math.toIntExact(address - 0x80000000L);
          System.out.println("memFilePos = " + memFilePos);
      
          int numBytes = 0;
          for (numBytes = 0; numBytes < reqBytes && numBytes < memFileContent.length; numBytes++) {
            //System.out.println("writing 0x" + Long.toHexString(memFileContent[memFilePos + numBytes] & 0xFFL));
            //jopencsd.uint8_t_Array_setitem(byteBuffer, numBytes, (short)(memFileContent[memFilePos + numBytes] & 0xFF));
            byteBuffer.setMemory(numBytes, 1, (byte)(memFileContent[memFilePos + numBytes] & 0xFF));
          }
          
          System.out.println("returning numBytes = " + numBytes);
          return numBytes;
        }
    }

    public enum ocsd_datapath_resp_t {
        OCSD_RESP_CONT, OCSD_RESP_WARN_CONT, OCSD_RESP_ERR_CONT, OCSD_RESP_WAIT, OCSD_RESP_WARN_WAIT, OCSD_RESP_ERR_WAIT, OCSD_RESP_FATAL_NOT_INIT,
        OCSD_RESP_FATAL_INVALID_OP, OCSD_RESP_FATAL_INVALID_PARAM, OCSD_RESP_FATAL_INVALID_DATA, OCSD_RESP_FATAL_SYS_ERR
    };
// typedef ocsd_datapath_resp_t (* FnTraceElemIn)( const void *p_context, 
//                                                 const ocsd_trc_index_t index_sop, 
//                                                 const uint8_t trc_chan_id, 
//                                                 const ocsd_generic_trace_elem *elem); 

// typedef struct _ocsd_generic_trace_elem {
//     ocsd_gen_trc_elem_t elem_type;   /**< Element type - remaining data interpreted according to this value */
//     ocsd_isa           isa;          /**< instruction set for executed instructions */
//     ocsd_vaddr_t       st_addr;      /**< start address for instruction execution range / inaccessible code address / data address */
//     ocsd_vaddr_t       en_addr;        /**< end address (exclusive) for instruction execution range. */
//     ocsd_pe_context    context;        /**< PE Context */
//     uint64_t           timestamp;      /**< timestamp value for TS element type */
//     uint32_t           cycle_count;    /**< cycle count for explicit cycle count element, or count for element with associated cycle count */
//     ocsd_instr_type    last_i_type;    /**< Last instruction type if instruction execution range */
//     ocsd_instr_subtype last_i_subtype; /**< sub type for last instruction in range */
 
//     //! per element flags
//     union {
//         struct {
//             uint32_t last_instr_exec:1;     /**< 1 if last instruction in range was executed; */
//             uint32_t last_instr_sz:3;       /**< size of last instruction in bytes (2/4) */
//             uint32_t has_cc:1;              /**< 1 if this packet has a valid cycle count included (e.g. cycle count included as part of instruction range packet, always 1 for pure cycle count packet.*/
//             uint32_t cpu_freq_change:1;     /**< 1 if this packet indicates a change in CPU frequency */
//             uint32_t excep_ret_addr:1;      /**< 1 if en_addr is the preferred exception return address on exception packet type */
//             uint32_t excep_data_marker:1;   /**< 1 if the exception entry packet is a data push marker only, with no address information (used typically in v7M trace for marking data pushed onto stack) */
//             uint32_t extended_data:1;       /**< 1 if the packet extended data pointer is valid. Allows packet extensions for custom decoders, or additional data payloads for data trace.  */
//             uint32_t has_ts:1;              /**< 1 if the packet has an associated timestamp - e.g. SW/STM trace TS+Payload as a single packet */
//             uint32_t last_instr_cond:1;     /**< 1 if the last instruction was conditional */
//             uint32_t excep_ret_addr_br_tgt:1;   /**< 1 if exception return address (en_addr) is also the target of a taken branch addr from the previous range. */
//         };
//         uint32_t flag_bits;
//     };

//     //! packet specific payloads
//     union {  
//         uint32_t exception_number;          /**< exception number for exception type packets */
//         trace_event_t  trace_event;         /**< Trace event - trigger etc      */
//         trace_on_reason_t trace_on_reason;  /**< reason for the trace on packet */
//         ocsd_swt_info_t sw_trace_info;      /**< software trace packet info    */
// 		uint32_t num_instr_range;	        /**< number of instructions covered by range packet (for T32 this cannot be calculated from en-st/i_size) */
//         unsync_info_t unsync_eot_info;      /**< additional information for unsync / end-of-trace packets. */
//         trace_marker_payload_t sync_marker; /**< marker element - sync later element to position in stream */
//         trace_memtrans_t mem_trans;         /**< memory transaction packet - transaction event */
//         trace_sw_ite_t sw_ite;              /**< PE sw instrumentation using FEAT_ITE */
//     };

//     const void *ptr_extended_data;        /**< pointer to extended data buffer (data trace, sw trace payload) / custom structure */

// } ocsd_generic_trace_elem;

// typedef struct _ocsd_generic_trace_elem_simple {
//     ocsd_vaddr_t       st_addr;      /**< start address for instruction execution range / inaccessible code address / data address */
//     ocsd_vaddr_t       en_addr;        /**< end address (exclusive) for instruction execution range. */

// } ocsd_generic_trace_elem_simple;

// public static class ocsd_generic_trace_elem_simple extends Struct {
//     public Struct.Unsigned64 st_addr = new Struct.Unsigned64();
//     public Struct.Unsigned64 en_addr = new Struct.Unsigned64();

//     public ocsd_generic_trace_elem_simple(jnr.ffi.Runtime runtime) {
//         super(runtime);
//     }
//   }


    public interface FnTraceElemIn_CB {
        @Delegate
        ocsd_datapath_resp_t invoke(Pointer p_context, int index_sop, short trc_chan_id, Pointer elem);
    }
    public static class FnTraceElemIn_CB_imp implements FnTraceElemIn_CB {
        @Delegate
        public ocsd_datapath_resp_t invoke(Pointer p_context, int index_sop, short trc_chan_id, Pointer elem) {
            System.out.println("Hello from FnTraceElemIn_CB_imp");
            System.out.println("index_sop = " + index_sop);
            System.out.println("trc_chan_id = " + trc_chan_id);
            long offset = 0;
            System.out.println(" ------------> 0x" + Long.toHexString(elem.getInt(offset)));
            offset += Integer.BYTES;
            System.out.println(" ------------> 0x" + Long.toHexString(elem.getInt(offset)));
            offset += Integer.BYTES;
            System.out.println(" ------------> 0x" + Long.toHexString(elem.getLong(offset)));
            offset += Long.BYTES;
            System.out.println(" ------------> 0x" + Long.toHexString(elem.getLong(offset)));
            offset += Long.BYTES;

            return ocsd_datapath_resp_t.OCSD_RESP_CONT;
        }
    }
    
    
    enum ocsd_dcd_tree_src_t {OCSD_TRC_SRC_FRAME_FORMATTED, OCSD_TRC_SRC_SINGLE};
    enum ocsd_arch_version_t {ARCH_UNKNOWN, ARCH_CUSTOM, ARCH_V7, ARCH_V8, ARCH_V8r3, ARCH_AA64, ARCH_V8_max};
    enum ocsd_core_profile_t {profile_Unknown, profile_CortexM, profile_CortexR, profile_CortexA, profile_Custom};
    enum ocsd_datapath_op_t {OCSD_OP_DATA, OCSD_OP_EOT, OCSD_OP_FLUSH, OCSD_OP_RESET};
    public static enum ocsd_mem_space_acc_t implements EnumMapper.IntegerEnum {
        OCSD_MEM_SPACE_EL1S(0x1), OCSD_MEM_SPACE_EL1N(0x2), OCSD_MEM_SPACE_EL2(0x4), OCSD_MEM_SPACE_EL3(0x8),
        OCSD_MEM_SPACE_EL2S(0x10), OCSD_MEM_SPACE_S(0x19), OCSD_MEM_SPACE_N(0x6), OCSD_MEM_SPACE_ANY(0x1F);

        private final int value;

        private ocsd_mem_space_acc_t(int value) {this.value = value;}

        @Override
        public int intValue() {return value;}
    }
    
    
    public interface JNROpenCSD {
        long ocsd_create_dcd_tree(int src_type, int deformatterCfgFlags);
        int ocsd_dt_create_decoder(long handle,
                                             String decoder_name,
                                             int create_flags,
                                             ocsd_etmv4_cfg cfg,
                                             IntByReference pCSID
                                             );
                                             // OCSD_C_API ocsd_err_t ocsd_dt_add_callback_trcid_mem_acc(const dcd_tree_handle_t handle, const ocsd_vaddr_t st_address, const ocsd_vaddr_t en_address,
                                             // const ocsd_mem_space_acc_t mem_space, Fn_MemAccID_CB p_cb_func, const void *p_context);
        int ocsd_dt_add_callback_trcid_mem_acc(long handle, long st_address, long en_address, ocsd_mem_space_acc_t mem_space, Fn_MemAccID_CB callback, Pointer p_context);
        //OCSD_C_API void ocsd_destroy_dcd_tree(const dcd_tree_handle_t handle);
        void ocsd_destroy_dcd_tree(long handle);
        //OCSD_C_API ocsd_err_t ocsd_dt_set_gen_elem_outfn(const dcd_tree_handle_t handle, FnTraceElemIn pFn, const void *p_context);
        int ocsd_dt_set_gen_elem_outfn(long handle, FnTraceElemIn_CB pFn, Pointer p_context);
        // OCSD_C_API ocsd_datapath_resp_t ocsd_dt_process_data(const dcd_tree_handle_t handle,
        //                                     const ocsd_datapath_op_t op,
        //                                     const ocsd_trc_index_t index,
        //                                     const uint32_t dataBlockSize,
        //                                     const uint8_t *pDataBlock,
        //                                     uint32_t *numBytesProcessed);
        ocsd_datapath_resp_t ocsd_dt_process_data(long handle, ocsd_datapath_op_t op, int index, long dataBlockSize, ByteBuffer pDataBlock, IntByReference numBytesProcessed);
    }

//     typedef struct _ocsd_etmv4_cfg 
// {
//     uint32_t                reg_idr0;    /**< ID0 register */
//     uint32_t                reg_idr1;    /**< ID1 register */
//     uint32_t                reg_idr2;    /**< ID2 register */
//     uint32_t                reg_idr8;
//     uint32_t                reg_idr9;   
//     uint32_t                reg_idr10;
//     uint32_t                reg_idr11;
//     uint32_t                reg_idr12;
//     uint32_t                reg_idr13;
//     uint32_t                reg_configr;  /**< Config Register */
//     uint32_t                reg_traceidr;  /**< Trace Stream ID register */
//     ocsd_arch_version_t    arch_ver;   /**< Architecture version */
//     ocsd_core_profile_t    core_prof;  /**< Core Profile */
// } ocsd_etmv4_cfg;

public static class ocsd_etmv4_cfg extends Struct {
    public Struct.Unsigned32 reg_idr0 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr1 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr2 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr8 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr9 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr10 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr11 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr12 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_idr13 = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_configr = new Struct.Unsigned32();
    public Struct.Unsigned32 reg_traceidr = new Struct.Unsigned32();
    public Struct.Unsigned32 arch_ver = new Struct.Unsigned32();
    public Struct.Unsigned32 core_prof = new Struct.Unsigned32();

    public ocsd_etmv4_cfg(jnr.ffi.Runtime runtime) {
        super(runtime);
    }
  }

    public static final int NO_FORMATTER_FLAGS = 0;

    public static void main(String[] args) throws IOException {
        JNROpenCSD jnrOpenCSD = LibraryLoader.create(JNROpenCSD.class).load("opencsd_c_api");
        Runtime runtime = Runtime.getRuntime(jnrOpenCSD);

        //SWIGTYPE_p_void handle = jopencsd.ocsd_create_dcd_tree(ocsd_dcd_tree_src_t.OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);
        long handle = jnrOpenCSD.ocsd_create_dcd_tree(ocsd_dcd_tree_src_t.OCSD_TRC_SRC_SINGLE.ordinal(), NO_FORMATTER_FLAGS);
        System.out.println("handle = 0x" + Long.toHexString(handle));

        ocsd_etmv4_cfg cfg = new ocsd_etmv4_cfg(runtime);
        cfg.reg_idr0.set(0x28000EA1L);
        cfg.reg_idr1.set(0x4100F403);
        cfg.reg_idr2.set(0x00000488);
        cfg.reg_configr.set(0x00000001);
        cfg.reg_traceidr.set(0x00000006);
        cfg.arch_ver.set(ocsd_arch_version_t.ARCH_V8.ordinal());
        cfg.core_prof.set(ocsd_core_profile_t.profile_CortexA.ordinal());

        //Pointer cfg_void = Pointer.newIntPointer(runtime, 0);
        IntByReference CSID = new IntByReference();

        int ret = jnrOpenCSD.ocsd_dt_create_decoder(handle, "ETMV4I", 0x02,
                                  cfg, CSID);
        System.out.println("ret = " + ret);
        System.out.println("CSID = " + CSID.intValue());

        ObjectReferenceManager referenceManager = runtime.newObjectReferenceManager();
        Fn_MemAccID_CB memAccID_CB_imp = new Fn_MemAccID_CB_imp();
        Pointer key = referenceManager.add(memAccID_CB_imp);
        Pointer p_context = Pointer.newIntPointer(runtime, 0);
        ret = jnrOpenCSD.ocsd_dt_add_callback_trcid_mem_acc(handle, 0x80000000L, 0x80000023L,
            ocsd_mem_space_acc_t.OCSD_MEM_SPACE_ANY, memAccID_CB_imp, p_context);
        System.out.println("ret = " + ret);

        ObjectReferenceManager referenceManager2 = runtime.newObjectReferenceManager();
        FnTraceElemIn_CB fnTraceElemIn_CB = new FnTraceElemIn_CB_imp();
        Pointer key2 = referenceManager2.add(fnTraceElemIn_CB);
        ret = jnrOpenCSD.ocsd_dt_set_gen_elem_outfn(handle, fnTraceElemIn_CB, p_context);
        System.out.println("ret = " + ret);

      File etmFile = new File("../simple_juno_trace/etm_dump/ETM_0_6_0.bin");
      byte[] etmFileContent = Files.readAllBytes(etmFile.toPath());
      ByteBuffer etmFileContentBB = ByteBuffer.allocateDirect(etmFileContent.length);
      etmFileContentBB.put(etmFileContent);
      etmFileContentBB.flip();
      ocsd_datapath_resp_t dataPathResp = ocsd_datapath_resp_t.OCSD_RESP_CONT;
      int trace_index = 0;
      IntByReference numBytesProcessed = new IntByReference(0);
      System.out.println("etmFileContent.length = " + etmFileContent.length);
      // ocsd_datapath_resp_t ocsd_dt_process_data(long handle, ocsd_datapath_op_t op, int index, long dataBlockSize, Pointer pDataBlock, IntByReference numBytesProcessed);
      dataPathResp = jnrOpenCSD.ocsd_dt_process_data(handle, ocsd_datapath_op_t.OCSD_OP_DATA, trace_index, etmFileContent.length, etmFileContentBB, numBytesProcessed);

      System.out.println("dataPathResp = " + dataPathResp);
      System.out.println("Number of bytes processed = " + numBytesProcessed.intValue());

        System.out.println("ocsd_destroy_dcd_tree");
        jnrOpenCSD.ocsd_destroy_dcd_tree(handle);
        System.out.println("referenceManager2.remove(key2)");
        referenceManager2.remove(key2);
        System.out.println("referenceManager.remove(key)");
        referenceManager.remove(key);
        System.out.println("done");
    }
}
