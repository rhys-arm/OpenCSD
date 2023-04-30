set(ETMV3OBJ_src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv3/trc_cmp_cfg_etmv3.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv3/trc_pkt_decode_etmv3.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv3/trc_pkt_elem_etmv3.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv3/trc_pkt_proc_etmv3.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv3/trc_pkt_proc_etmv3_impl.cpp
)

set(ETMV4OBJ_src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv4/trc_cmp_cfg_etmv4.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv4/trc_etmv4_stack_elem.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv4/trc_pkt_proc_etmv4i.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv4/trc_pkt_decode_etmv4i.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/etmv4/trc_pkt_elem_etmv4i.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ete/trc_cmp_cfg_ete.cpp
)

set(PTMOBJ_src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ptm/trc_cmp_cfg_ptm.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ptm/trc_pkt_elem_ptm.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ptm/trc_pkt_proc_ptm.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ptm/trc_pkt_decode_ptm.cpp
)

set(IDECOBJ_src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/i_dec/trc_i_decode.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/i_dec/trc_idec_arminst.cpp
)

set(MEMACCOBJ_src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/mem_acc/trc_mem_acc_mapper.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/mem_acc/trc_mem_acc_bufptr.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/mem_acc/trc_mem_acc_file.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/mem_acc/trc_mem_acc_base.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/mem_acc/trc_mem_acc_cb.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/mem_acc/trc_mem_acc_cache.cpp
)

set(STMOBJ_src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/stm/trc_pkt_elem_stm.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/stm/trc_pkt_proc_stm.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/stm/trc_pkt_decode_stm.cpp
)

set(PKTPRNTOBJ_src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/pkt_printers/raw_frame_printer.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/pkt_printers/trc_print_fact.cpp
)

set(src_files
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_code_follower.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_dcd_tree.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_error.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_error_logger.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_gen_elem_list.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_gen_elem_stack.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_lib_dcd_register.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_msg_logger.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/ocsd_version.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/trc_component.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/trc_core_arch_map.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/trc_frame_deformatter.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/trc_gen_elem.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/trc_printable_elem.cpp
    ${OPENCSD_SRCROOT_DIR}/decoder/source/trc_ret_stack.cpp
    ${ETMV3OBJ_src_files}
    ${ETMV4OBJ_src_files}
    ${IDECOBJ_src_files}
    ${MEMACCOBJ_src_files}
    ${STMOBJ_src_files}
    ${PTMOBJ_src_files}
    ${PKTPRNTOBJ_src_files}
)

if (UNIX)
    add_library(opencsd SHARED ${src_files})
endif()
add_library(opencsd_static STATIC ${src_files})

set(incl_dirs
    ${OPENCSD_SRCROOT_DIR}/decoder/include
)
if (UNIX)
    target_include_directories(opencsd PRIVATE
        ${incl_dirs}
    )
endif()
target_include_directories(opencsd_static PRIVATE
    ${incl_dirs}
)

if (UNIX)
    target_link_options(opencsd PRIVATE "LINKER:${COMMON_LINKER_FLAGS}")

    set_target_properties(opencsd
        PROPERTIES SOVERSION 1
    )

    set_target_properties(opencsd_static PROPERTIES OUTPUT_NAME opencsd)
endif()
