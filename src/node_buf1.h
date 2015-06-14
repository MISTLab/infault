/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by I on Thu Jan 24 13:58:24 2008

#ifndef NODE_BUF1_H
#define NODE_BUF1_H
#include "node.h"

class Node_BUF1 : public Node
{
public:
    Node_BUF1(const char *name = "?");
    ~Node_BUF1();

    static Node* create_function(const char *name){ return new Node_BUF1(name); }
    static std::string node_factory_dummy;
    static std::string node_factory_dummy2;

    std::string get_input_name(int i);
    std::string get_formula();
    int get_value(int run = 0);    int is_buffer(){return 1;};

};

#endif

