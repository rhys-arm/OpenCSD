#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

//#include "ocsd_dcd_tree.h"
//#include "ocsd_c_api_types.h"
#include "opencsd_c_api.h"
#include "trc_gen_elem.h"


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

    // typedef ocsd_datapath_resp_t (* FnTraceElemIn)( const void *p_context, 
    //                                             const ocsd_trc_index_t index_sop, 
    //                                             const uint8_t trc_chan_id, 
    //                                             const ocsd_generic_trace_elem *elem); 

uint32_t my_mem_acc_function(const void *p_context, const ocsd_vaddr_t address,
        const ocsd_mem_space_acc_t mem_space, const uint8_t trcID, const uint32_t reqBytes, uint8_t *byteBuffer)
{
    cout << "my_mem_acc_function" << endl;
    cout << "address = 0x" << hex << address << dec << endl;
    cout << "mem_space = " << mem_space << endl;
    cout << "trcID = 0x" << hex << static_cast<int>(trcID) << dec << endl;
    cout << "reqBytes = " << reqBytes << endl;

    FILE* fp = fopen("simple_juno_trace/updated_juno_snapshot/mem_Cortex-A53_0_0_EXEC.bin", "rb");
    uint32_t program_image_size = 0;
    size_t bytes_read = 0;
    if (!fp)
        return OCSD_ERR_FILE_ERROR;
    fseek(fp, 0, SEEK_END);
    program_image_size = ftell(fp);
    uint8_t* program_image_buffer = new (std::nothrow) uint8_t[program_image_size];
    if (!program_image_buffer) {
        fclose(fp);
        return OCSD_ERR_MEM;
    }
    rewind(fp);
    bytes_read = fread(program_image_buffer, 1, program_image_size, fp);
    fclose(fp);
    if (bytes_read < (size_t)program_image_size)
        return OCSD_ERR_FILE_ERROR;
    
    uint32_t numBytes = 0;
    for (numBytes = 0; numBytes < reqBytes, numBytes < program_image_size; numBytes++) {
        *(byteBuffer + numBytes) = *(program_image_buffer + (address - 0x80000000) + numBytes);
    }

    delete [] program_image_buffer;

    cout << "return numBytes = " << numBytes << endl;
    return numBytes;
}

ocsd_datapath_resp_t my_decoder_output_processor(const void *p_context, 
                                                 const ocsd_trc_index_t index_sop, 
                                                 const uint8_t trc_chan_id, 
                                                 const ocsd_generic_trace_elem *elem) {
    std::ostringstream oss;
    oss << "Idx:" << index_sop << "; ID:" << std::hex << (uint32_t)trc_chan_id << "; ";
    std::string elemStr = ocsd_generic_trace_elem_to_string(elem);
    oss << elemStr << std::endl;
    cout << oss.str() << endl;
    return OCSD_RESP_CONT;
}

int main() {
    cout << "Start" << endl;

    dcd_tree_handle_t dcdtree_handle = ocsd_create_dcd_tree(OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);

    ocsd_etmv4_cfg config = {};

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
    uint8_t CSID = 0;

    ocsd_err_t ret = ocsd_dt_create_decoder(dcdtree_handle, decoderName, OCSD_CREATE_FLG_FULL_DECODER, (void *)&config, &CSID);
    if (ret != OCSD_OK)
        return ret;
    cout << "CSID = " << (int)CSID << endl;
    

    // ret = ocsd_dt_add_buffer_mem_acc(dcdtree_handle, 0x80000000, OCSD_MEM_SPACE_ANY, program_image_buffer, program_image_size);
    // if (ret != OCSD_OK)
    //     return ret;
    
    // OCSD_C_API ocsd_err_t ocsd_dt_add_callback_trcid_mem_acc(const dcd_tree_handle_t handle, const ocsd_vaddr_t st_address,
    // const ocsd_vaddr_t en_address, const ocsd_mem_space_acc_t mem_space, Fn_MemAccID_CB p_cb_func, const void *p_context);
    const ocsd_vaddr_t st_address = 0x80000000;
    const ocsd_vaddr_t en_address = 0x80000033;
    ret = ocsd_dt_add_callback_trcid_mem_acc(dcdtree_handle, st_address, en_address, OCSD_MEM_SPACE_ANY, my_mem_acc_function, p_context);
    if (ret != OCSD_OK) {
        cerr << "ret = " << ret << endl;
        return ret;
    }
    
    ret = ocsd_dt_set_gen_elem_outfn(dcdtree_handle, my_decoder_output_processor, 0);
    if (ret != OCSD_OK)
        return ret;

    ocsd_datapath_resp_t dataPathResp = OCSD_RESP_CONT;
    uint32_t trace_index = 0;

    std::string filepath = "simple_juno_trace/updated_juno_snapshot_trace/ETM_0_6_0.bin";
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        cerr << "Could not open file: " << filepath << endl;
        return -1;
    }
    std::streamsize size = file.tellg();
    cout << "ETM trace size = " << size << endl;
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        cerr << "Failed to read file" << endl;
        return -1;
    }

    uint8_t* etmBufPos = (uint8_t*)buffer.data();
    const uint32_t chunkSize = 8;
    uint32_t dataThisIteration = 0;
    uint32_t nUsedThisTime = 0;
    uint32_t etmDataRemaining = size;
    while (etmDataRemaining > 0) {
        dataThisIteration = std::min(chunkSize, etmDataRemaining);
        cout << "\n> dataThisIteration = " << dataThisIteration << endl;
        dataPathResp = ocsd_dt_process_data(dcdtree_handle,
                                    OCSD_OP_DATA,
                                    trace_index,
                                    dataThisIteration,
                                    etmBufPos,
                                    &nUsedThisTime);

        etmBufPos += nUsedThisTime;
        etmDataRemaining -= nUsedThisTime;
        cout << " -> dataPathResp = " << ocsd_datapath_resp_t_as_string(dataPathResp) << endl;
        if (dataPathResp != OCSD_RESP_CONT) {
            cerr << " -----> Exiting early as dataPathResp != OCSD_RESP_CONT" << endl;
            break;
        }
    }

    cout << "etmDataRemaining = " << etmDataRemaining << endl;


    // OCSD_C_API ocsd_datapath_resp_t ocsd_dt_process_data(const dcd_tree_handle_t handle,
    //                                             const ocsd_datapath_op_t op,
    //                                             const ocsd_trc_index_t index,
    //                                             const uint32_t dataBlockSize,
    //                                             const uint8_t *pDataBlock,
    //                                             uint32_t *numBytesProcessed);




    // may not be required, given we're subsequently calling ocsd_destroy_dcd_tree
    ret = ocsd_dt_remove_decoder(dcdtree_handle, CSID);
    if (ret != OCSD_OK)
        return ret;

    ocsd_destroy_dcd_tree(dcdtree_handle);
    // DecodeTree::DestroyDecodeTree(pTree);

    cout << "End" << endl;
    cout << endl;
}