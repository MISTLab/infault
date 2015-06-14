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
#include "verilog_parser.h"
#include "verilogparser.h"
#include "node_factory.h"
#include "radiator.h"

Verilog_Parser::Verilog_Parser() : Parser()
{
    module_name = "__WRONG_CONSTRUCTOR_CALLED__";
}


Verilog_Parser::~Verilog_Parser()
{
}

Verilog_Parser::Verilog_Parser(const char *_filename, const char *modname) : Parser(_filename)
{
    module_name = modname;
    filename = _filename;
    input_stream.open(filename);
    
    if (!input_stream.good()){
        printf("ERROR: cant open '%s'. parse will fail...\n",filename);
    }
}

void Verilog_Parser::remove_newlines_and_tabs(char *line)
{
    char *linep_src    = &line[0];
    char *linep_target = &line[0];
    while(*linep_src != 0){
        if (*linep_src == '\n'){
            //remove this newline 
            linep_src++;
            //and also all spaces afterwars
            while( (*(linep_src+1)!=0) && ((*linep_src == ' ') || (*linep_src == '\t'))){
                  linep_src++;
            }
        }if (*linep_src == '\t'){
            linep_src++;
        }
        *linep_target = *linep_src;
        *linep_src++;
        *linep_target++;
    }
    *linep_target = *linep_src;
}

int Verilog_Parser::is_output_port(string s, string node_type)
{
    cmatch re_result;
    regex re_output = regex("ZO|Z|CO|Q|QN|ZN", regex_constants::perl);
    
    //uppercase
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    transform(node_type.begin(), node_type.end(), node_type.begin(), ::toupper);
    
    if (regex_match(s.c_str(), re_result, re_output)){
        return 1;
    }else if((node_type == "AH01D") && (s=="S")){
        //ah01 has an output named S
        return 1;
    }else{
        return 0;
    }
}

void Verilog_Parser::create_gates(Graph *graph, moduledescription_map_t *module_map, string submodule_name, string instance_name)
{
    int count = 0;
    
    debug_vparse_verbose_printf("create_gates called (..,..,%s,%s);\n",submodule_name.c_str(),instance_name.c_str());
    
    //create objects
    int max = (*module_map)[submodule_name].node2type_map.size();
    int now = 0;
    
    stringstring_map_t::iterator node_it;
    for (node_it = (*module_map)[submodule_name].node2type_map.begin(); node_it != (*module_map)[submodule_name].node2type_map.end(); node_it++){
        now++;
        if (count++ == 50){
            printf("                                   \r> parsing verilog (step 2) %3d%% done : ",now*100/max);
            cout.flush();
            count=0;
        }
        string node_name = instance_name + "/" + node_it->first;
        string node_class = node_it->second;
        
        debug_vparse_verbose_printf("Node [%s] is type '%s'\n", node_name.c_str(), node_class.c_str()); 
        //create node, check if this is a known submodule:
        if ((*module_map).count(node_class) > 0){
            //yes, submodule -> intantiate it for the given name
            //printf("RECURSION\n");
            create_gates(graph, module_map, node_class, node_name);
        }else{
            //no, this is a gate -> create
            Node *newnode = graph->create_node("VERILOG", node_name, node_class, 0);
            graph->add_node(newnode);
        }
    }
}

//connect nodes
void Verilog_Parser::connect_gates(Graph *graph, moduledescription_map_t *module_map, string submodule_name, string instance_name)
{
    debug_vparse_verbose_printf("connect_gates(...,%s,%s)\n",submodule_name.c_str(),instance_name.c_str());
    nodename_map_t::iterator it;
    
    //add info where to find gnd/vcc (=how to translate netname to node)
    (*module_map)[submodule_name].net2node_map["1'b0"] = "1'b0";
    (*module_map)[submodule_name].net2node_map["1'b1"] = "1'b1";
    Node *gnd = new Node_GND((instance_name + "/1'b0").c_str());
    graph->add_node(gnd);
    Node *vcc = new Node_VCC((instance_name + "/1'b1").c_str());
    graph->add_node(vcc);
      
    //add vcc & gnd
    graph->add_node(new Node_GND((instance_name + "/GND").c_str()));
    graph->add_node(new Node_VCC((instance_name + "/VCC").c_str()));
    
    int max = (*module_map)[submodule_name].nodeport2net_map.size();
    int now = 0;
    int count = 0;
    
    for (it = (*module_map)[submodule_name].nodeport2net_map.begin(); it != (*module_map)[submodule_name].nodeport2net_map.end(); it++){  
        
        now++;
        if (count++ == 50){
            printf("                                   \r> parsing verilog (step 3) %3d%% done : ",now*100/max);
            cout.flush();
            count=0;
        }
        
        string node_name_plain = it->first;
        string node_name = instance_name + "/" + node_name_plain;
        
        port2net_vector_t port_info = it->second;
        
        
        //ok this could have been an instance we need to connect this internally too:
        string node_type = (*module_map)[submodule_name].node2type_map[node_name_plain];
        
        int is_submodule = ((*module_map).count(node_type) > 0)?1:0;
        if (is_submodule){
            connect_gates(graph,module_map,node_type,node_name);
            //printf("NODE TYPE %s\n",node_type.c_str());
        }
        
        //now connect
        debug_vparse_verbose_printf("> need to connect %d ports for Node [%s]\n",  port_info.size(), node_name.c_str()); 
        
        //find all ports of this
        for (int p=0; p<port_info.size(); p++){
            port2net_t p2net = port_info.at(p);
            string node_port = p2net.first;
            string node_net  = p2net.second;
            
            //only connect if this is an input!
            iodescription_enum io_dir = INPUT;
            if (is_submodule){
                //fetch io_dir, note that submodule for this fetch is node_type!
                io_dir = (*module_map)[node_type].iodescription_map[node_port];
                debug_vparse_verbose_printf("get submodule io_dir module_map[%s].iodescription_map[%s] = %d\n", node_type.c_str(), node_port.c_str(), io_dir);
            }else if(is_output_port(node_port, node_type)){
                io_dir = OUTPUT;
            }
            
            if (io_dir == INPUT){
                //find node associated to this net:
                string node_source_name = instance_name + "/" + (*module_map)[submodule_name].net2node_map[node_net];
                string node_target_name = node_name;
                //printf("xxx searching source result : module_map[%s].net2node_map[%s] = %s\n",submodule_name.c_str(),node_net.c_str(),node_source_name.c_str());
                        
                if (is_submodule){
                    //this is a submodule, we connect all cross boundary buffers now
                    node_target_name = node_name  + "/" + node_port + "_CONNECTOR_";
                    node_port = "i";
                }
                
                //string node_name_full = node_name + ":" + node_port;
                
                debug_vparse_verbose_printf("Node %3d: %s:%s [net %s]",p, node_target_name.c_str(),node_port.c_str(),node_net.c_str());
                if ((*module_map)[submodule_name].net2node_map[node_net] == ""){
                    printf("\rERROR: cant find driver for net %s!\n", node_net.c_str());
                    exit(-1);
                }else{
                    //debug
                    debug_vparse_verbose_printf(" is driven by node %s\n",node_source_name.c_str()); 
                    
                    Node *source = graph->get_node_by_name_quiet(node_source_name);
                    Node *target = graph->get_node_by_name_quiet(node_target_name);
                    
                    if ( (source == NULL) || (target == NULL)){
                        //hmmm src or target does not exist...
                        
                        //check if this was a request for a inverted output gate _?N
                        //no, skip the test, just test if there is a node with 
                        //the N removed from the name:
                        Node *target2 = graph->get_node_by_name_quiet(node_target_name.substr(0, node_target_name.length()-1));
                        if ((source != NULL) && (target2 != NULL)){
                            //ok this is still safe
                        }else{
                            //no something went wrong                            
                            printf("ERROR: source or target node is NULL\n");
                            exit(-1);
                        }
                    }else{
                        //connect
                        bool ok = target->set_input(source, node_port.c_str());
                        if (!ok){
                            printf("failed during set_input()\n");
                            exit(-1);
                        }
                    }
                }
            }else if (io_dir == OUTPUT){
                //if this is a submodule, connect all outputs to the outside
                if (is_submodule){
                    //this is actually useless ?!?!
                    printf("UUUPS IT WAS NOT USELESS!!\n");
                    exit(-1);
                    
                    /*
                    //we need to do some special things
                    string node_target_name = instance_name + "/" + (*module_map)[submodule_name].net2node_map[node_net];
                    string node_source_name = node_name  + "/" + node_port + "_CONNECTOR_";
                    
                    debug_vparse_verbose_printf("Node %3d: %s:%s [net %s]",p,node_target_name.c_str(),"d",node_net.c_str());
                    
                    if ((*module_map)[node_type].net2node_map[node_net] == ""){
                        printf("\rERROR: cant find driver for net %s!\n", node_net.c_str());
                        exit(-1);
                    }else{
                        //debug
                        debug_vparse_verbose_printf(" is driven by node %s\n",node_source_name.c_str()); 
                        Node *source = graph->get_node_by_name(node_source_name);
                        Node *target = graph->get_node_by_name(node_target_name);
                        
                        if ( (source == NULL) || (target == NULL)){
                            printf("ERROR: source or target node is NULL\n");
                            exit(-1);
                        }else{
                            //connect directly to input 0
                            target->set_input(source, 0);
                        }
                    }*/
                }
            }else{
                printf("INVALID TYPE OF IO for [%s] %s! exiting\n",submodule_name.c_str(),node_port.c_str());
                exit(-1);
            }
        }
    }
}
    

Graph *Verilog_Parser::get_graph()
{
    char line[1024000];
    cmatch re_result;
    string submodule_name = "";
    
    regex re_module     = regex("\\s*\\w*module\\s+([^\\s\\(]+)\\s*(.*)", regex_constants::perl);
    regex re_module_io  = regex("\\(?\\s*([^,\\)\\s]+)\\s*[,\\)]\\s*", regex_constants::perl);
    
    regex re_node_io_search = regex("\\.([^\\s\\(]+)\\s*\\(\\s*([^\\)\\s]*)\\s*\\)", regex_constants::perl);
    
    regex re_endmodulemodule     = regex("\\s*endmodulemodule\\s+([^\\s\\(]+)\\s*.*", regex_constants::perl);
    
    regex re_input_array = regex("\\s*wire|input\\s+\\[(\\d+):(\\d+)\\]\\s+([^\\s]+)\\s*.*", regex_constants::perl);
    regex re_output_array = regex("\\s*output\\s+\\[(\\d+):(\\d+)\\]\\s+([^\\s]+)\\s*.*", regex_constants::perl);
    regex re_input      = regex("\\s*input\\s+([^\\s]+)\\s*.*", regex_constants::perl);
    regex re_output     = regex("\\s*output\\s+([^\\s]+)\\s*.*", regex_constants::perl);
    regex re_wire_array = regex("\\s*wire\\s+\\[(\\d+):(\\d+)\\]\\s+([^\\s]+)\\s*.*", regex_constants::perl);
    regex re_input_list = regex(  "\\s*wire|input\\s+([^:;]+)\\s*", regex_constants::perl);
    regex re_output_list = regex("\\s*output\\s+([^:;]+)\\s*", regex_constants::perl);
    
    //                           assign slvi_HWRITE_0 = \slvi[HWRITE] ;
    regex re_assign = regex("\\s*assign\\s+([^\\s]+)\\s*=\\s*([^\\s;]+).*", regex_constants::perl);
    
    //inv0d2 U451(.i(clkm_2_L8_N100), .zn(n894))
    regex re_node     = regex("\\s*([^\\s]+)\\s+([^\\s\\(]+)\\s*\\((.*)\\)", regex_constants::perl);
    //regex re_nodeoutput = regex(".*\\.(zo|z|co|q|qn|zn)\\s*\\(\\s*([^\\)\\s]+)\\s*\\).*", regex_constants::perl);
    regex re_node_io = regex("([^\\s\\(]+)\\s*\\(\\s*([^\\)\\s]+)\\s*\\).*", regex_constants::perl);
    regex re_nodeoutput = regex(".*\\.(zo|z|co|q|qn|zn|s)\\s*\\(\\s*([^\\)\\s]+)\\s*\\).*", regex_constants::perl);
    regex re_nodeoutput_search = regex("\\.(zo|z|co|q|qn|zn|s)\\s*\\(\\s*([^\\)\\s]+)\\s*\\)", regex_constants::perl);
    regex re_module_input_element = regex("\\s*([^,\\s]+),?\\s*", regex_constants::perl);

    regex re_connect = regex("\\s*([^\\s\\(]+)\\(\\s*([^\\s\\)]+)\\s*\\).*", regex_constants::perl);
    
    Graph *graph = new Graph();
    
    //add special default nodes for GND and VCC:
    Node *gnd = new Node_GND("1'b0");
    graph->add_node(gnd);
    Node *vcc = new Node_VCC("1'b1");
    graph->add_node(vcc);
    
    
    
    int count = 0;
    vector<connectionwire> connwire_vector;
    vector<node_description> node_description_vector;
    
    //get filesize
    input_stream.seekg(0, ios_base::end);
    int maxlen = input_stream.tellg(); 
    input_stream.seekg(0, ios_base::beg);
    
    stringstring_map_t net2node_map;
    stringstring_map_t node2type_map;
    nodename_map_t nodeport2net_map;
    
    moduledescription_map_t module_map;
    
    //first loop through file:
    // -> create all nodes and build a table for wirename -> driving net lookup
    while( input_stream.good()){
        if (count++ == 200){
            printf("                                   \r> parsing verilog (step 1) %3d%% done : ",input_stream.tellg()*100/maxlen);
            cout.flush();
            count=0;
        }
        
        //get a line, use ; as delimiter instead of \n
        input_stream.getline(line, 1024000, ';');
    
        //remove all newlines:
        remove_newlines_and_tabs(&line[0]);        
        
        //new try:
        if (regex_match(line, re_result, re_module)){
            submodule_name = re_result[1].str();
            string module_ios = re_result[2].str();
            
            printf("\r> processing submodule [%s]                                          \n",submodule_name.c_str());
            
            //get all ios from module
            string::const_iterator start, end; 
            start = module_ios.begin(); 
            end = module_ios.end();
            match_results<std::string::const_iterator> what; 
            match_flag_type flags = boost::match_default; 
            while(regex_search(start, end, what, re_module_io, flags)){ 
                string io_name = what[1].str();
                printf("= '%s'\n", io_name.c_str());
                module_map[submodule_name].iodescription_map[io_name] = UNKNOWN;

                //update start pos
                start = what[0].second; 
                // update flags: 
                flags |= boost::match_prev_avail; 
                flags |= boost::match_not_bob; 
            }
        }else if (regex_match(line, re_result, re_input_list)){
            ///create input nodes
            string module_ios = re_result[1].str();
    
            string::const_iterator start, end;
            start = module_ios.begin();
            end = module_ios.end();
            match_results<std::string::const_iterator> what;
            match_flag_type flags = boost::match_default;
            while(regex_search(start, end, what, re_module_input_element, flags)){
                string io_name = what[1].str();
    
                module_map[submodule_name].iodescription_map[io_name] = INPUT;
                debug_vparse_verbose_printf("DEI module_map[%s].iodescription_map[%s] = %d\n",submodule_name.c_str(),io_name.c_str(),INPUT);
                    //force to add & connect a cross module boundary buffer
                module_map[submodule_name].node2type_map[io_name + "_CONNECTOR_"] = "buffd_z";
                module_map[submodule_name].net2node_map[io_name] = io_name + "_CONNECTOR_";
                debug_vparse_verbose_printf("CON module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"buffd_z");
    
                    //update start pos
                start = what[0].second;
                    // update flags:
                flags |= boost::match_prev_avail;
                flags |= boost::match_not_bob;
            }
        }else if (regex_match(line, re_result, re_output_list)){
                ///create output nodes
            string module_ios = re_result[1].str();
    
            string::const_iterator start, end;
            start = module_ios.begin();
            end = module_ios.end();
            match_results<std::string::const_iterator> what;
            match_flag_type flags = boost::match_default;
            while(regex_search(start, end, what, re_module_input_element, flags)){
                string io_name = what[1].str();
    
                module_map[submodule_name].iodescription_map[io_name] = OUTPUT;
                debug_vparse_verbose_printf("DEO module_map[%s].iodescription_map[%s] = %d\n",submodule_name.c_str(),io_name.c_str(),OUTPUT);
    
                    //force to add & connect a cross module boundary buffer
                module_map[submodule_name].node2type_map[io_name + "_CONNECTOR_"] = "buffd_z";
                    //default driver is gnd
                module_map[submodule_name].net2node_map[io_name] = "1'b0" ;
                debug_vparse_verbose_printf("CON module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"buffd_z");
    
                    //update start pos
                start = what[0].second;
                    // update flags:
                flags |= boost::match_prev_avail;
                flags |= boost::match_not_bob;
            }
        }else if (regex_match(line, re_result, re_input_array)){
            ///create input nodes
            int a  = atoi(re_result[1].str().c_str());
            int b    = atoi(re_result[2].str().c_str());
            string s  = re_result[3].str();
            for(int j=min(a,b); j<=max(a,b); j++){
                sprintf(line, "%s[%d]", s.c_str(), j);
                string io_name = line;
                module_map[submodule_name].iodescription_map[io_name] = INPUT;
                debug_vparse_verbose_printf("DEI module_map[%s].iodescription_map[%s] = %d\n",submodule_name.c_str(),io_name.c_str(),INPUT);
                //force to add & connect a cross module boundary buffer
                module_map[submodule_name].node2type_map[io_name + "_CONNECTOR_"] = "buffd_z";
                module_map[submodule_name].net2node_map[io_name] = io_name + "_CONNECTOR_";
                debug_vparse_verbose_printf("CON module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"buffd_z");
            }
        }else if (regex_match(line, re_result, re_assign)){
            ///set assigned driver
            string io_name   = re_result[1].str();
            string io_driver = re_result[2].str();
            
            //force to add & connect a cross module boundary buffer
            
            //there are 2 possible ways:
            //A) assign abc = INPUT
            //B) assign OUTPUT = abc
            //we need to distinguish them!!!
            //when we meet a ASSIGN statement we should already know if this is an input
            if (module_map[submodule_name].iodescription_map[io_name] == OUTPUT){
                //this is an output!
                //so set up a dummy buffer to get from abc to OUTPUT:
                module_map[submodule_name].node2type_map[io_name] = "buffd_z";
                module_map[submodule_name].net2node_map[io_name] = io_driver;
                debug_vparse_verbose_printf("ASO module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name).c_str(),"buffd_z");
                
                //connect driver for 
                //module_map[submodule_name].net2node_map[io_name] = 
            }else if(module_map[submodule_name].iodescription_map[io_driver] == INPUT){
                //input!
                module_map[submodule_name].node2type_map[io_name] = "buffd_z";
                module_map[submodule_name].net2node_map[io_name] = io_driver + "_CONNECTOR_";
                debug_vparse_verbose_printf("ASI module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name).c_str(),"buffd_z");
            }else{
                printf("unknown ASSIGNMENT\n");
                exit(-1);
            }
            
        }else if (regex_match(line, re_result, re_input)){
            ///set input nodes
            string io_name = re_result[1].str();
            module_map[submodule_name].iodescription_map[io_name] = INPUT;
            debug_vparse_verbose_printf("DEI module_map[%s].iodescription_map[%s] = %d\n",submodule_name.c_str(),io_name.c_str(),INPUT);
            
            //force to add & connect a cross module boundary buffer
            module_map[submodule_name].node2type_map[io_name + "_CONNECTOR_"] = "buffd_z";
            module_map[submodule_name].net2node_map[io_name] = io_name + "_CONNECTOR_";
            debug_vparse_verbose_printf("CON module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"buffd_z");
        }else if (regex_match(line, re_result, re_output_array)){
            ///create output nodes
            int a  = atoi(re_result[1].str().c_str());
            int b    = atoi(re_result[2].str().c_str());
            string s  = re_result[3].str();
            for(int j=min(a,b); j<=max(a,b); j++){
                sprintf(line, "%s[%d]", s.c_str(), j);
                string io_name = line;
                module_map[submodule_name].iodescription_map[io_name] = OUTPUT;
                debug_vparse_verbose_printf("DEO module_map[%s].iodescription_map[%s] = %d\n",submodule_name.c_str(),io_name.c_str(),OUTPUT);
            
                //force to add & connect a cross module boundary buffer
                module_map[submodule_name].node2type_map[io_name + "_CONNECTOR_"] = "buffd_z";
                 //default driver is gnd
                module_map[submodule_name].net2node_map[io_name] = "1'b0" ;
                debug_vparse_verbose_printf("CON module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"buffd_z");
            }
        }else if (regex_match(line, re_result, re_output)){
            ///set output nodes
            string io_name = re_result[1].str();
            module_map[submodule_name].iodescription_map[io_name] = OUTPUT;
            debug_vparse_verbose_printf("DEO module_map[%s].iodescription_map[%s] = %d\n",submodule_name.c_str(),io_name.c_str(),OUTPUT);
            
            //force to add & connect a cross module boundary buffer
            module_map[submodule_name].node2type_map[io_name + "_CONNECTOR_"] = "buffd_z";
            //default driver is gnd
            module_map[submodule_name].net2node_map[io_name] = "1'b0" ;
            debug_vparse_verbose_printf("CON module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"buffd_z");
            
                        
        }else if (regex_match(line, re_result, re_node)){
            ///create all other nodes
            int is_known_submodule = 0;
            string node_type = re_result[1].str();
            string node_name = re_result[2].str();
            string node_io   = re_result[3].str();
            
            if (module_map.count(node_type) != 0){
                is_known_submodule = 1;
            }
            
            //find all outputs
            string::const_iterator start, end; 
            start = node_io.begin(); 
            end   = node_io.end();
            
            match_results<std::string::const_iterator> what; 
            match_flag_type flags = boost::match_default; 
            bool found = false;

            //search all IOs
            while(regex_search(start, end, what, re_node_io_search, flags)){ 
                string conn_port = what[1].str();
                string conn_net  = what[2].str();
                
                //ok we add this to our info list
                //check if this is a known submodule:
                if (is_known_submodule){
                    //printf("KNOWN SUB\n");
                    //this is a node we have in our "on the fly database"
                    if (module_map[node_type].iodescription_map[conn_port] == OUTPUT){
                        //this is an output, add to list
                        
                        //update/set nodename to type map
                        module_map[submodule_name].node2type_map[node_name] = node_type;
                        debug_vparse_verbose_printf("OUT module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),node_name.c_str(),node_type.c_str());
                        
                        
                        if (conn_net == ""){
                            //force buffer connect to dummy zero
                            module_map[node_type].nodeport2net_map[conn_port + "_CONNECTOR_"].push_back(port2net_t("i","1'b0"));
                            //debug_vparse_printf("OUT module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",node_type.c_str(),(conn_port+"_CONNECTOR_").c_str(),"i","1'b0");
                        
                            //connect to the outside
                            ///module_map[submodule_name].net2node_map[conn_port + "_CONNECTOR_"] = "1'b0";
                            ///debug_vparse_verbose_printf("OUT module_map[%s].net2node_map[%s] = %s\n",submodule_name.c_str(),(conn_port + "_CONNECTOR_").c_str(),"1'b0");
                            
                        }else{
                            //force buffer connect
                            module_map[node_type].nodeport2net_map[conn_port + "_CONNECTOR_"].push_back(port2net_t("i",conn_port));
                            debug_vparse_verbose_printf("OUT module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",node_type.c_str(),(conn_port+"_CONNECTOR_").c_str(),"i",conn_port.c_str());
                            
                            //connect to the outside
                            module_map[submodule_name].net2node_map[conn_net] = node_name +"/"+ conn_port + "_CONNECTOR_";
                            debug_vparse_verbose_printf("OUT module_map[%s].net2node_map[%s] = %s\n",submodule_name.c_str(),conn_net.c_str(),(node_name +"/"+conn_port + "_CONNECTOR_").c_str());
                        }
                    }else if(module_map[node_type].iodescription_map[conn_port] == INPUT){
                        //this is an input
                        
                        //update/set nodename to type map
                        module_map[submodule_name].node2type_map[node_name] = node_type;
                        debug_vparse_verbose_printf("IN  module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),node_name.c_str(),node_type.c_str());
                        
                        //this is an input, store this info in the connection db
                        if (conn_net == ""){
                            //this is not connected!
                            conn_net = "__UNCONNECTED__";
                            //add translation map entry for this to force to GND
                            module_map[submodule_name].net2node_map["__UNCONNECTED__"] = "GND";
                            debug_vparse_verbose_printf("IN  module_map[%s].net2type_map[%s] = %s\n",submodule_name.c_str(),"__UNCONNECTED__","GND");
                        }
                            
                        //add
                        module_map[submodule_name].nodeport2net_map[node_name].push_back(port2net_t(conn_port, conn_net));
                        debug_vparse_verbose_printf("IN  module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",submodule_name.c_str(),node_name.c_str(),conn_port.c_str(),conn_net.c_str());
                        
                    }else{
                        printf("ERROR: found unknown io '%s' for %s\n",conn_port.c_str(),node_type.c_str());
                        exit(-1);
                    }
                }else{
                    //normal gate
                    
                    //remove drive strength from node type
                    string class_type = node_type.substr(0, node_type.size()-1);
                    transform(class_type.begin(), class_type.end(), class_type.begin(), ::toupper);
                    
                    if (is_output_port(conn_port, class_type)){
                        if (conn_net != ""){
                            //this is an connected output, we need to:
                            //port to uppercase
                            transform(conn_port.begin(), conn_port.end(), conn_port.begin(), ::toupper);
                            string node_name_out = node_name + "__" + conn_port;
                            
                            //1 )add a descriptor to the node_obj_list:
                            module_map[submodule_name].node2type_map[node_name_out] = class_type+ "_" + conn_port;
                            debug_vparse_verbose_printf("OUT2 module_map[%s].node2type_map[%s] = %s\n",submodule_name.c_str(),(node_name_out).c_str(),(class_type+ "_" + conn_port).c_str());
                            
                            //2) add to netname -> node_name map
                            module_map[submodule_name].net2node_map[conn_net] = node_name_out; // + "__" + conn_port;
                            debug_vparse_verbose_printf("OUT2 module_map[%s].net2node_map[%s] = %s\n",submodule_name.c_str(),conn_net.c_str(),(node_name_out).c_str());
                            
                            //module_map[submodule_name].nodeport2net_map[node_name].push_back(port2net_t(conn_port, conn_net));
                            //printf("OUT2 module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",submodule_name.c_str(),(node_name).c_str(),conn_port.c_str(),conn_net.c_str());
                        }
                    }else{
                        //this is an input, store this info in the connection db
                        //port to uppercase
                        transform(conn_port.begin(), conn_port.end(), conn_port.begin(), ::toupper);
                
                        //but we have to do this for all outputs of this nodetype !!
                        for(int o=0; o<Node_factory<Node>::get_output_count("ATC18_"+class_type); o++){
                            string node_name_out = node_name + "__" + Node_factory<Node>::get_output_name("ATC18_"+class_type, o);
                            module_map[submodule_name].nodeport2net_map[node_name_out].push_back(port2net_t(conn_port, conn_net));
                            debug_vparse_verbose_printf("IN2  module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",submodule_name.c_str(),(node_name_out).c_str(),conn_port.c_str(),conn_net.c_str());
                        }
                    }
                }
                //update start pos
                start = what[0].second; 
                // update flags: 
                flags |= boost::match_prev_avail; 
                flags |= boost::match_not_bob; 
                found = true;
            }
            
            if (!found){
                printf("ERROR: cant extract IOs for node %s\n",node_name.c_str());
            }
        }
    }
    
    
    if (module_map.count(module_name) == 0){
        //this module does not exist!
        printf("ERROR: submodule '%s' not found in file. exiting\n", module_name.c_str());
        exit(-1);
    }
    
    //create_gates(graph, &module_map, "cu3232af", "pci_core");
    create_gates(graph, &module_map, module_name, "DUT");
    
    //create IOs for this module
    iodescription_map_t::iterator nit;
    for (nit = module_map[module_name].iodescription_map.begin(); nit != module_map[module_name].iodescription_map.end(); nit++){
        string io_name = nit->first;
        iodescription_enum io_dir = nit->second;
        if (io_dir == INPUT){
            graph->add_node(new Node_INPUT(("DUT/"+io_name + "__IN").c_str()));
            
            //add connection between this and connector
            module_map[module_name].nodeport2net_map[io_name + "_CONNECTOR_"].push_back(port2net_t("i", io_name + "__IN_net"));
            module_map[module_name].net2node_map[io_name + "__IN_net"] = io_name + "__IN";
            debug_vparse_verbose_printf("IOI  module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",module_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"i",("DUT/" + io_name + "__IN_net").c_str());
            
        }else{
            graph->add_node(new Node_OUTPUT(("DUT/"+io_name + "__OUT").c_str()));
            //add connection between this and connector
            module_map[module_name].nodeport2net_map[io_name + "__OUT"].push_back(port2net_t("d", io_name+ "_CONNECTOR__net"));
            debug_vparse_verbose_printf("IOO  module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",module_name.c_str(),(io_name + "__OUT").c_str(),"d",(io_name  + "_CONNECTOR__net").c_str());
            
            module_map[module_name].net2node_map[io_name + "_CONNECTOR__net"] = io_name + "_CONNECTOR_";
            
            //test
            module_map[submodule_name].node2type_map[io_name + "_CONNECTOR_"] = "buffd_z";
            graph->add_node(new Node_OUTPUT(("DUT/"+io_name + "_CONNECTOR_").c_str()));
            
            debug_vparse_verbose_printf("module_map[%s].net2node_map[%s] = %s\n",module_name.c_str(),(io_name + "_CONNECTOR__net").c_str(),(io_name + "_CONNECTOR_").c_str());
            
            module_map[module_name].nodeport2net_map[io_name + "_CONNECTOR_"].push_back(port2net_t("i", io_name));
            debug_vparse_verbose_printf("IOO  module_map[%s].nodeport2net_map[%s].push_back( %s -> %s )\n",module_name.c_str(),(io_name + "_CONNECTOR_").c_str(),"i",(io_name).c_str());
        }
    }
    
    //exit(0);
    
    
    
    
    connect_gates(graph, &module_map, module_name, "DUT");
    
    //check whole graph if we have gates gatename_x AND gatename_xN
    //-> remove gatename_xN and instanciate a NOT gate
    //thus change
    //gatename_xN--->Y to
    //gatename_x--->NOT(gatename_xN)--->Y
    vector<Node *> nodes_to_erase_vector;
    for(int i=0; i<graph->get_node_count(); i++){
        Node *node = graph->get_node_by_id(i);
        if (node != NULL){
            //check all inputs
            for(int j=0; j<node->get_input_node_count(); j++){
                Node *input = node->get_input(j);
                if (input != NULL){
                    string input_name = string(input->get_name());
                    if (input_name.substr(input_name.length()-1, input_name.length()) == "N"){
                        string invname = input_name.substr(0, input_name.length()-1);
                        Node *node_ninv = graph->get_node_by_name_quiet(invname);
                        if (node_ninv != NULL){
                            //whohoo there is also an non inverted version!
                            //replace input with node ---> INV ---> IN !
                            printf("replacing %s with a NOT(%s)\n",input->get_name(),node_ninv->get_name());
                            Node *node_not = new Node_NOT((invname + "_NOT").c_str());
                            node_not->set_input(node_ninv, "I");
                            //connect to in
                            node->set_input(node_not, j);
                            //mark old node to be erased
                            nodes_to_erase_vector.push_back(input);
                            graph->add_node(node_not);
                        }
                    }
                }
            }
        }
    }
    for (int i=0; i<nodes_to_erase_vector.size(); i++){
        graph->remove_node(nodes_to_erase_vector.at(i));
    }
    
    
    //erase unused nodes inverted nodes
    //(node is unused if no other node is using it as input)
    
//     //1) add all inverted nodes to the erase list
//     vector<Node *> nodes_to_erase_vector;
//     for(int i=0; i<graph->get_node_count(); i++){
//         Node *node = graph->get_node_by_id(i);
//         if (node != NULL){
//             string input_name = string(node->get_name());
//             if (input_name.substr(input_name.length()-1, input_name.length()) == "N"){
//                 nodes_to_erase_vector.push_back(node);
//             }
//         }
//     }
    
    //2) check if they are beeing used:
    
    //graph->check_consistency();
    
    ///graph->print_graphviz();
    
    //exit(0);
    
    //Node *t = graph->get_node_by_name("cu3232af_r_2_2269"); //cu3232af_n10138");
    //t->print_graphviz(0);
    
    //Radiator r;
    //int done = 0;
    //r.do_seu_search(graph, t, done);
    //exit(0);
    
    return graph;
    
}


connectionwire Verilog_Parser::binary_search_connectionwire_vector(vector<connectionwire> connectionwire_vector, string name)
{
    // Keep halfing the search space until we reach the end of the vector
    vector<connectionwire>::iterator Middle;
    vector<connectionwire>::iterator begin = connectionwire_vector.begin();
    vector<connectionwire>::iterator end   = connectionwire_vector.end();
    connectionwire cwire;
    
    //printf("\n\nlooking for '%s':\n",name.c_str());
    while(begin < end){
        // Find the median value between the iterators
        Middle = begin + (std::distance(begin, end) / 2);

        // Re-adjust the iterators based on the median value
        cwire = *Middle;
        
        //printf("\tchecking '%s' -> %d\n",cwire.netname.c_str(),strcmp(cwire.netname.c_str(), name.c_str()));
        if(strcmp(cwire.netname.c_str(), name.c_str()) == 0){
            return cwire;
        }else if(strcmp(cwire.netname.c_str(), name.c_str()) < 0){
            end = Middle;
        }else{
            begin = Middle + 1;
        }
    }
printf("NOT FOUND\n");
cwire.netname = "_not_found_";
cwire.port    = "_not_found_";
cwire.instance = "pci_perr_in_n";
    return cwire;
}

bool Verilog_Parser::compare_connectionwire_by_netname(connectionwire a, connectionwire b)
{
    if (strcmp(a.netname.c_str(), b.netname.c_str())>0){
        return true;
    }else{
        return false;
    }
}

void Verilog_Parser::tokenize_string(const string& str, vector<string>& tokens, const string& delimiters)
{
    // found on the web...
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


