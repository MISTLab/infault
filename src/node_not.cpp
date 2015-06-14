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
#include "node_not.h"

//register to node factory
std::string Node_NOT::node_factory_dummy = Node_factory<Node>::register_create_function("NOT",Node_NOT::create_function);

Node_NOT::Node_NOT(const char *name): Node(name, 1){
    port_map_vector.push_back("I");
}

Node_NOT::~Node_NOT()
{
}

int Node_NOT::get_value(int run){
    //dont do run based decision, this is faster than the if clause i think
   if (force_return_stored_value) {
        return value;
   }
   return 1 - (input_nodes[0])->get_value(run);
}

std::string Node_NOT::get_formula()
{
    return std::string("!")+input_nodes[0]->get_formula();
}

