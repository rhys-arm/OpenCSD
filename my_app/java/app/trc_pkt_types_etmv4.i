%module jopencsd
%{
/*#include "ocsd_if_types.h"
#include "trc_pkt_types.h"*/
#include "trc_pkt_types_etmv4.h"

#include "ocsd_dcd_tree.h"

#include "trc_cmp_cfg_etmv4.h"

#include "ocsd_msg_logger.h"

#include "ocsd_error_logger.h"

#include "trc_gen_elem_in_i.h"
%}

%include various.i
//%apply char *BYTE { char *buffer_variable_name };
%apply char *BYTE {unsigned char*};
//%apply char*  {unsigned char*};



%include "std_string.i"
%include "stdint.i"
%include "ocsd_if_types.h"
%include "trc_pkt_types.h"
%include "trc_pkt_types_etmv4.h"

%include "trc_data_raw_in_i.h"
%include "ocsd_dcd_tree.h"

%include "trc_cs_config.h"
%include "trc_cmp_cfg_etmv4.h"

%include "ocsd_msg_logger.h"

%include "trc_error_log_i.h"
%include "ocsd_error_logger.h"

%include "trc_gen_elem_types.h"
%include "trc_printable_elem.h"
%include "trc_gen_elem.h"
%include "trc_gen_elem_in_i.h"
