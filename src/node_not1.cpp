/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by I on Thu Jan 24 13:58:25 2008

#include "node_not1.h"

//register to node factory
std::string Node_NOT1::node_factory_dummy = Node_factory<Node>::register_create_function("NOT1",Node_NOT1::create_function);
std::string Node_NOT1::node_factory_dummy2 = Node_factory<Node>::register_output("NOT1","ZN");

Node_NOT1::Node_NOT1(const char *name) : Node(name, 1){
    port_map_vector.push_back("I"); //0

}

Node_NOT1::~Node_NOT1()
{
}

std::string Node_NOT1::get_input_name(int i)
{
    switch(i){
        case(0): return("I");
        default: return "<?>";
    }

}

int Node_NOT1::get_value(int run)
{
    if (force_return_stored_value) {
//         printf("RET FORCED for %s => ret %d\n", get_name(),value);
        return value;
    }
    
//define some helpers, makes it easier to write function
    #define I input_nodes[0]->get_value(run)
    #define SC 0
    #define SD D
    #define ZN  value


    if (last_run != run){
        //need to recalc
        last_run_value = (!I);
        last_run = run;
    }
    return last_run_value;
}

std::string Node_NOT1::get_formula()
{
//define some helpers, makes it easier to write function
    std::string formula_I = input_nodes[0]->get_formula();
    std::string formula_0 = "FALSE";
    std::string formula_1 = "TRUE";

    if (is_safe){
        return "SAFE";
    }else{
        return std::string("(!" + formula_I + ")");
    }
}
