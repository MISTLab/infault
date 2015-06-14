/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by C3 on Thu Jan 24 13:58:30 2008

#include "node_atc18_ora311d_z.h"

//register to node factory
std::string Node_ATC18_ORA311D_Z::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_ORA311D_Z",Node_ATC18_ORA311D_Z::create_function);
std::string Node_ATC18_ORA311D_Z::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_ORA311D","Z");

Node_ATC18_ORA311D_Z::Node_ATC18_ORA311D_Z(const char *name) : Node(name, 5){
    port_map_vector.push_back("A"); //0
    port_map_vector.push_back("C3"); //1
    port_map_vector.push_back("B"); //2
    port_map_vector.push_back("C2"); //3
    port_map_vector.push_back("C1"); //4

}

Node_ATC18_ORA311D_Z::~Node_ATC18_ORA311D_Z()
{
}

std::string Node_ATC18_ORA311D_Z::get_input_name(int i)
{
    switch(i){
        case(0): return("A");
        case(1): return("C3");
        case(2): return("B");
        case(3): return("C2");
        case(4): return("C1");
        default: return "<?>";
    }

}

int Node_ATC18_ORA311D_Z::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define A input_nodes[0]->get_value(run)
    #define C3 input_nodes[1]->get_value(run)
    #define B input_nodes[2]->get_value(run)
    #define C2 input_nodes[3]->get_value(run)
    #define C1 input_nodes[4]->get_value(run)
    #define SC 0
    #define SD D
    #define Z  value


    if (last_run != run){
        //need to recalc
        last_run_value = (A && B && (C1 || C2 || C3));
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_ORA311D_Z::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_A = input_nodes[0]->get_formula();
    std::string formula_C3 = input_nodes[1]->get_formula();
    std::string formula_B = input_nodes[2]->get_formula();
    std::string formula_C2 = input_nodes[3]->get_formula();
    std::string formula_C1 = input_nodes[4]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(" + formula_A + " && " + formula_B + " && (" + formula_C1 + " || " + formula_C2 + " || " + formula_C3 + "))");
    }
}
