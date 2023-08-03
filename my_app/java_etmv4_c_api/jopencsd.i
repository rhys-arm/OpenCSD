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


#include "Callback2.h"
static Callback2 *handler_ptr2 = NULL;

uint32_t my_mem_acc_function_helper(const void *p_context, const ocsd_vaddr_t address,
        const ocsd_mem_space_acc_t mem_space, const uint8_t trcID, const uint32_t reqBytes, uint8_t *byteBuffer)
{
    return handler_ptr2->my_mem_acc_function(p_context, address, mem_space, trcID, reqBytes, byteBuffer);
}

%}

%include "std_string.i"
%include "stdint.i"

%inline %{
std::string ocsd_generic_trace_elem_to_string(const ocsd_generic_trace_elem* elem);
%}


%inline %{
typedef struct _my_elem {
  ocsd_gen_trc_elem_t elem_type;
  ocsd_vaddr_t       st_addr;
  uint32_t num_instr_range;
  uint32_t last_instr_exec;
  uint32_t exception_number;
  ocsd_isa           isa;
  ocsd_pe_context    context;
} my_elem;

%}

%include "std_vector.i"
namespace std {
   %template(vectorelem) vector<my_elem>;
};


%inline %{
#include <vector>

std::vector<my_elem> my_elems_actual;
ocsd_isa           last_isa;
ocsd_pe_context    last_context;
%}


%inline %{

ocsd_datapath_resp_t my_C_decoder_output_processor(const void *p_context, 
                                                 const ocsd_trc_index_t index_sop, 
                                                 const uint8_t trc_chan_id, 
                                                 const ocsd_generic_trace_elem *elem) {

    const std::size_t MAX_SIZE = 100000;
    //const std::size_t MAX_SIZE = 1000;

    // std::cout << "Idx:" << index_sop << "; ID:" << std::hex << (uint32_t)trc_chan_id << "; "
    //     << ocsd_generic_trace_elem_to_string(elem) << std::endl << std::endl;
    
    std::size_t idx = my_elems_actual.size();
    my_elems_actual.push_back(my_elem());
    my_elems_actual[idx].elem_type = elem->elem_type;
    my_elems_actual[idx].st_addr = elem->st_addr;
    my_elems_actual[idx].num_instr_range = elem->num_instr_range;
    my_elems_actual[idx].last_instr_exec = elem->last_instr_exec;
    my_elems_actual[idx].exception_number = elem->exception_number;
    my_elems_actual[idx].isa = elem->isa;
    my_elems_actual[idx].context = elem->context;

    last_isa = elem->isa;
    last_context = elem->context;

    if (my_elems_actual.size() == MAX_SIZE) {
      return OCSD_RESP_WAIT;
    }

    return OCSD_RESP_CONT;
}

OCSD_C_API ocsd_err_t ocsd_dt_set_gen_elem_outfn_wrapper(const dcd_tree_handle_t handle, Callback *handler, const void *p_context) {
  //handler_ptr = handler;
  ocsd_err_t result = ocsd_dt_set_gen_elem_outfn(handle, my_C_decoder_output_processor, p_context);
  //handler = NULL;
  return result;
}
%}

%inline %{
OCSD_C_API ocsd_err_t ocsd_dt_add_callback_trcid_mem_acc_wrapper(const dcd_tree_handle_t handle, const ocsd_vaddr_t st_address, const ocsd_vaddr_t en_address,
                          const ocsd_mem_space_acc_t mem_space, Callback2 *handler2, const void *p_context) {
  handler_ptr2 = handler2;
  ocsd_err_t result = ocsd_dt_add_callback_trcid_mem_acc(handle, st_address, en_address, mem_space, &my_mem_acc_function_helper, p_context);
  handler2 = NULL;
  return result;
}
%}








%include cpointer.i
%include various.i

%typemap(in)        (const uint32_t dataBlockSize, const uint8_t *pDataBlock) {
  $1 = (int)JCALL1(GetDirectBufferCapacity, jenv, $input); 
  $2 = JCALL1(GetDirectBufferAddress, jenv, $input); 
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

%feature("director") Callback;
%include "Callback.h"
%feature("director") Callback2;
%include "Callback2.h"

%include "carrays.i"
%array_functions(uint8_t, uint8_t_Array);

%inline %{
int64_t ocsd_generic_trace_elem_st_addr_as_long(const ocsd_generic_trace_elem* elem)
{
  return elem->st_addr;
}

bool ocsd_generic_trace_elem_is_trace_on(const ocsd_generic_trace_elem* elem)
{return elem->elem_type == OCSD_GEN_TRC_ELEM_TRACE_ON;}
bool ocsd_generic_trace_elem_is_range(const ocsd_generic_trace_elem* elem)
{return elem->elem_type == OCSD_GEN_TRC_ELEM_INSTR_RANGE;}
bool ocsd_generic_trace_elem_is_pe_context(const ocsd_generic_trace_elem* elem)
{return elem->elem_type == OCSD_GEN_TRC_ELEM_PE_CONTEXT;}
bool ocsd_generic_trace_elem_is_exception(const ocsd_generic_trace_elem* elem)
{return elem->elem_type == OCSD_GEN_TRC_ELEM_EXCEPTION;}
%}
