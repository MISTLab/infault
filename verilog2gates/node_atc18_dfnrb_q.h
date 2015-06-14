/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by 1 on Thu Jan 24 13:58:25 2008

#ifndef NODE_ATC18_DFNRB_Q_H
#define NODE_ATC18_DFNRB_Q_H
#include "node.h"

class Node_ATC18_DFNRB_Q : public Node
{
public:
    Node_ATC18_DFNRB_Q(const char *name = "?");
    ~Node_ATC18_DFNRB_Q();

    static Node* create_function(const char *name){ return new Node_ATC18_DFNRB_Q(name); }
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

