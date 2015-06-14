/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by 1 on Thu Jan 24 13:58:30 2008

#include "node_udp_dff.h"

//register to node factory
std::string Node_UDP_DFF::node_factory_dummy = Node_factory<Node>::register_create_function("UDP_DFF",Node_UDP_DFF::create_function);
std::string Node_UDP_DFF::node_factory_dummy2 = Node_factory<Node>::register_output("UDP_DFF","Q");

Node_UDP_DFF::Node_UDP_DFF(const char *name) : Node(name, 5)
{
    port_map_vector.push_back("D");
    port_map_vector.push_back("CP");
    port_map_vector.push_back("SDN");
    port_map_vector.push_back("CDN");
    port_map_vector.push_back("NOTIFIER");

}

Node_UDP_DFF::~Node_UDP_DFF()
{
}

std::string Node_UDP_DFF::get_input_name(int i)
{
    switch(i){
        case(0): return("D");
        case(1): return("CP");
        case(2): return("SDN");
        case(3): return("CDN");
        case(4): return("NOTIFIER");
        default: return "<?>";
    }

}

//NOTE: evaluate is different for FF elements!
//      -> for an evaluate call we actually calculate the values using the inputs
//         while get_value() just returns a value set before
//         we use this for example when having "loops" from the output FF to the 
//         input of this FF. (think about it ;) )
int Node_UDP_DFF::evaluate()
{
    //define some helpers, makes it easier to write function
    int run = last_eval++;
    
    #define D   input_nodes[0]->get_value(run)
    #define CP  input_nodes[1]->get_value(run)
    #define SDN input_nodes[2]->get_value(run)
    #define CDN input_nodes[3]->get_value(run)
    #define NOTIFIER input_nodes[4]->get_value(run)

    //ignore clock and notifier
    if (!SDN){
        last_run_value = 1; 
    }else if (!CDN){
        last_run_value = 0; 
    }else{
        last_run_value = D;
    }
    
//     printf("EVAL %s(%d) = %d\n",get_name(),last_eval, last_run_value);
    
    return last_run_value;
}

Node *Node_UDP_DFF::get_ff_input_node()
{
    #define IN_CP input_nodes[1]
    #define IN_D input_nodes[0]
    return IN_D;

}

std::string Node_UDP_DFF::get_formula()
{
	return std::string(get_name());
}
