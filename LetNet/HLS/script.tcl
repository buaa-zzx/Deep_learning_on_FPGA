############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project LetNet
set_top top_fun
add_files LetNet/src/conv.h
add_files LetNet/src/conv.cpp
add_files -tb LetNet/src/main.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu3eg-sfvc784-1-i} -tool vivado
create_clock -period 10 -name default
#source "./LetNet/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
