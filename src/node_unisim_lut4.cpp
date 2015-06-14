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
#include "node_unisim_lut4.h"

//register to node factory
std::string Node_UNISIM_LUT4::node_factory_dummy = Node_factory<Node>::register_create_function("UNISIM_LUT4",Node_UNISIM_LUT4::create_function);

Node_UNISIM_LUT4::Node_UNISIM_LUT4(const char *name) : Node(name, 4){
    for(int i=0; i<16; i++){
        lut[i] = 0;
    }
        
    port_map_vector.push_back("I0");
    port_map_vector.push_back("I1");
    port_map_vector.push_back("I2");
    port_map_vector.push_back("I3");
}
    
Node_UNISIM_LUT4::Node_UNISIM_LUT4(const Node &node) : Node(node)
{
    for(int i=0; i<16; i++){
        lut[i] = node.get_lut_init(i);
    }
    
    port_map_vector.push_back("I0");
    port_map_vector.push_back("I1");
    port_map_vector.push_back("I2");
    port_map_vector.push_back("I3");
}

Node_UNISIM_LUT4::~Node_UNISIM_LUT4()
{
}

int Node_UNISIM_LUT4::get_value(int run)
{
    if (force_return_stored_value) {
        return value;
    }
    if (run != last_run){
        //recalc
        int lut_pos = 
                    ((input_nodes[3])->get_value(run) << 3) |
                    ((input_nodes[2])->get_value(run) << 2) |
                    ((input_nodes[1])->get_value(run) << 1) |
                    ((input_nodes[0])->get_value(run) << 0) ;
    
        last_run_value = lut[lut_pos];
        last_run = run;
        //        printf("LUT4::%s.getvalue(%d)=%d recalc\n",get_name(),run,last_run_value);
        //    }else{
        //        printf("LUT4::%s.getvalue(%d)=%d saved\n",get_name(),run,last_run_value);
    }

    return last_run_value;
}

void Node_UNISIM_LUT4::set_init(int v){
    for(int i=0; i<16; i++){
        lut[i] = (v & (1 << i)) >> i;
    }
};
