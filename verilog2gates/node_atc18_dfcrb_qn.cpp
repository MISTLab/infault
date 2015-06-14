/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by 1 on Thu Jan 24 13:58:25 2008

#include "node_atc18_dfcrb_qn.h"

//register to node factory
std::string Node_ATC18_DFCRB_QN::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_DFCRB_QN",Node_ATC18_DFCRB_QN::create_function);
std::string Node_ATC18_DFCRB_QN::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_DFCRB","QN");

Node_ATC18_DFCRB_QN::Node_ATC18_DFCRB_QN(const char *name) : Node(name, 3){
    port_map_vector.push_back("CP"); //0
    port_map_vector.push_back("D"); //1
    port_map_vector.push_back("CDN"); //2

}

Node_ATC18_DFCRB_QN::~Node_ATC18_DFCRB_QN()
{
}

std::string Node_ATC18_DFCRB_QN::get_input_name(int i)
{
    switch(i){
        case(0): return("CP");
        case(1): return("D");
        case(2): return("CDN");
        default: return "<?>";
    }

}

//NOTE: evaluate is different for FF elements!
//      -> for an evaluate call we actually calculate the values using the inputs
//         while get_value() just returns a value set before
//         we use this for example when having "loops" from the output FF to the 
//         input of this FF. (think about it ;) )
int Node_ATC18_DFCRB_QN::evaluate()
{
//define some helpers, makes it easier to write function
    #define CP input_nodes[0]->get_value(run)
    #define D input_nodes[1]->get_value(run)
    #define CDN input_nodes[2]->get_value(run)
    #define SC 0
    #define SD D
    #define QN  value
    #define Q !(QN)


    int run = last_eval++;
    last_run_value = !((1?(CDN?D:0):1));
    return last_run_value;
}

Node *Node_ATC18_DFCRB_QN::get_ff_input_node()
{
    #define IN_CP input_nodes[0]
    #define IN_D input_nodes[1]
    #define IN_CDN input_nodes[2]
    return IN_D;

}

std::string Node_ATC18_DFCRB_QN::get_formula()
{
	return std::string(get_name());
}

int Node_ATC18_DFCRB_QN::get_value(int run)
{
    return !value;
}
