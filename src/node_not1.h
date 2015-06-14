/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by I on Thu Jan 24 13:58:25 2008

#ifndef NODE_NOT1_H
#define NODE_NOT1_H
#include "node.h"

class Node_NOT1 : public Node
{
public:
    Node_NOT1(const char *name = "?");
    ~Node_NOT1();

    static Node* create_function(const char *name){ return new Node_NOT1(name); }
    static std::string node_factory_dummy;
    static std::string node_factory_dummy2;

    std::string get_input_name(int i);
    std::string get_formula();
    int get_value(int run = 0);
    int is_inverter(){return 1;};
};

#endif

