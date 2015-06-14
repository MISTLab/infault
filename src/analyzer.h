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
#ifndef ANALYZER_H
#define ANALYZER_H

#include "graph.h"
#include "node_include.h"
#include <vector> 
/**
	@author Simon Schulz <Simon.Schulz@esa.int>
*/
class Analyzer{
public:
    Analyzer();
    ~Analyzer();
    int analyze_node(Graph *g, Node *node, int ffs_done=0);
    void analyze(Graph *g);
    //void print_node_id(Node *n, std::string s="");
    //void print_node_id_int(Node *n, long long id=0);
    int closer_match(std::string target, std::string a, std::string b);
    //int id_distance(Node *a, Node *b);
    void show_activity_icon(int count);
    //void _recursive_split_special(Node *node, std::vector<Node *> *result, int rek=0);
    //void _recursive_split(Node *node, std::vector<Node *> *result, int rek=0);
    //std::vector<Node *> recursive_split(Node *node, int rek=0);
    //int do_seu_test_for_node(Graph *g, Node *target);

};

#endif
