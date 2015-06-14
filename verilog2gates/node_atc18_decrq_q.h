/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by 1 on Thu Jan 24 13:58:24 2008

#ifndef NODE_ATC18_DECRQ_Q_H
#define NODE_ATC18_DECRQ_Q_H
#include "node.h"

class Node_ATC18_DECRQ_Q : public Node
{
public:
    Node_ATC18_DECRQ_Q(const char *name = "?");
    ~Node_ATC18_DECRQ_Q();

    static Node* create_function(const char *name){ return new Node_ATC18_DECRQ_Q(name); }
    static std::string node_factory_dummy;
    static std::string node_factory_dummy2;

    std::string get_input_name(int i);
    std::string get_formula();
    int is_ff(){return 1;};
    Node *get_ff_input_node();

protected:
        int evaluate();

};

#endif
