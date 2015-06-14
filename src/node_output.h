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
#ifndef NODE_OUTPUT_H
#define NODE_OUTPUT_H

#include "node.h"
#include "node_input.h"

/**
	@author Simon Schulz <Simon.Schulz@esa.int>
*/
class Node_OUTPUT : public Node
{
public:
    
    Node_OUTPUT(const char *name = "?");
    ~Node_OUTPUT();
    int is_ff(){return 0;};
    int is_output(){return 1;};
    static Node* create_function(const char *name){ return new Node_OUTPUT(name); }
    static std::string node_factory_dummy;
    int get_value(int run=0);
};

#endif
