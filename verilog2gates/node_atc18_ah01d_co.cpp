/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by A on Thu Jan 24 13:58:23 2008

#include "node_atc18_ah01d_co.h"

//register to node factory
std::string Node_ATC18_AH01D_CO::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_AH01D_CO",Node_ATC18_AH01D_CO::create_function);
std::string Node_ATC18_AH01D_CO::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_AH01D","CO");

Node_ATC18_AH01D_CO::Node_ATC18_AH01D_CO(const char *name) : Node(name, 2){
    port_map_vector.push_back("A"); //0
    port_map_vector.push_back("B"); //1

}

Node_ATC18_AH01D_CO::~Node_ATC18_AH01D_CO()
{
}

std::string Node_ATC18_AH01D_CO::get_input_name(int i)
{
    switch(i){
        case(0): return("A");
        case(1): return("B");
        default: return "<?>";
    }

}

int Node_ATC18_AH01D_CO::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define A input_nodes[0]->get_value(run)
    #define B input_nodes[1]->get_value(run)
    #define SC 0
    #define SD D
    #define CO  value


    if (last_run != run){
        //need to recalc
        last_run_value = (B && A);
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_AH01D_CO::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_A = input_nodes[0]->get_formula();
    std::string formula_B = input_nodes[1]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(" + formula_B + " && " + formula_A + ")");
    }
}
