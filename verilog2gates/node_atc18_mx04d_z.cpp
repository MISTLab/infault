/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by I0 on Thu Jan 24 13:58:27 2008

#include "node_atc18_mx04d_z.h"

//register to node factory
std::string Node_ATC18_MX04D_Z::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_MX04D_Z",Node_ATC18_MX04D_Z::create_function);
std::string Node_ATC18_MX04D_Z::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_MX04D","Z");

Node_ATC18_MX04D_Z::Node_ATC18_MX04D_Z(const char *name) : Node(name, 6){
    port_map_vector.push_back("S1"); //0
    port_map_vector.push_back("I2"); //1
    port_map_vector.push_back("I0"); //2
    port_map_vector.push_back("I3"); //3
    port_map_vector.push_back("S0"); //4
    port_map_vector.push_back("I1"); //5

}

Node_ATC18_MX04D_Z::~Node_ATC18_MX04D_Z()
{
}

std::string Node_ATC18_MX04D_Z::get_input_name(int i)
{
    switch(i){
        case(0): return("S1");
        case(1): return("I2");
        case(2): return("I0");
        case(3): return("I3");
        case(4): return("S0");
        case(5): return("I1");
        default: return "<?>";
    }

}

int Node_ATC18_MX04D_Z::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define S1 input_nodes[0]->get_value(run)
    #define I2 input_nodes[1]->get_value(run)
    #define I0 input_nodes[2]->get_value(run)
    #define I3 input_nodes[3]->get_value(run)
    #define S0 input_nodes[4]->get_value(run)
    #define I1 input_nodes[5]->get_value(run)
    #define SC 0
    #define SD D
    #define Z  value


    if (last_run != run){
        //need to recalc
        last_run_value = (S0?(S1?I3:I1):(S1?I2:I0));
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_MX04D_Z::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_S1 = input_nodes[0]->get_formula();
    std::string formula_I2 = input_nodes[1]->get_formula();
    std::string formula_I0 = input_nodes[2]->get_formula();
    std::string formula_I3 = input_nodes[3]->get_formula();
    std::string formula_S0 = input_nodes[4]->get_formula();
    std::string formula_I1 = input_nodes[5]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(" + formula_S0 + "?(" + formula_S1 + "?" + formula_I3 + ":" + formula_I1 + "):(" + formula_S1 + "?" + formula_I2 + ":" + formula_I0 + "))");
    }
}
