/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by I0 on Thu Jan 24 13:58:26 2008

#include "node_atc18_mx02d_z.h"

//register to node factory
std::string Node_ATC18_MX02D_Z::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_MX02D_Z",Node_ATC18_MX02D_Z::create_function);
std::string Node_ATC18_MX02D_Z::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_MX02D","Z");

Node_ATC18_MX02D_Z::Node_ATC18_MX02D_Z(const char *name) : Node(name, 3){
    port_map_vector.push_back("S"); //0
    port_map_vector.push_back("I0"); //1
    port_map_vector.push_back("I1"); //2

}

Node_ATC18_MX02D_Z::~Node_ATC18_MX02D_Z()
{
}

std::string Node_ATC18_MX02D_Z::get_input_name(int i)
{
    switch(i){
        case(0): return("S");
        case(1): return("I0");
        case(2): return("I1");
        default: return "<?>";
    }

}

int Node_ATC18_MX02D_Z::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define S input_nodes[0]->get_value(run)
    #define I0 input_nodes[1]->get_value(run)
    #define I1 input_nodes[2]->get_value(run)
    #define SC 0
    #define SD D
    #define Z  value


    if (last_run != run){
        //need to recalc
        last_run_value = (S?I1:I0);
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_MX02D_Z::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_S = input_nodes[0]->get_formula();
    std::string formula_I0 = input_nodes[1]->get_formula();
    std::string formula_I1 = input_nodes[2]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(" + formula_S + "?" + formula_I1 + ":" + formula_I0 + ")");
    }
}
