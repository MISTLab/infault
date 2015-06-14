/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by 1 on Thu Jan 24 13:58:30 2008

#include "node_atc18_sdnrq_q.h"

//register to node factory
std::string Node_ATC18_SDNRQ_Q::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_SDNRQ_Q",Node_ATC18_SDNRQ_Q::create_function);
std::string Node_ATC18_SDNRQ_Q::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_SDNRQ","Q");

Node_ATC18_SDNRQ_Q::Node_ATC18_SDNRQ_Q(const char *name) : Node(name, 2){
    port_map_vector.push_back("CP"); //0
    port_map_vector.push_back("D"); //1

}

Node_ATC18_SDNRQ_Q::~Node_ATC18_SDNRQ_Q()
{
}

std::string Node_ATC18_SDNRQ_Q::get_input_name(int i)
{
    switch(i){
        case(0): return("CP");
        case(1): return("D");
        default: return "<?>";
    }

}

//NOTE: evaluate is different for FF elements!
//      -> for an evaluate call we actually calculate the values using the inputs
//         while get_value() just returns a value set before
//         we use this for example when having "loops" from the output FF to the 
//         input of this FF. (think about it ;) )
int Node_ATC18_SDNRQ_Q::evaluate()
{
//define some helpers, makes it easier to write function
    #define CP input_nodes[0]->get_value(run)
    #define D input_nodes[1]->get_value(run)
    #define SC 0
    #define SD D
    #define Q  value


    int run = last_eval++;
    last_run_value = (1?(1?(SC?SD:D):0):1);
    return last_run_value;
}

Node *Node_ATC18_SDNRQ_Q::get_ff_input_node()
{
    #define IN_CP input_nodes[0]
    #define IN_D input_nodes[1]
    return IN_D;

}

std::string Node_ATC18_SDNRQ_Q::get_formula()
{
	return std::string(get_name());
}
