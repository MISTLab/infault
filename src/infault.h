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
#ifndef INFAULT_H
#define INFAULT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <boost/regex.hpp>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>

#include "graph.h"
#include "parser.h"
#include "edif_parser.h"

using namespace std;
using namespace boost;

enum filetype_enum{ FILETYPE_EDIF, FILETYPE_VERILOG, FILETYPE_INTERMEDIATE, FILETYPE_NOFILE};

filetype_enum  detect_filetype(std::string s);

int do_seu_test_for_node(Graph *g, Node *target);
bool node_in_vector(vector<Node *> *vector, Node * node);
void safe_push(vector<Node *> *vector, Node * node);
void _recursive_split_special(Node *node, vector<Node *> *result, int rek=0);
void _recursive_split(Node *node, vector<Node *> *result, int rek=0);
vector<Node *> recursive_split(Node *node, int rek=0);
int main(int argc, char *argv[]);

#endif

