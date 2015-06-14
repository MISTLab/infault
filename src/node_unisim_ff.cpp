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
#include "node_unisim_ff.h"

//register to node factory
std::string Node_UNISIM_FF::node_factory_dummy = Node_factory<Node>::register_create_function("UNISIM_FF",Node_UNISIM_FF::create_function);

Node_UNISIM_FF::Node_UNISIM_FF(const char *name) : Node(name, 1)
{
    port_map_vector.push_back("D");
}

Node_UNISIM_FF::~Node_UNISIM_FF()
{
}
    
//NOTE: evaluate is different for FF elements!
//      -> for an evaluate call we actually calculate the values using the inputs
//         while get_value() just returns a value set before
//         we use this for example when having "loops" from the output FF to the 
//         input of this FF. (think about it ;) )
int Node_UNISIM_FF::evaluate()
{
    last_eval++;
    return (input_nodes[0])->get_value(last_eval);
}

