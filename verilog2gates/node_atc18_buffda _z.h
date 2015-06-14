/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by I on Thu Jan 24 13:58:24 2008

#ifndef NODE_ATC18_BUFFDA _Z_H
#define NODE_ATC18_BUFFDA _Z_H
#include "node.h"

class Node_ATC18_BUFFDA _Z : public Node
{
public:
    Node_ATC18_BUFFDA _Z(const char *name = "?");
    ~Node_ATC18_BUFFDA _Z();

    static Node* create_function(const char *name){ return new Node_ATC18_BUFFDA _Z(name); }
    static std::string node_factory_dummy;
    static std::string node_factory_dummy2;

    std::string get_input_name(int i);
    std::string get_formula();
    int get_value(int run = 0);    int is_buffer(){return 1;};

};

#endif

