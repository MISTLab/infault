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
#ifndef GRAPH_H
#define GRAPH_H

#include "node_include.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <debug.h>


/**
	@author Simon Schulz <Simon.Schulz@esa.int>
*/

typedef struct{
        std::string instance;
        std::string port;
} wire;

class Graph{

public:
    Graph();
    ~Graph();
    
    Node *get_node_by_name(std::string name, int quiet=0);
    Node *get_node_by_name_quiet(std::string name);
    Node *get_node_by_id(int id);
    int get_node_count(void);
    Node *get_buffer_source(Node *buf);
    int remove_buffers(void);
    int optimize_muxes(void);
    int optimize_duplicates_to_ffs(void);
    int optimize_double_inverters(void);
    
    int check_for_functional_identical(Node *a, Node *b);
    
    
    Node *create_node(std::string library,  std::string instance, std::string celltype, int initvalue);
    Node *create_node_UNISIMS(std::string instance, std::string celltype, int initvalue);
    Node *create_node_VERILOG(std::string instance, std::string celltype, int initvalue);
    int is_child_node(Node *child);
    void prepare_connection(wire w); //string instance, string port);
    void execute_connections();
    void add_node(Node *node);
    void remove_node(Node *node);
    void print_graphviz();
    int check_consistency();
    void sort_node_vector();
    void sort_node_vector_by_ff_child_count();
    
    Node *binary_search_node_vector(std::string name);
    std::vector <Node *> get_child_ff_nodes(Node *p);
    
    Node *create_ram_node_UNISIM(std::string instance, std::string celltype, int initvalue, int addra_len, int addrb_len, int dia_len, int dib_len, int dipa_len, int dipb_len, int doa_len, int dopa_len, int dob_len, int dopb_len);
    int get_node_vector_ff_count();
            
private:
    std::vector <Node *> _get_child_ff_nodes(Node *p);
    std::vector <Node *> node_vector;
    std::vector <Node *> node_printed_vector;
    std::vector <wire> conn_vector;
    int node_vector_is_sorted;
};

#endif
