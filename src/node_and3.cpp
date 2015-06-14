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
#include "node_and3.h"

std::string Node_AND3::node_factory_dummy = Node_factory<Node>::register_create_function("AND3",Node_AND3::create_function);


Node_AND3::Node_AND3(const char *name) : Node(name, 3)
{
}

Node_AND3::~Node_AND3()
{
}

int Node_AND3::get_value(int run){
    //printf("GET VAL %s\n",get_name());
    if (force_return_stored_value) {
        return value;
    }
    
    if (run != last_run){
        last_run_value = (input_nodes[2])->get_value(run) & (input_nodes[1])->get_value(run) & (input_nodes[0])->get_value(run);
        last_run = run;
//         printf("AND3::%s.getvalue(%d)=%d & %d & %d = %d recalc\n",get_name(),run,              (input_nodes[0])->get_value(run),                (input_nodes[1])->get_value(run),                 (input_nodes[2])->get_value(run),                  last_run_value);
//     }else{
//         printf("AND3::%s.getvalue(%d)=%d saved\n",get_name(),run,last_run_value);
    }
    return last_run_value;
}
