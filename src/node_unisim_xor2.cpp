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
#include "node_unisim_xor2.h"

//register to node factory
std::string Node_UNISIM_XOR2::node_factory_dummy = Node_factory<Node>::register_create_function("UNISIM_XOR2",Node_UNISIM_XOR2::create_function);

Node_UNISIM_XOR2::Node_UNISIM_XOR2(const char *name) : Node(name, 2)
{
    port_map_vector.push_back("LI"); //0
    port_map_vector.push_back("CI"); //1
}
    
Node_UNISIM_XOR2::~Node_UNISIM_XOR2()
{
}
    
    
int Node_UNISIM_XOR2::get_value(int run){
    if (force_return_stored_value) {
        return value;
    }
    //return (input_nodes[1])->get_value(run) ^ (input_nodes[0])->get_value(run);
    if (run != last_run){
        last_run_value = (input_nodes[1])->get_value(run) ^ (input_nodes[0])->get_value(run);
        last_run = run;
    }
    return last_run_value;
}

