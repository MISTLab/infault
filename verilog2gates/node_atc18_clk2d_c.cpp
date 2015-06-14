/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by CLK on Thu Jan 24 13:58:24 2008

#include "node_atc18_clk2d_c.h"

//register to node factory
std::string Node_ATC18_CLK2D_C::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_CLK2D_C",Node_ATC18_CLK2D_C::create_function);
std::string Node_ATC18_CLK2D_C::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_CLK2D","C");

Node_ATC18_CLK2D_C::Node_ATC18_CLK2D_C(const char *name) : Node(name, 1){
    port_map_vector.push_back("CLK"); //0

}

Node_ATC18_CLK2D_C::~Node_ATC18_CLK2D_C()
{
}

std::string Node_ATC18_CLK2D_C::get_input_name(int i)
{
    switch(i){
        case(0): return("CLK");
        default: return "<?>";
    }

}

int Node_ATC18_CLK2D_C::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define CLK input_nodes[0]->get_value(run)
    #define SC 0
    #define SD D
    #define C  value


    if (last_run != run){
        //need to recalc
        last_run_value = (!(!CLK));
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_CLK2D_C::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_CLK = input_nodes[0]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(!(!" + formula_CLK + "))");
    }
}
