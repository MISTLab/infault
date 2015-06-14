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
#include "node_udp_mux21.h"

//register to node factory
std::string Node_UDP_MUX21::node_factory_dummy = Node_factory<Node>::register_create_function("UDP_MUX21",Node_UDP_MUX21::create_function);

Node_UDP_MUX21::Node_UDP_MUX21(const char *name) : Node(name, 3)
{
    port_map_vector.push_back("S"); 
    port_map_vector.push_back("I0"); 
    port_map_vector.push_back("I1");
}

Node_UDP_MUX21::~Node_UDP_MUX21()
{
}


int Node_UDP_MUX21::get_value(int run){
    if (force_return_stored_value) {
        return value;
    }
    if (last_run != run){
	//need to recalc
        if (input_nodes[0]->get_value(run)){
            last_run_value = (input_nodes[2])->get_value(run);
        }else{
            last_run_value = (input_nodes[1])->get_value(run);
        }
        last_run = run;
    }
    return last_run_value;
}

