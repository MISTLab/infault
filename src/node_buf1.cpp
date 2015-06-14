/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by I on Thu Jan 24 13:58:24 2008

#include "node_buf1.h"

//register to node factory
std::string Node_BUF1::node_factory_dummy = Node_factory<Node>::register_create_function("BUF1",Node_BUF1::create_function);
std::string Node_BUF1::node_factory_dummy2 = Node_factory<Node>::register_output("BUF1","Z");

Node_BUF1::Node_BUF1(const char *name) : Node(name, 1){
    port_map_vector.push_back("I"); //0

}

Node_BUF1::~Node_BUF1()
{
}

std::string Node_BUF1::get_input_name(int i)
{
    switch(i){
        case(0): return("I");
        default: return "<?>";
    }

}

int Node_BUF1::get_value(int run)
{
    if (force_return_stored_value) {
        return value;
    }
    
//define some helpers, makes it easier to write function
    #define I input_nodes[0]->get_value(run)
    #define SC 0
    #define SD D
    #define Z  value


    if (last_run != run){
        //need to recalc
        last_run_value = (I);
        last_run = run;
    }
    return last_run_value;
}

std::string Node_BUF1::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_I = input_nodes[0]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(" + formula_I + ")");
    }
}
