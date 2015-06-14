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
#include "node_or4.h"

//register to node factory
std::string Node_OR4::node_factory_dummy = Node_factory<Node>::register_create_function("OR4",Node_OR4::create_function);

Node_OR4::Node_OR4(const char *name) : Node(name, 4)
{
}
    
Node_OR4::~Node_OR4()
{
}
    
int Node_OR4::get_value(int run){
    if (force_return_stored_value) {
        return value;
    }
    if (run != last_run){
    
        if (! (input_nodes[0])->get_value(run)){
            //0***
            if (! (input_nodes[1])->get_value(run)){
                //00**
		if (! (input_nodes[2])->get_value(run)){
		    //000*
		    if (! (input_nodes[3])->get_value(run)){
			//0000
			last_run_value = 0;
		    }else{
		    	//0001
			last_run_value = 1;
		    }
		}else{
		    //001*
		    last_run_value = 1;
		}
            }else{
                //01**
                last_run_value = 1;
            }
        }else{
            //1* -> result is 1 anyway, shorter to abort now
            last_run_value = 1;
        }
        last_run = run;
    }
    return last_run_value;
}

