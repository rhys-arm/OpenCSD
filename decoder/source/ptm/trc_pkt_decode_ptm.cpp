/*
 * \file       trc_pkt_decode_ptm.cpp
 * \brief      Reference CoreSight Trace Decoder : PTM packet decoder.
 * 
 * \copyright  Copyright (c) 2016, ARM Limited. All Rights Reserved.
 */

/* 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution. 
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors 
 * may be used to endorse or promote products derived from this software without 
 * specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */ 

#include <sstream>
#include "ptm/trc_pkt_decode_ptm.h"

#define DCD_NAME "DCD_PTM"

TrcPktDecodePtm::TrcPktDecodePtm()
    : TrcPktDecodeBase(DCD_NAME)
{
}

TrcPktDecodePtm::TrcPktDecodePtm(int instIDNum)
    : TrcPktDecodeBase(DCD_NAME,instIDNum)
{
}

TrcPktDecodePtm::~TrcPktDecodePtm()
{
}

/*********************** implementation packet decoding interface */

rctdl_datapath_resp_t TrcPktDecodePtm::processPacket()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    bool bPktDone = false;

    while(!bPktDone)
    {
        switch(m_curr_state)
        {
        case NO_SYNC:
            // no sync - output a no sync packet then transition to wait sync.
            m_output_elem.elem_type = RCTDL_GEN_TRC_ELEM_NO_SYNC;
            resp = outputTraceElement(m_output_elem);
            m_curr_state = (m_curr_packet_in->getType() == PTM_PKT_A_SYNC) ? WAIT_ISYNC : WAIT_SYNC;
            bPktDone = true;
            break;

        case WAIT_SYNC:
            if(m_curr_packet_in->getType() == PTM_PKT_A_SYNC)
                m_curr_state = WAIT_ISYNC;
            bPktDone = true;
            break;

        case WAIT_ISYNC:
            if(m_curr_packet_in->getType() == PTM_PKT_I_SYNC)
                m_curr_state = DECODE_PKTS;
            break;

        case DECODE_PKTS:
            resp = decodePacket();
            bPktDone = true;
            break;

        default:
             // should only see these after a _WAIT resp - in flush handler 
        case CONT_ISYNC: 
        case CONT_ATOM:
            bPktDone = true;
            // throw a decoder error
            break;
        }
    }
    return resp;
}

rctdl_datapath_resp_t TrcPktDecodePtm::onEOT()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    resp = contProcess();
    return resp;
}

rctdl_datapath_resp_t TrcPktDecodePtm::onReset()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    resetDecoder();
    return resp;
}

rctdl_datapath_resp_t TrcPktDecodePtm::onFlush()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    resp = contProcess();
    return resp;
}

// atom and isync packets can have multiple ouput packets that can be _WAITed mid stream.
rctdl_datapath_resp_t TrcPktDecodePtm::contProcess()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    switch(m_curr_state)
    {
    default: break; 
    case CONT_ISYNC:
        resp = processIsync();
        if(RCTDL_DATA_RESP_IS_CONT(resp))
            m_curr_state = DECODE_PKTS;
        break;

    case CONT_ATOM:
        resp = processAtom();
        if(RCTDL_DATA_RESP_IS_CONT(resp))
            m_curr_state = DECODE_PKTS;
        break;
    }
    return resp;
}

rctdl_err_t TrcPktDecodePtm::onProtocolConfig()
{
    rctdl_err_t err = RCTDL_OK;
    if(m_config == 0)
        return RCTDL_ERR_NOT_INIT;

    // static config - copy of CSID for easy reference
    m_CSID = m_config->getTraceID();

    // config options affecting decode
    m_instr_info.pe_type.profile = m_config->core_prof;
    m_instr_info.pe_type.arch = m_config->arch_ver;
    m_instr_info.dsb_dmb_waypoints = m_config->dmsbWayPt() ? 1 : 0;
    return err;
}

/****************** local decoder routines */

void TrcPktDecodePtm::initDecoder()
{
    m_CSID = 0;
    m_instr_info.pe_type.profile = profile_Unknown;
    m_instr_info.pe_type.arch = ARCH_UNKNOWN;
    m_instr_info.dsb_dmb_waypoints = 0;
    resetDecoder();
}

void TrcPktDecodePtm::resetDecoder()
{
    m_curr_state = NO_SYNC;
    m_need_isync = true;    // need context to start.
    m_need_addr = true;     // need an initial address.
    m_instr_info.isa = rctdl_isa_unknown;
    m_is_secure = true;
    m_mem_nacc_pending = false;

    m_pe_context.ctxt_id_valid = 0;
    m_pe_context.bits64 = 0;
    m_pe_context.vmid_valid = 0;
    m_pe_context.exception_level = rctdl_EL3;
    m_pe_context.security_level = rctdl_sec_secure;
    m_pe_context.el_valid = 0;
    
    m_last_state.instr_addr = 0x0;
    m_last_state.isa = rctdl_isa_unknown;
    m_last_state.valid = false;
    m_current_state = m_last_state;

    m_atoms.clearAll();
}

rctdl_datapath_resp_t TrcPktDecodePtm::decodePacket()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    switch(m_curr_packet_in->getType())
    {
        // ignore these from trace o/p point of veiw
    case PTM_PKT_NOTSYNC:   
    case PTM_PKT_INCOMPLETE_EOT:
    case PTM_PKT_NOERROR:
        break;

        // bad / reserved packet - need to wait for next sync point
    case PTM_PKT_BAD_SEQUENCE:
    case PTM_PKT_RESERVED:
        m_curr_state = WAIT_SYNC;
        m_output_elem.setType(RCTDL_GEN_TRC_ELEM_NO_SYNC);
        resp = outputTraceElement(m_output_elem);
        break;

        // packets we can ignore if in sync
    case PTM_PKT_A_SYNC:
    case PTM_PKT_IGNORE:
        break;

        // 
    case PTM_PKT_I_SYNC:
        resp = processIsync();
        break;

    case PTM_PKT_BRANCH_ADDRESS:
        resp = processBranch();
        break;

    case PTM_PKT_TRIGGER:
        m_output_elem.setType(RCTDL_GEN_TRC_ELEM_EVENT);
        m_output_elem.setEvent(EVENT_TRIGGER, 0);
        resp = outputTraceElement(m_output_elem);
        break;

    case PTM_PKT_WPOINT_UPDATE:
        resp = processWPUpdate();
        break;

    case PTM_PKT_CONTEXT_ID:
        {
            bool bUpdate = true;  
            // see if this is a change
            if((m_pe_context.ctxt_id_valid) && (m_pe_context.context_id == m_curr_packet_in->context.ctxtID))
                bUpdate = false;
            if(bUpdate)
            {
                m_pe_context.context_id = m_curr_packet_in->context.ctxtID;
                m_pe_context.ctxt_id_valid = 1;
                m_output_elem.setType(RCTDL_GEN_TRC_ELEM_PE_CONTEXT);
                m_output_elem.setContext(m_pe_context);
                resp = outputTraceElement(m_output_elem);
            }
        }        
        break;

    case PTM_PKT_VMID:
        {
            bool bUpdate = true;  
            // see if this is a change
            if((m_pe_context.vmid_valid) && (m_pe_context.vmid == m_curr_packet_in->context.VMID))
                bUpdate = false;
            if(bUpdate)
            {
                m_pe_context.vmid = m_curr_packet_in->context.VMID;
                m_pe_context.vmid_valid = 1;
                m_output_elem.setType(RCTDL_GEN_TRC_ELEM_PE_CONTEXT);
                m_output_elem.setContext(m_pe_context);
                resp = outputTraceElement(m_output_elem);
            }
        }   
        break;

    case PTM_PKT_ATOM:
        if(!m_need_addr)
        {
            m_atoms.initAtomPkt(m_curr_packet_in->getAtom(),m_index_curr_pkt);
            resp = processAtom();
        }
        break;

    case PTM_PKT_TIMESTAMP:
        m_output_elem.setType(RCTDL_GEN_TRC_ELEM_TIMESTAMP);
        m_output_elem.timestamp = m_curr_packet_in->timestamp;
        if(m_curr_packet_in->cc_valid)
            m_output_elem.setCycleCount(m_curr_packet_in->cycle_count);
        resp = outputTraceElement(m_output_elem);
        break;

    case PTM_PKT_EXCEPTION_RET:
        m_output_elem.setType(RCTDL_GEN_TRC_ELEM_EXCEPTION_RET);
        resp = outputTraceElement(m_output_elem);
        break;

    }
    return resp;
}

rctdl_datapath_resp_t TrcPktDecodePtm::processIsync()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;

    // extract the I-Sync data
    if(!m_b_part_isync)
    {

        m_b_part_isync = true;  // part i-sync;

        m_last_state.instr_addr = m_curr_packet_in->addr.val;
        m_last_state.isa = m_curr_packet_in->getISA();
        m_last_state.valid = true;

        m_current_state = m_last_state;


        m_b_i_sync_pe_context = m_curr_packet_in->ISAChanged();
        if(m_curr_packet_in->CtxtIDUpdated())
        {
            m_pe_context.context_id = m_curr_packet_in->getCtxtID();
            m_pe_context.ctxt_id_valid = 1;
            m_b_i_sync_pe_context = true;
        }

        if(m_curr_packet_in->VMIDUpdated())
        {
            m_pe_context.vmid = m_curr_packet_in->getVMID();
            m_pe_context.vmid_valid = 1;
            m_b_i_sync_pe_context = true;
        }

        m_pe_context.security_level = m_curr_packet_in->getNS() ? rctdl_sec_nonsecure : rctdl_sec_secure;
    }


    return resp;
}

// change of address and/or exception in program flow.
// implies E atom before the branch if none exception.
rctdl_datapath_resp_t TrcPktDecodePtm::processBranch()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    


    return resp;
}

// effectively completes a range prior to exception or after many bytes of trace (>4096)
//
rctdl_datapath_resp_t TrcPktDecodePtm::processWPUpdate()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;

    // if we need an address to run from then the WPUpdate will not form a range as 
    // we do not have a start point - still waiting for branch or other address packet
    if(!m_need_addr)
    {
        // WP update implies E atom - though it doesn't really matter.
        resp = processAtomRange(ATOM_E,"WP update",true,m_curr_packet_in->getAddrVal());
    }

    // atom range may return with NACC pending 
    checkPendingNacc(resp);

    // if wait and still stuff to process....
    if(RCTDL_DATA_RESP_IS_WAIT(resp) && ( m_mem_nacc_pending))
        m_curr_state = CONT_WPUP;

    return resp;
}

// a single atom packet can result in multiple range outputs...need to be re-entrant in case we get a wait response.
// also need to handle nacc response from instruction walking routine
// 
rctdl_datapath_resp_t TrcPktDecodePtm::processAtom()
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    bool bWPFound = false;

    // loop to process all the atoms in the packet
    while(m_atoms.numAtoms() && !m_need_addr && RCTDL_DATA_RESP_IS_CONT(resp))
    {
        resp = processAtomRange(m_atoms.getCurrAtomVal(),"atom");
        if(m_need_addr)
            m_atoms.clearAll();
        else
            m_atoms.clearAtom();
    }

    // bad address may mean a nacc needs sending
    checkPendingNacc(resp);

    // if wait and still stuff to process....
    if(RCTDL_DATA_RESP_IS_WAIT(resp) && ( m_mem_nacc_pending || m_atoms.numAtoms()))
        m_curr_state = CONT_ATOM;

    return resp;
}

 void TrcPktDecodePtm::checkPendingNacc(rctdl_datapath_resp_t &resp)
 {
    if(m_mem_nacc_pending && RCTDL_DATA_RESP_IS_CONT(resp))
    {
        m_output_elem.setType(RCTDL_GEN_TRC_ELEM_ADDR_NACC);
        m_output_elem.st_addr = m_nacc_addr;
        resp = outputTraceElementIdx(m_index_curr_pkt,m_output_elem);
        m_mem_nacc_pending = false;
    }
 }

// given an atom element - walk the code and output a range or mark nacc.
rctdl_datapath_resp_t TrcPktDecodePtm::processAtomRange(const rctdl_atm_val A, const char *pkt_msg,  const bool traceToAddrNext /*= false*/, const rctdl_vaddr_t nextAddrMatch /*= 0*/)
{
    rctdl_datapath_resp_t resp = RCTDL_RESP_CONT;
    bool bWPFound = false;
    std::ostringstream oss;

    m_instr_info.instr_addr = m_current_state.instr_addr;
    m_instr_info.isa = m_current_state.isa;

    rctdl_err_t err = traceInstrToWP(bWPFound);
    if(err != RCTDL_OK)
    {
        if(err == RCTDL_ERR_UNSUPPORTED_ISA)
        {
                m_need_addr = true;
                oss << "Warning: unsupported instruction set processing " << pkt_msg << " packet.";
                LogError(rctdlError(RCTDL_ERR_SEV_WARN,err,m_index_curr_pkt,m_CSID,oss.str()));  
                // wait for next address
                return resp;
        }
        else
        {
            resp = RCDTL_RESP_FATAL_INVALID_DATA;
            oss << "Error processing " << pkt_msg << " packet.";
            LogError(rctdlError(RCTDL_ERR_SEV_ERROR,err,m_index_curr_pkt,m_CSID,oss.str()));  
            return resp;
        }
    }
    
    if(bWPFound)
    {
        // action according to waypoint type and atom value
        switch(m_instr_info.type)
        {
        case RCTDL_INSTR_BR:
            if(A == ATOM_E)
                m_instr_info.instr_addr = m_instr_info.branch_addr;
            break;

            // TBD: is this valid for PTM -> branch addresses imply E atom, N atom does not need address (return stack will require this)
        case RCTDL_INSTR_BR_INDIRECT:
            if(A == ATOM_E)
                m_need_addr = true; // indirect branch taken - need new address.
            break;
        }
            
        m_output_elem.setType(RCTDL_GEN_TRC_ELEM_INSTR_RANGE);
        m_output_elem.last_instr_exec = (A == ATOM_E) ? 1 : 0;
        m_output_elem.last_i_type = m_instr_info.type;
        m_output_elem.last_i_subtype = m_instr_info.sub_type;
        resp = outputTraceElementIdx(m_index_curr_pkt,m_output_elem);

        m_current_state.instr_addr = m_instr_info.instr_addr;
        m_current_state.isa = m_instr_info.next_isa;
    }
    else
    {
        // no waypoint - likely inaccessible memory range.
        m_need_addr = true; // need an address update 

        if(m_output_elem.st_addr != m_output_elem.en_addr)
        {
            // some trace before we were out of memory access range
            m_output_elem.setType(RCTDL_GEN_TRC_ELEM_INSTR_RANGE);
            resp = outputTraceElementIdx(m_index_curr_pkt,m_output_elem);
        }
    }
    return resp;
}

rctdl_err_t TrcPktDecodePtm::traceInstrToWP(bool &bWPFound, const bool traceToAddrNext /*= false*/, const rctdl_vaddr_t nextAddrMatch /*= 0*/)
{
    uint32_t opcode;
    uint32_t bytesReq;
    rctdl_err_t err = RCTDL_OK;

    rctdl_mem_space_acc_t mem_space = m_is_secure ? RCTDL_MEM_SPACE_S : RCTDL_MEM_SPACE_N;

    m_output_elem.st_addr = m_output_elem.en_addr = m_instr_info.instr_addr;

    bWPFound = false;

    while(!bWPFound && !m_mem_nacc_pending)
    {
        // start off by reading next opcode;
        bytesReq = 4;
        err = accessMemory(m_instr_info.instr_addr,mem_space,&bytesReq,(uint8_t *)&opcode);
        if(err != RCTDL_OK) break;

        if(bytesReq == 4) // got data back
        {
            m_instr_info.opcode = opcode;
            err = instrDecode(&m_instr_info);
            if(err != RCTDL_OK) break;

            // increment address - may be adjusted by direct branch value later
            m_instr_info.instr_addr += m_instr_info.instr_size;

            // update the range decoded address in the output packet.
            m_output_elem.en_addr = m_instr_info.instr_addr;

            m_output_elem.last_i_type = m_instr_info.type;
            // either walking to match the next instruction address or a real watchpoint
            if(traceToAddrNext)
                bWPFound = (m_output_elem.en_addr == nextAddrMatch);
            else
                bWPFound = (m_instr_info.type != RCTDL_INSTR_OTHER);
        }
        else
        {
            // not enough memory accessible.
            m_mem_nacc_pending = true;
            m_nacc_addr = m_instr_info.instr_addr;
        }
    }
    return err;
}

/* End of File trc_pkt_decode_ptm.cpp */
