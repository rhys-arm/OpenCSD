#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "opencsd_c_api.h"
#include "trc_gen_elem.h"


using namespace std;

const uint32_t NO_FORMATTER_FLAGS = 0;

vector<uint8_t> program_image_buffer;

vector<uint8_t> readFileIntoBuffer(string fileName) {
    ifstream file(fileName, ios::binary | ios::ate);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer_c(size);
    if (!file.read(buffer_c.data(), size)) {
        throw runtime_error("Could not open file: " + fileName);
    }
    vector<uint8_t> buffer;
    for (char c : buffer_c) {
        buffer.push_back(c);
    }
    return buffer;
}

uint32_t my_mem_acc_function(const void *p_context, const ocsd_vaddr_t address,
        const ocsd_mem_space_acc_t mem_space, const uint8_t trcID, const uint32_t reqBytes, uint8_t *byteBuffer)
{
    uint32_t numBytes = 0;
    for (numBytes = 0; numBytes < reqBytes, numBytes < program_image_buffer.size(); numBytes++) {
        *(byteBuffer + numBytes) = *(program_image_buffer.data() + (address - 0x80000000) + numBytes);
    }
    return numBytes;
}

ocsd_datapath_resp_t my_decoder_output_processor(const void *p_context, 
                                                 const ocsd_trc_index_t index_sop, 
                                                 const uint8_t trc_chan_id, 
                                                 const ocsd_generic_trace_elem *elem) {
    std::ostringstream oss;
    oss << "Idx:" << index_sop << "; ID:" << std::hex << (uint32_t)trc_chan_id << "; ";
    std::string elemStr = ocsd_generic_trace_elem_to_string(elem);
    oss << elemStr;
    cout << oss.str() << endl;
    return OCSD_RESP_CONT;
}

int main() {
    cout << "Start" << endl;

    program_image_buffer = readFileIntoBuffer("../simple_juno_trace/juno_snapshot/mem_Cortex-A53_0_0_EXEC.bin");

    dcd_tree_handle_t dcdtree_handle = ocsd_create_dcd_tree(OCSD_TRC_SRC_SINGLE, NO_FORMATTER_FLAGS);

    ocsd_etmv4_cfg config = {};

    // Setup config from programmer ETM register values
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
    const char* decoderName = OCSD_BUILTIN_DCD_ETMV4I;  // use built in ETMv4 instruction decoder.
    void *p_context = NULL; // <some_client_context>
    uint8_t CSID = 0;
    
    // Setup logging
    ocsd_err_t ret = ocsd_def_errlog_init(OCSD_ERR_SEV_INFO, 1);
    if (ret != OCSD_OK)
        return ret;
    
    ret = ocsd_def_errlog_config_output(C_API_MSGLOGOUT_FLG_STDOUT, 0);
    if (ret != OCSD_OK)
        return ret;

    // Create decoder
    ret = ocsd_dt_create_decoder(dcdtree_handle, decoderName, OCSD_CREATE_FLG_FULL_DECODER, (void *)&config, &CSID);
    if (ret != OCSD_OK)
        return ret;
    
    const ocsd_vaddr_t st_address = 0x80000000;
    const ocsd_vaddr_t en_address = 0x80000033;
    ret = ocsd_dt_add_callback_trcid_mem_acc(dcdtree_handle, st_address, en_address, OCSD_MEM_SPACE_ANY, my_mem_acc_function, p_context);
    if (ret != OCSD_OK) {
        return ret;
    }
    
    ret = ocsd_dt_set_gen_elem_outfn(dcdtree_handle, my_decoder_output_processor, 0);
    if (ret != OCSD_OK)
        return ret;
    
    // Push data into the decoder
    vector<uint8_t> etm_data_buffer = readFileIntoBuffer("../simple_juno_trace/etm_dump/ETM_0_6_0.bin");
    ocsd_datapath_resp_t dataPathResp = OCSD_RESP_CONT;
    uint32_t trace_index = 0;
    uint32_t nUsedThisTime = 0;
    uint32_t total_used = 0;

    for (; total_used < etm_data_buffer.size(); total_used += nUsedThisTime) {
        const uint32_t dataBlockSize = etm_data_buffer.size() - total_used;
        const uint8_t *pDataBlock = ((uint8_t *) etm_data_buffer.data()) + total_used;
        nUsedThisTime = 0;
        ocsd_datapath_resp_t dataPathResp = ocsd_dt_process_data(dcdtree_handle,
                                                                 OCSD_OP_DATA,
                                                                 trace_index,
                                                                 dataBlockSize,
                                                                 pDataBlock,
                                                                 &nUsedThisTime);
        cout << "dataPathResp = " << dataPathResp << endl;
        if (dataPathResp != OCSD_RESP_CONT) {
            throw runtime_error("Bad dataPathResp: " + dataPathResp);
        }
    }

    if (total_used != etm_data_buffer.size()) {
        throw runtime_error("Not all ETM data was processed");
    }

    // Shutdown
    ret = ocsd_dt_remove_decoder(dcdtree_handle, CSID);
    if (ret != OCSD_OK)
        return ret;

    ocsd_destroy_dcd_tree(dcdtree_handle);
}
