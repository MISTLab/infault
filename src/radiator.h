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
#ifndef RADIATOR_H
#define RADIATOR_H
#include "graph.h"
#include "node_include.h"
#include <vector> 
/**
	@author Simon Schulz <Simon.Schulz@esa.int>
*/
class Radiator{
public:
    Radiator();
    ~Radiator();
    int do_seu_search(Graph *g, Node *node);
    void do_seu_search(Graph *g);
    
    Node *create_dummy_triplet(std::string s);
    void _recursive_split_special(Node *node, std::vector<Node *> *result, int rek=0);
    void _recursive_split(Node *node, std::vector<Node *> *result, int rek=0);
    std::vector<Node *> recursive_split(Node *node, int rek=0);
    int do_seu_test_for_node(Graph *g, Node *target);
//     int _do_seu_test_for_node(Graph *g, Node *target);
    static int compare_nodes_for_equality(Node *node_a, Node *node_b);
    void log_sensitive_ff(Node *n);

private:
    int seu_sensitive_ff_count;
    std::vector <Node*> garbage_vector;
    int ffs_done;
    int stat_seu_sensitive_ffs;
    int stat_splitcounter;
    int stat_subgraph_size_after_split[4096];
    int stat_subgraph_size_before_split[4096];
    std::vector<Node *> stat_sensitive_ffs;
    
};

#endif
