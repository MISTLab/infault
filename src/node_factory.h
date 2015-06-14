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
#ifndef NODE_FACTORY_H
#define NODE_FACTORY_H

#include <map>
#include <string>
#include <vector>
/**
	@author Simon Schulz <Simon.Schulz@esa.int>
*/

template <typename _Baseclass, typename _predictor = std::less<std::string> >
class Node_factory{
public:
    Node_factory() {};
    ~Node_factory() {};

    typedef _Baseclass* (*create_function) (const char *name);
    typedef std::map<std::string, create_function, _predictor> _map_factory;
    typedef std::map<std::string, std::vector<std::string>, _predictor> _map_outputs;
    
    static std::string register_create_function(std::string id, create_function class_creator){
        //printf("insert %s\n",id.c_str());
        get_map_factory()->insert(std::pair<std::string, create_function>(id, class_creator));
        return id;
    }
    
    static std::string register_output(std::string id, std::string output){
        //printf("output insert %s %s\n",id.c_str(),output.c_str());
        typename _map_outputs::iterator it = get_map_outputs()->find(id);
        if (it != get_map_outputs()->end()){
            //found already, so add to vector
            it->second.push_back(output);
        }else{
            std::vector<std::string> newvect;
            newvect.push_back(output);
            get_map_outputs()->insert(std::pair<std::string, std::vector<std::string> >(id, newvect));
        }
        return id;
    }
    
    static int get_output_count(std::string id){
        //return how many outputs a given gate type has
        typename _map_outputs::iterator it = get_map_outputs()->find(id);
        if (it != get_map_outputs()->end()){
            return it->second.size();
        }else{
            printf("ERROR: get_output_count() class %s unknown! exiting\n", id.c_str());
            exit(-1);
        }
        return 0;
    }
    
    static std::string get_output_name(std::string id, int i){
        //return name of output
        if (i >= get_output_count(id)){
            printf("ERROR: get_output_name(%s, %d) requested output id too high. exiting\n", id.c_str(), i);
            exit(-1);
        }
        
        typename _map_outputs::iterator it = get_map_outputs()->find(id);
        if (it != get_map_outputs()->end()){
            return it->second.at(i);
        }else{
            printf("ERROR: get_output_name(%s, %d) class %s unknown! exiting\n",id.c_str(), i,  id.c_str());
            exit(-1);
        }
        return "??";
    }
    
    static _Baseclass* create_instance(std::string id, const char *name){
        typename _map_factory::iterator it = get_map_factory()->find(id);
        //printf("create %s\n",id.c_str());
        if (it != get_map_factory()->end()){
            //printf("iterate\n");
            if (it->second){
                //get main node
                _Baseclass *main_node = it->second(name);
                return main_node;
            }
        }
        return NULL;
    }

protected:
    static _map_factory *get_map_factory(){
        static _map_factory m_s_map_factory;
        return &m_s_map_factory;
    };
    
    static _map_outputs *get_map_outputs(){
        static _map_outputs m_s_map_outputs;
        return &m_s_map_outputs;
    };
};


#endif
