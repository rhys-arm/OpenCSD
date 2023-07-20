%module jopencsd
%{
/*#include "ocsd_if_types.h"
#include "trc_pkt_types.h"*/
#include "trc_pkt_types_etmv4.h"
#include "ocsd_dcd_tree.h"
%}

%include "stdint.i"
%include "ocsd_if_types.h"
%include "trc_pkt_types.h"
%include "trc_pkt_types_etmv4.h"

%include "trc_data_raw_in_i.h"
%include "ocsd_dcd_tree.h"
