############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project skynet
set_top SkyNet
add_files skynet/src/reorder_weight.cc
add_files skynet/src/output_verify.cc
add_files skynet/src/net_hls.h
add_files skynet/src/net_hls.cc
add_files skynet/src/golden_c.cc
add_files skynet/src/dwconv3x3.cc
add_files skynet/src/conv1x1.cc
add_files -tb skynet/src/tb.cc -cflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu3eg-sfvc784-1-i} -tool vivado
create_clock -period 5 -name default
config_compile -no_signed_zeros=0 -unsafe_math_optimizations=0
config_schedule -effort medium -enable_dsp_full_reg=0 -relax_ii_for_timing=0 -verbose=0
config_export -format ip_catalog -rtl verilog -vivado_phys_opt place -vivado_report_level 0
config_bind -effort medium
config_sdx -optimization_level none -target none
#source "./skynet/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
