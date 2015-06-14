/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by C2 on Thu Jan 24 13:58:23 2008

#include "node_atc18_aoi222p_zn.h"

//register to node factory
std::string Node_ATC18_AOI222P_ZN::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_AOI222P_ZN",Node_ATC18_AOI222P_ZN::create_function);
std::string Node_ATC18_AOI222P_ZN::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_AOI222P","ZN");

Node_ATC18_AOI222P_ZN::Node_ATC18_AOI222P_ZN(const char *name) : Node(name, 6){
    port_map_vector.push_back("B1"); //0
    port_map_vector.push_back("A1"); //1
    port_map_vector.push_back("B2"); //2
    port_map_vector.push_back("A2"); //3
    port_map_vector.push_back("C2"); //4
    port_map_vector.push_back("C1"); //5

}

Node_ATC18_AOI222P_ZN::~Node_ATC18_AOI222P_ZN()
{
}

std::string Node_ATC18_AOI222P_ZN::get_input_name(int i)
{
    switch(i){
        case(0): return("B1");
        case(1): return("A1");
        case(2): return("B2");
        case(3): return("A2");
        case(4): return("C2");
        case(5): return("C1");
        default: return "<?>";
    }

}

int Node_ATC18_AOI222P_ZN::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define B1 input_nodes[0]->get_value(run)
    #define A1 input_nodes[1]->get_value(run)
    #define B2 input_nodes[2]->get_value(run)
    #define A2 input_nodes[3]->get_value(run)
    #define C2 input_nodes[4]->get_value(run)
    #define C1 input_nodes[5]->get_value(run)
    #define SC 0
    #define SD D
    #define ZN  value


    if (last_run != run){
        //need to recalc
        last_run_value = (!((A1 && A2) || (B1 && B2) || (C1 && C2)));
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_AOI222P_ZN::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_B1 = input_nodes[0]->get_formula();
    std::string formula_A1 = input_nodes[1]->get_formula();
    std::string formula_B2 = input_nodes[2]->get_formula();
    std::string formula_A2 = input_nodes[3]->get_formula();
    std::string formula_C2 = input_nodes[4]->get_formula();
    std::string formula_C1 = input_nodes[5]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(!((" + formula_A1 + " && " + formula_A2 + ") || (" + formula_B1 + " && " + formula_B2 + ") || (" + formula_C1 + " && " + formula_C2 + ")))");
    }
}
