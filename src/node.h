/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef NODE_H
#define NODE_H

#include "node_factory.h"

#include <stdio.h>
#include <typeinfo>
#include <vector>
#include <string>

//

/**
	@author Simon Schulz <Simon.Schulz@esa.int>
*/
class Node{
public:
    Node();
    Node(int n);
    Node(const char *name, int n);
    Node(const Node &node);
    ~Node();

    void set_input(Node *input, int n);
    void update_input(Node *input, int n);
    bool set_input(Node *input, std::string port);
    void set_name(const char *name);
    
    virtual int check_consistency();
    virtual int get_value(int run);
    virtual int evaluate();
    virtual void set_init(int n);
    virtual int get_lut_init(int i) const;
    Node *copy();
    
    virtual Node *get_ff_input_node(){ return input_nodes[0]; };
    
    int get_direct_ff_connection_count();
    int get_first_logic_connection_pos();
    void flip();
    void clear_printhistory(void);
    void set_value(int v);
    
    void set_force_return_stored_value(int v);
    
    void set_is_input(int i);
    int get_is_input();
    const char *get_name();
    int get_input_node_count();
    void print(const char* s = "", int rek = 0);
    void print_graphviz(int rek = 0);
    void print_graphviz_full(int rek = 0);
    void print_graphviz_this();
    Node *get_input(int n);
    Node *get_input(std::string s);
    
    virtual int is_ff(){return 0;};
    virtual int is_buffer(){return 0;};
    virtual int is_inverter(){return 0;};
    virtual int is_output(){return 0;};
    virtual int is_input(){return 0;};
    virtual int is_mux(){return 0;};
    virtual int is_constant(){return 0;};
    
    
    //removed flag
    int get_to_be_removed(void);
    void set_to_be_removed(int t);
    
    void mark_node(int id=-1);
    int get_node_mark(void);
    std::vector<Node *> find_marked_nodelist(int id);
    
    
    static bool compare_by_name(Node *a, Node *b);
    static bool compare_by_child_ff_count(Node *a, Node *b);
    static bool compare_by_child_address(Node *a, Node *b);
    static bool compare_by_address(Node *a, Node *b);

    int get_seu_check_done();
    void set_seu_check_done(int n);
    
    std::vector<Node *> _get_child_ff_nodes(Node *node, int unsafe_only);
    std::vector<Node *> get_child_ff_nodes();
    std::vector<Node *> get_unsafe_child_ff_nodes();
    
    std::vector<Node *> get_child_input_nodes();
    std::vector<Node *> _get_child_input_nodes(Node *node, int unsafe_only);
            
    
    static bool node_in_vector(std::vector<Node *> *vector, Node * node);
    static void safe_push(std::vector<Node *> *vector, Node * node);
    
    
    void set_child_ff_vector_valid(int i);
    
    void set_reset_node(Node *r);
    Node *get_reset_node();

    static Node* create_function(){ return new Node; }
    static std::string factory_dummy;
    void print_inputmap();
    
    virtual std::string get_input_name(int i){return "?";};
    
    //void recalc_node_id_rek(std::string s="");
    //std::string get_node_id();
    //void set_node_id(std::string s);
    int get_is_safe();
    void set_is_safe(int t);
    void set_triplet(int i, Node *n);
    Node *get_triplet(int i);
    int get_triplet_ok();
    
    virtual std::string get_formula();
    int get_is_processed();
    void set_is_processed(int i);
    void set_node_id(int id);
    int get_node_id();
    
    int get_is_deleteable_copy();
    void set_is_deleteable_copy(int i);
    
    static int last_eval;
protected:
    int node_id;
    int is_safe;
    int processed;
    int to_be_removed;
    int mark;
    int force_return_stored_value;
    int is_deleteable_copy;
    
    void _init(const char *name, int n);
    //std::string node_id;
    
    int input_nodes_count;
    int input_nodes_assigned;
    
    Node **input_nodes;
    Node *reset_node;
    Node *triplet_nodes[3];
    
    int is_input_node;
    int value;
    int last_printed;
    char *name;
    int seu_check_done;
    
    
    int last_run;
    int last_run_value;
    
    int child_ff_vector_valid;
    std::vector <Node *> child_ff_vector;

    
    int child_ff_vector_valid2;
    std::vector <Node *> child_ff_vector2;
    
    int child_input_vector_valid;
    std::vector <Node *> child_input_vector;
    
    std::vector<std::string> port_map_vector;
    
};



#endif
