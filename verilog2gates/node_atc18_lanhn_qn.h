/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by 1 on Thu Jan 24 13:58:26 2008

#ifndef NODE_ATC18_LANHN_QN_H
#define NODE_ATC18_LANHN_QN_H
#include "node.h"

class Node_ATC18_LANHN_QN : public Node
{
public:
    Node_ATC18_LANHN_QN(const char *name = "?");
    ~Node_ATC18_LANHN_QN();

    static Node* create_function(const char *name){ return new Node_ATC18_LANHN_QN(name); }
    static std::string node_factory_dummy;
    static std::string node_factory_dummy2;

    std::string get_input_name(int i);
    std::string get_formula();
    int is_ff(){return 1;};
    Node *get_ff_input_node();

protected:
        int evaluate();
    int get_value(int run = 0);
};

#endif

