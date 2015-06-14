/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>

#include "infault.h"
#include "radiator.h"
#include "analyzer.h"
#include "graph.h"

#include "parser.h"
#include "edif_parser.h"
#include "verilog_parser.h"
#include "serializer.h"
#include "verilogparser.h"

using namespace std;
using namespace boost;
namespace po = program_options;

filetype_enum detect_filetype(std::string s)
{
    char buf[100];
    
    //open file
    FILE *pFile = fopen(s.c_str(),"r+");
    
    //check if open ok
    if (pFile == NULL){
        cout << "ERROR: cant read file" << endl;
        return FILETYPE_NOFILE;
    }
    
    //read first bytes
    fgets(buf, 100 , pFile);
    
    //close file
    fclose(pFile);
    
    //check if it is an edif ?
    if (strstr(buf,"edif") != NULL){
        //edif detected
        cout << "EDIF filetype detected" << endl;
        return FILETYPE_EDIF;
    }else if (strstr(buf,"infault_human_readable_dump_file") != NULL){
        //intermediate file detected
        cout << "INTERMEDIATE filetype detected" << endl;
        return FILETYPE_INTERMEDIATE;
    }else{
        //quick hack, assume verilog
        cout << "VERILOG filetype detected" << endl;
        return FILETYPE_VERILOG;
    }
}


int main(int argc, char *argv[])
{   string input_filename  = "";
    string input_module    = "";
    string output_filename = "";
    
    Parser *parser = NULL;
    
    Graph      *g      = new Graph();
    Radiator   *radiator = new Radiator();
    Analyzer   *analyzer = new Analyzer();
    Serializer *ser = new Serializer();
    
    //declare the supported options.
    po::options_description desc("INFault - INtelligent Fault analysis\n\nusage: infault [arguments] [input file]");
    desc.add_options()
        ("help", "show this help message")
        ("export",  po::value<string>(),"convert given file to intermediate file representation into file <arg>")
        ("graphviz-full","print resulting graph as dot format (DO NOT use on big graphs!)")
        ("inject", "perform seu injection campaign (on given edif, verilog or intermediate file)")
        ("analyze", "perform triplication analysis (on given edif, verilog or intermediate file)")
        ("dump",  po::value<string>(), "dump graph for given node")
        ("input-file",  po::value<string>(), "input file (verilog,edif or intermediate)")
        ("input-module",  po::value<string>(), "input module name (for verilog input only)")
    ;

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
    options(desc).positional(p).run(), vm);
    po::notify(vm);
    
    //po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, desc), vm);
    //po::notify(vm);

    //show help message
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    cout << "> init..." << endl;
    
    //extract input filename
    if (!vm.count("input-file")){
        cout << desc << "\n> ERROR: no inputfile given!\n\n";
        return 1;
    }else{
        input_filename = vm["input-file"].as<string>();
    }
    
    //check what to do
    if (!vm.count("analyze") && !vm.count("export") && !vm.count("inject") && !vm.count("dump")){
        cout << desc << "\n> ERROR: no action requested (analyze, export, inject, dump)\n\n";
        return 1;
    }
    
    //extract input modulename
    if (vm.count("input-module")){
        input_module = vm["input-module"].as<string>();
    }
    
    cout << "> processing file '" << input_filename << "': ";
    
    filetype_enum ftype = detect_filetype(input_filename);
    
    //detect filetype (edif,verilog or intermediate)
    switch(ftype){
        default:
        case(FILETYPE_NOFILE) : 
            cout << "> error opening file. exiting" << endl;
            return 0;
        
        case(FILETYPE_EDIF) : 
            //edif file! set up parser & get graph
            parser = new EDIF_Parser(input_filename.c_str());
            g = parser->get_graph();
            break;

        case(FILETYPE_VERILOG) : 
            //verilog file! set up parser & get graph
            if (input_module == ""){
                cout << "> ERROR: verilog parser option --input-module=xx necessary!\n\n";
                return 0;
            }
            parser = new Verilog_Parser(input_filename.c_str(), input_module.c_str());
            g = parser->get_graph();
            break;

        case(FILETYPE_INTERMEDIATE) : 
            //intermediate format, reload data
            ser->load_graph_from_file(g, input_filename);
            break;
    }
    
    //check graph consistency
    g->check_consistency();
    
    //print graphviz ?
    if (vm.count("graphviz-full")){
        g->print_graphviz();
    }
    
    //export request ?
    if (vm.count("export")){
        string output_filename = vm["export"].as<string>();
        cout << "> export requested. saving intermediate file to '" << output_filename << "'" << endl;
        ser->save_graph_to_file(g, output_filename);
        cout << "> done. file saved" << endl;
    }

    //remove buffers from graph! 
    g->remove_buffers();
    
    //remove chains like ... ---> NOT ----> NOT ---> ....
     g->optimize_double_inverters();
    
    //call buffer remover again, we might have introduced some new
    g->remove_buffers();
    
    //optimize muxes
    //NOTE: this is mandatory as it removes the scanmode input MUXES
    //      so that we can detect & work with the triplets directly
    //      without having the muxes interferring with our algorithms ;)
    //      (quick'n'dirty hack but allows asic library independent analysis)
    g->optimize_muxes();
    
    //optimize away identical gates that drive FFs:
    g->optimize_duplicates_to_ffs();
    
    /*printf("\n\ndigraph g{\n");
    Node *dut = g->get_node_by_name("/pci/pci0_mas_if/U1590/_inst_NOT_12614");
    dut->print_graphviz(0);
    printf("}\n");
    exit(-1);*/
    
//     Node *dut = g->get_node_by_name("/iu/wr_error_r2_reg/_inst_UDP_DFF_35344"); 
//       if (dut != NULL)dut->print_graphviz(0);
//       if (dut != NULL) radiator->do_seu_search(g, dut);
//                exit(0);
//     
    //dump graph?
    if (vm.count("dump")){
        string gatename = vm["dump"].as<string>();
        cout << "> dump  of gate '" << gatename << "' requested." << endl;
        Node *dut = g->get_node_by_name(gatename);
        if (dut == NULL){
            cout << "> ERROR: gate not found ..." << endl;
            exit(-1);
        }
        cout << "digraph g{" << endl;
        dut->print_graphviz();
        cout << "}" << endl;
        exit(0);
    }
    
    //analyse triplicatyion ?
    if (vm.count("analyze")){
        cout << "> analyze requested." << endl;
        analyzer->analyze(g);
    }
        
    //do seu injection ?
    if (vm.count("inject")){
        cout << "> seu inject simulation requested." << endl;
        radiator->do_seu_search(g);
    }
    
    
    
    //Parser *parser = new Parser();
    //Parser *parser = new EDIF_Parser("/tmp/mcore.ndf");
    //Parser *parser = new Verilog_Parser("/home/sschulz/pci_atmel_all__array__.v", "pci_is");
    //Parser *parser = new Verilog_Parser("/tmp/new-netlist/leon_pci_mod.v", "pci_mas");
    //Parser *parser = new Verilog_Parser("/tmp/new-netlist/minitest.v", "main");
    //Parser *parser = new Verilog_Parser("/tmp/o", "pci_mas");
    
    //test print
    /*printf("\n\ndigraph g{\n");
    Node *dut = g->get_node_by_name("/pci/pci0_mas_if/U1590/_inst_NOT_12614");
    dut->print_graphviz(0);
    printf("}\n");
    exit(-1);*/
    
    
    //for debugging: do small search
    //Node *dut = g->get_node_by_name("DUT/pci_out[10]__OUT");
    //dut->print_graphviz();
    //radiator->do_seu_search(g, dut);
    //exit(0);
    
    //check graph
    //analyzer->analyze(g);
    //exit(-1);
    
    //Node *n = g->get_node_by_name("DUT/pci_core/U1449__Z", 0);
    //Node *n = g->get_node_by_name("DUT/pci_core/U15008__ZN", 0);
    //Node *n = g->get_node_by_name("DUT/pci_core/U15024__ZN", 0);
    
    //n->print_graphviz(0);
    //radiator->do_seu_search(g, n,0);
    //xit(0);
    
    //some info
    printf("> finished everything. normal exit.\n\n");
    
    exit(EXIT_SUCCESS);
}
