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
#include "node_unisim_mux2cy.h"

//register to node factory
std::string Node_UNISIM_MUX2CY::node_factory_dummy = Node_factory<Node>::register_create_function("UNISIM_MUX2CY",Node_UNISIM_MUX2CY::create_function);

Node_UNISIM_MUX2CY::Node_UNISIM_MUX2CY(const char *name) : Node(name, 3)
{
    port_map_vector.push_back("DI"); //0
    port_map_vector.push_back("CI"); //1
    port_map_vector.push_back("S");  //2
}

Node_UNISIM_MUX2CY::~Node_UNISIM_MUX2CY()
{
}


int Node_UNISIM_MUX2CY::get_value(int run){
    if (force_return_stored_value) {
        return value;
    }
    if (last_run != run){
    //need to recalc
        if (input_nodes[2]->get_value(run)){
            last_run_value = (input_nodes[0])->get_value(run);
        }else{
            last_run_value = (input_nodes[1])->get_value(run);
        }
        last_run = run;
    }
    return last_run_value;
}

