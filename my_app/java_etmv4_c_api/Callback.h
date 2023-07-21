#ifndef CALLBACK_SWIG
#define CALLBACK_SWIG

#include <cstdio>
#include <iostream>

class Callback {
public:
	virtual ~Callback() { std::cout << "Callback::~Callback()" << std:: endl; }
	virtual ocsd_datapath_resp_t my_decoder_output_processor(const void *p_context, 
                                                 const ocsd_trc_index_t index_sop, 
                                                 const uint8_t trc_chan_id, 
                                                 const ocsd_generic_trace_elem *elem) = 0;
};

#endif
