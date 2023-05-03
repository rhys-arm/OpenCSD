#include <iostream>

//#include "ocsd_dcd_tree.h"
//#include "ocsd_c_api_types.h"
#include "opencsd_c_api.h"


using namespace std;

const uint32_t NO_FORMATTER_FLAGS = 0;

// callback for the decoder output elements
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

int main() {
    cout << "Start" << endl;

    dcd_tree_handle_t dcdtree_handle = ocsd_create_dcd_tree(OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);

    ocsd_etmv4_cfg config = {};

    // TRCIDR0(id:0x78)=0x28000EA1
    // TRCIDR1(id:0x79)=0x4100F403
    // TRCIDR2(id:0x7A)=0x00000488
    // TRCIDR3(id:0x7B)=0x0D7B0004
    // TRCIDR4(id:0x7C)=0x11170004
    // TRCIDR5(id:0x7D)=0x28C7081E
    // TRCIDR6(id:0x7E)=0x00000000
    // TRCIDR7(id:0x7F)=0x00000000
    // TRCIDR8(id:0x60)=0x00000000
    // TRCIDR9(id:0x61)=0x00000000
    // TRCIDR10(id:0x62)=0x00000000
    // TRCIDR11(id:0x63)=0x00000000
    // TRCIDR12(id:0x64)=0x00000000
    // TRCIDR13(id:0x65)=0x00000000
    // TRCCONFIGR(id:0x4)=0x00000001
    // TRCCONFIGR(id:0x4)=0x00000001
    // TRCTRACEIDR(id:0x10)=0x00000006

    config.reg_idr0 = 0x28000EA1;    /**< ID0 register */
    config.reg_idr1 = 0x4100F403;    /**< ID1 register */
    config.reg_idr2 = 0x00000488;    /**< ID2 register */
    config.reg_idr8 = 0x00000000;
    config.reg_idr9 = 0x00000000;   
    config.reg_idr10 = 0x00000000;
    config.reg_idr11 = 0x00000000;
    config.reg_idr12 = 0x00000000;
    config.reg_idr13 = 0x00000000;
    config.reg_configr = 0x00000001;  /**< Config Register */
    config.reg_traceidr = 0x00000006;  /**< Trace Stream ID register */
    config.arch_ver = ARCH_V8;   /**< Architecture version */
    config.core_prof = profile_CortexA;  /**< Core Profile */

    
    // ...
    // code to fill in config from programmed registers and id registers
    // ...

    const char* decoderName = OCSD_BUILTIN_DCD_ETMV4I;  // use built in ETMv4 instruction decoder.
    int decoderCreateFlags = OCSD_CREATE_FLG_FULL_DECODER; // decoder type to create - OCSD_CREATE_FLG_PACKET_PROC for packet processor only
    void *p_context = NULL; // <some_client_context>
    ocsd_err_t err = create_generic_decoder(dcdtree_handle, decoderName, (void *)&config, p_context); 
    
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

    // ocsd_err_t ret = pTree->createMemAccMapper();   // the mapper is needed to add code images to.
    // if (ret != OCSD_OK)
    //     return ret;
    
    // FILE* fp = fopen("/Users/rhys/Programming/OpenCSD/my_app/simple_juno_trace/juno_snapshot/mem_Cortex-A53_0_0_EXEC.bin", "rb");
    // uint32_t program_image_size = 0;
    // size_t bytes_read = 0;
    // if (!fp)
    //     return OCSD_ERR_FILE_ERROR;
    // fseek(fp, 0, SEEK_END);
    // program_image_size = ftell(fp);
    // uint8_t* program_image_buffer = new (std::nothrow) uint8_t[program_image_size];
    // if (!program_image_buffer) {
    //     fclose(fp);
    //     return OCSD_ERR_MEM;
    // }
    // rewind(fp);
    // bytes_read = fread(program_image_buffer, 1, program_image_size, fp);
    // fclose(fp);
    // if (bytes_read < (size_t)program_image_size)
    //     return OCSD_ERR_FILE_ERROR;
    
    // ret = pTree->addBufferMemAcc(0x80000000, OCSD_MEM_SPACE_ANY, program_image_buffer, program_image_size);
    // if (ret != OCSD_OK)
    //     return ret;
    
    // /* finally we need to provide an output callback to recieve the decoded information */
    // DecoderOutputProcessor decoderOutputProcessor;
    // pTree->setGenTraceElemOutI(&decoderOutputProcessor);

    // ocsd_datapath_resp_t dataPathResp = OCSD_RESP_CONT;
    // uint32_t trace_index = 0;
    // uint32_t nUsedThisTime = 0;

    // string filepath = "/Users/rhys/Programming/OpenCSD/my_app/simple_juno_trace/etm_dump/ETM_0_6_0.bin";
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



    // DecodeTree::DestroyDecodeTree(pTree);

    cout << "End" << endl;
    cout << endl;
}
