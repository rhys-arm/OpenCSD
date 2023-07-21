%module(directors="1") jopencsd
%{
#include "trc_pkt_types_etmv4.h"

#include "opencsd_c_api.h"

#include "Callback.h"

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

%typemap(in)        (const uint8_t *p_mem_buffer, const uint32_t mem_length) {
  $1 = JCALL1(GetDirectBufferAddress, jenv, $input); 
  $2 = (int)JCALL1(GetDirectBufferCapacity, jenv, $input); 
}

%typemap(in)        (const uint32_t dataBlockSize, const uint8_t *pDataBlock) {
  $1 = (int)JCALL1(GetDirectBufferCapacity, jenv, $input); 
  $2 = JCALL1(GetDirectBufferAddress, jenv, $input); 
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */ 
%typemap(jni)       (const uint8_t *p_mem_buffer, const uint32_t mem_length), (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "jobject" 
%typemap(jtype)     (const uint8_t *p_mem_buffer, const uint32_t mem_length), (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "java.nio.ByteBuffer" 
%typemap(jstype)    (const uint8_t *p_mem_buffer, const uint32_t mem_length), (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "java.nio.ByteBuffer" 
%typemap(javain)    (const uint8_t *p_mem_buffer, const uint32_t mem_length), (const uint32_t dataBlockSize, const uint8_t *pDataBlock) "$javainput" 
%typemap(javaout)   (const uint8_t *p_mem_buffer, const uint32_t mem_length), (const uint32_t dataBlockSize, const uint8_t *pDataBlock) { 
    return $jnicall; 
}

%include "ocsd_if_types.h"
%include "trc_pkt_types.h"
%include "trc_pkt_types_etmv4.h"

%include "ocsd_c_api_custom.h"
%include "ocsd_c_api_types.h"
%include "opencsd_c_api.h"

%include "trc_gen_elem_types.h"

%pointer_cast(const ocsd_etmv4_cfg *, const void *, ocsd_etmv4_cfg_to_void);
%pointer_functions(unsigned char, unsigned_char_ptr);
%pointer_functions(uint32_t, uint32_t_ptr);

%feature("director") Callback;
%include "Callback.h"
