/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by C2 on Thu Jan 24 13:58:24 2008

#ifndef NODE_ATC18_AOR211D_Z_H
#define NODE_ATC18_AOR211D_Z_H
#include "node.h"

class Node_ATC18_AOR211D_Z : public Node
{
public:
    Node_ATC18_AOR211D_Z(const char *name = "?");
    ~Node_ATC18_AOR211D_Z();

    static Node* create_function(const char *name){ return new Node_ATC18_AOR211D_Z(name); }
    static std::string node_factory_dummy;
    static std::string node_factory_dummy2;

    std::string get_input_name(int i);
    std::string get_formula();
    int get_value(int run = 0);
};

#endif

