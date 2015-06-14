/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by CLK on Thu Jan 24 13:58:24 2008

#include "node_atc18_clk2d_cn.h"

//register to node factory
std::string Node_ATC18_CLK2D_CN::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_CLK2D_CN",Node_ATC18_CLK2D_CN::create_function);
std::string Node_ATC18_CLK2D_CN::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_CLK2D","CN");

Node_ATC18_CLK2D_CN::Node_ATC18_CLK2D_CN(const char *name) : Node(name, 1){
    port_map_vector.push_back("CLK"); //0

}

Node_ATC18_CLK2D_CN::~Node_ATC18_CLK2D_CN()
{
}

std::string Node_ATC18_CLK2D_CN::get_input_name(int i)
{
    switch(i){
        case(0): return("CLK");
        default: return "<?>";
    }

}

int Node_ATC18_CLK2D_CN::get_value(int run)
{
//define some helpers, makes it easier to write function
    #define CLK input_nodes[0]->get_value(run)
    #define SC 0
    #define SD D
    #define CN  value


    if (last_run != run){
        //need to recalc
        last_run_value = ((!CLK));
        last_run = run;
    }
    return last_run_value;
}

std::string Node_ATC18_CLK2D_CN::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_CLK = input_nodes[0]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("((!" + formula_CLK + "))");
    }
}
