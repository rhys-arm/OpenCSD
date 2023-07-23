%module(directors="1") jopencsd
%{
/*#include "ocsd_if_types.h"
#include "trc_pkt_types.h"*/
#include "trc_pkt_types_etmv4.h"

#include "opencsd_c_api.h"

// #include "ocsd_dcd_tree.h"

// #include "trc_cmp_cfg_etmv4.h"

// #include "ocsd_msg_logger.h"

// #include "ocsd_error_logger.h"

// #include "trc_gen_elem_in_i.h"

// void* cfg_struct_to_voidp(ocsd_etmv4_cfg* cfg)
// {
//     return (void*)cfg;
// }

#include "Callback.h"

// ocsd_datapath_resp_t my_decoder_output_processor(const void *p_context, 
//                                                  const ocsd_trc_index_t index_sop, 
//                                                  const uint8_t trc_chan_id, 
//                                                  const ocsd_generic_trace_elem *elem) {
//     // std::string elemStr;
//     // std::ostringstream oss;
//     // oss << "Idx:" << index_sop << "; ID:" << std::hex << (uint32_t)trc_chan_id << "; ";
//     // // elem.toString(elemStr);
//     // // oss << elemStr << std::endl;
//     // cout << oss.str() << endl;
//     printf("hello2: index_sop = %u\n", index_sop);
//     return OCSD_RESP_CONT;
// }

static Callback *handler_ptr = NULL;

static ocsd_datapath_resp_t my_decoder_output_processor_helper(const void *p_context, 
                                                 const ocsd_trc_index_t index_sop, 
                                                 const uint8_t trc_chan_id, 
                                                 const ocsd_generic_trace_elem *elem) {
  // Make the call up to the target language when handler_ptr
  // is an instance of a target language director class
  return handler_ptr->my_decoder_output_processor(p_context, 
                                                 index_sop, 
                                                 trc_chan_id, 
                                                 elem);
}

%}

%include "std_string.i"
%include "stdint.i"

%inline %{
OCSD_C_API ocsd_err_t ocsd_dt_set_gen_elem_outfn_wrapper(const dcd_tree_handle_t handle, Callback *handler, const void *p_context) {
  handler_ptr = handler;
  ocsd_err_t result = ocsd_dt_set_gen_elem_outfn(handle, &my_decoder_output_processor_helper, p_context);
  handler = NULL;
  return result;
}
%}

%inline %{
std::string ocsd_generic_trace_elem_to_string(const ocsd_generic_trace_elem* elem);
%}

%include cpointer.i
%include various.i
%apply char *BYTE {unsigned char* p_mem_buffer};
%apply char *BYTE {unsigned char* pDataBlock};

//%constant ocsd_datapath_resp_t (*HELLO)(const void *, const ocsd_trc_index_t, const uint8_t, const ocsd_generic_trace_elem*) = my_decoder_output_processor;

//%typemap(jstype) const void *decoder_cfg "Object"
// %apply int * { unsigned char *pCSID };

%include "ocsd_if_types.h"
%include "trc_pkt_types.h"
%include "trc_pkt_types_etmv4.h"

%include "ocsd_c_api_custom.h"
%include "ocsd_c_api_types.h"
%include "opencsd_c_api.h"

// %include "trc_data_raw_in_i.h"
// %include "ocsd_dcd_tree.h"

// %include "trc_cs_config.h"
// %include "trc_cmp_cfg_etmv4.h"

// %include "ocsd_msg_logger.h"

// %include "trc_error_log_i.h"
// %include "ocsd_error_logger.h"

%include "trc_gen_elem_types.h"
// %include "trc_printable_elem.h"
// %include "trc_gen_elem.h"
// %include "trc_gen_elem_in_i.h"

%pointer_cast(const ocsd_etmv4_cfg *, const void *, ocsd_etmv4_cfg_to_void);
%pointer_functions(unsigned char, unsigned_char_ptr);
%pointer_functions(uint32_t, uint32_t_ptr);

%feature("director") Callback;
%include "Callback.h"

//std::string ocsd_generic_trace_elem_to_string(const ocsd_generic_trace_elem* elem);

