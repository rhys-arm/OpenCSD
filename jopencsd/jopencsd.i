%module(directors="1") jopencsd

// We want proper Java enums
%include "enums.swg"
// We want constants translated directly into Java code
%javaconst(1);
// except for this awkward one
%javaconst(0) OCSD_VA_MASK;

%include "std_string.i"
%include "stdint.i"

%{
#include "trc_pkt_types_etmv4.h"

#include "opencsd_c_api.h"

#include "DecodedTraceCallback.h"

static DecodedTraceCallback *decodedTraceCallbackHandler_ptr = NULL;

static ocsd_datapath_resp_t my_decoder_output_processor_helper(const void *p_context, 
                                                 const ocsd_trc_index_t index_sop, 
                                                 const uint8_t trc_chan_id, 
                                                 const ocsd_generic_trace_elem *elem) {
  // Make the call up to the target language when decodedTraceCallbackHandler_ptr
  // is an instance of a target language director class
  return decodedTraceCallbackHandler_ptr->my_decoder_output_processor(p_context, 
                                                 index_sop, 
                                                 trc_chan_id, 
                                                 elem);
}
%}

%inline %{
OCSD_C_API ocsd_err_t ocsd_dt_set_gen_elem_outfn_wrapper(const dcd_tree_handle_t handle, DecodedTraceCallback *decodedTraceCallbackHandler, const void *p_context) {
  decodedTraceCallbackHandler_ptr = decodedTraceCallbackHandler;
  ocsd_err_t result = ocsd_dt_set_gen_elem_outfn(handle, &my_decoder_output_processor_helper, p_context);
  decodedTraceCallbackHandler = NULL;
  return result;
}
%}


%{

#include "MemAccCallback.h"
static MemAccCallback *memAccCallbackHandler_ptr = NULL;

uint32_t my_mem_acc_function_helper(const void *p_context, const ocsd_vaddr_t address,
        const ocsd_mem_space_acc_t mem_space, const uint8_t trcID, const uint32_t reqBytes, uint8_t *byteBuffer)
{
    return memAccCallbackHandler_ptr->my_mem_acc_function(p_context, address, mem_space, trcID, reqBytes, byteBuffer);
}

%}

%inline %{
OCSD_C_API ocsd_err_t ocsd_dt_add_callback_trcid_mem_acc_wrapper(const dcd_tree_handle_t handle, const ocsd_vaddr_t st_address, const ocsd_vaddr_t en_address,
                          const ocsd_mem_space_acc_t mem_space, MemAccCallback *memAccCallbackHandler, const void *p_context) {
  memAccCallbackHandler_ptr = memAccCallbackHandler;
  ocsd_err_t result = ocsd_dt_add_callback_trcid_mem_acc(handle, st_address, en_address, mem_space, &my_mem_acc_function_helper, p_context);
  memAccCallbackHandler = NULL;
  return result;
}
%}







// %inline %{
// std::string ocsd_generic_trace_elem_to_string(const ocsd_generic_trace_elem* elem);
// %}

%include cpointer.i
%include various.i

%typemap(in)        (const uint32_t dataBlockSize, const uint8_t *pDataBlock) {
  $1 = (int)JCALL1(GetDirectBufferCapacity, jenv, $input); 
  $2 = (uint8_t*)JCALL1(GetDirectBufferAddress, jenv, $input);
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */ 
%typemap(jni)       (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "jobject" 
%typemap(jtype)     (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "java.nio.ByteBuffer" 
%typemap(jstype)    (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "java.nio.ByteBuffer" 
%typemap(javain)    (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "$javainput" 
%typemap(javaout)   (const uint32_t dataBlockSize, const uint8_t *pDataBlock) { 
    return $jnicall; 
}

%include "ocsd_if_types.h"
%include "trc_pkt_types.h"
%include "trc_pkt_types_etmv3.h"
%include "trc_pkt_types_etmv4.h"

%include "ocsd_c_api_custom.h"
%include "ocsd_c_api_types.h"
%include "opencsd_c_api.h"

%include "trc_gen_elem_types.h"

%pointer_cast(const ocsd_etmv4_cfg *, const void *, ocsd_etmv4_cfg_to_void);
%pointer_cast(const ocsd_etmv3_cfg *, const void *, ocsd_etmv3_cfg_to_void);
%pointer_functions(unsigned char, unsigned_char_ptr);
%pointer_functions(uint32_t, uint32_t_ptr);

%feature("director") DecodedTraceCallback;
%include "DecodedTraceCallback.h"
%feature("director") MemAccCallback;
%include "MemAccCallback.h"

%include "carrays.i"
%array_functions(uint8_t, uint8_t_Array);

%inline %{
int64_t ocsd_generic_trace_elem_st_addr_as_long(const ocsd_generic_trace_elem* elem)
{
  return elem->st_addr;
}
%}
