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
#include "node_output.h"

//register to node factory
std::string Node_OUTPUT::node_factory_dummy = Node_factory<Node>::register_create_function("OUTPUT",Node_OUTPUT::create_function);

Node_OUTPUT::Node_OUTPUT(const char *name) : Node(name, 1)
{
    Node *dummy = new Node_INPUT("UNASSIGNED_OUTPUT");
    set_input(dummy, 0);
    port_map_vector.push_back("D");
}

Node_OUTPUT::~Node_OUTPUT()
{
}

int Node_OUTPUT::get_value(int run){
    if (force_return_stored_value) {
        return value;
    }
    //dont do run based decision, this is faster than the if clause i think
    return 1 - (input_nodes[0])->get_value(run);
}
