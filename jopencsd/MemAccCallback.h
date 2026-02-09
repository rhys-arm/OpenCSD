#ifndef CALLBACK_2_SWIG
#define CALLBACK_2_SWIG

#include <cstdio>
#include <iostream>

class MemAccCallback {
public:
	virtual ~MemAccCallback() {}
    virtual uint32_t my_mem_acc_function(const void *p_context, const ocsd_vaddr_t address,
        const ocsd_mem_space_acc_t mem_space, const uint8_t trcID, const uint32_t reqBytes, uint8_t *byteBuffer) = 0;
};

#endif
