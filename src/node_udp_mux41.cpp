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
 *   59 Temple Place - Suite 330, Boston, MA  04111-1307, USA.             *
 ***************************************************************************/
#include "node_udp_mux41.h"

//register to node factory
std::string Node_UDP_MUX41::node_factory_dummy = Node_factory<Node>::register_create_function("UDP_MUX41",Node_UDP_MUX41::create_function);

Node_UDP_MUX41::Node_UDP_MUX41(const char *name) : Node(name, 6)
{
    port_map_vector.push_back("S0");
    port_map_vector.push_back("S1");  
    port_map_vector.push_back("I0"); 
    port_map_vector.push_back("I1");
    port_map_vector.push_back("I2");
    port_map_vector.push_back("I3");
}

Node_UDP_MUX41::~Node_UDP_MUX41()
{
}


int Node_UDP_MUX41::get_value(int run){
    if (force_return_stored_value) {
        return value;
    }
    if (last_run != run){
	//need to recalc
        if (input_nodes[0]->get_value(run)){
            if (input_nodes[1]->get_value(run)){
                //S0S1 = 11
                last_run_value = (input_nodes[2+3])->get_value(run);
            }else{
                //S0S1 = 10
                last_run_value = (input_nodes[2+2])->get_value(run);
            }
        }else{
            if (input_nodes[1]->get_value(run)){
                //S0S1 = 01
                last_run_value = (input_nodes[2+1])->get_value(run);
            }else{
                //S0S1 = 00
                last_run_value = (input_nodes[2+0])->get_value(run);
            }
        }
        last_run = run;
    }
    return last_run_value;
}

