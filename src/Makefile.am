bin_PROGRAMS = infault
infault_SOURCES = analyzer.cpp edif_parser.cpp graph.cpp infault.cpp node.cpp \
	node_buf1.cpp \
	node_and2.cpp \
	node_and3.cpp \
	node_and4.cpp \
	node_gnd.cpp \
	node_input.cpp \
	node_nand2.cpp \
	node_nor2.cpp \
	node_not1.cpp \
	node_not.cpp \
	node_or2.cpp \
	node_or3.cpp \
	node_or4.cpp \
	node_output.cpp \
	node_udp_dff.cpp \
	node_udp_mux21.cpp \
	node_unisim_and2.cpp \
	node_unisim_ff.cpp \
	node_unisim_gnd.cpp \
	node_unisim_lut4.cpp \
	node_unisim_mux2.cpp \
	node_unisim_mux2cy.cpp \
	node_unisim_not.cpp \
	node_unisim_or2.cpp \
	node_unisim_vcc.cpp \
	node_unisim_xor2.cpp \
	node_vcc.cpp \
	node_xnor2.cpp \
	node_xor2.cpp \
	parser.cpp radiator.cpp \
	serializer.cpp verilog_parser.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
infault_LDFLAGS = $(all_libraries) -lboost_regex -lboost_program_options
noinst_HEADERS = analyzer.h debug.h edif_parser.h graph.h infault.h node.h \
	node_gnd.h node_include.h node_vcc.h parser.h radiator.h serializer.h \
	verilog_parser.h verilog_parser_globals.h verilogparser.h
