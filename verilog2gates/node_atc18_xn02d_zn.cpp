/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by A2 on Thu Jan 24 13:58:30 2008

#include "node_atc18_xn02d_zn.h"

//register to node factory
std::string Node_ATC18_XN02D_ZN::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_XN02D_ZN",Node_ATC18_XN02D_ZN::create_function);
std::string Node_ATC18_XN02D_ZN::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_XN02D","ZN");

Node_ATC18_XN02D_ZN::Node_ATC18_XN02D_ZN(const char *name) : Node(name, 2){
    port_map_vector.push_back("A1"); //0
    port_map_vector.push_back("A2"); //1

}

Node_ATC18_XN02D_ZN::~Node_ATC18_XN02D_ZN()
{
}

std::string Node_ATC18_XN02D_ZN::get_input_name(int i)
{
    switch(i){
        case(0): return("A1");
        case(1): return("A2");
        default: return "<?>";
    }

}

int Node_ATC18_XN02D_ZN::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define A1 input_nodes[0]->get_value(run)
    #define A2 input_nodes[1]->get_value(run)
    #define SC 0
    #define SD D
    #define ZN  value


    if (last_run != run){
        //need to recalc
        last_run_value = (A1 == A2);
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_XN02D_ZN::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_A1 = input_nodes[0]->get_formula();
    std::string formula_A2 = input_nodes[1]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(" + formula_A1 + " == " + formula_A2 + ")");
    }
}
