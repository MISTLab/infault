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
#ifndef VERILOG_PARSER_H
#define VERILOG_PARSER_H

#include <parser.h>
#include <debug.h>
#include <parser.h>
#include <iostream>
#include <cstdlib>
#include <boost/regex.hpp>
#include <fstream>

using namespace std;
using namespace boost;


typedef struct{
    string instance;
    string port;
    string netname;
} connectionwire;

typedef struct{
    string node_type;
    string instance;
    string port_description;
    string submodule;
    string port;
} node_description;

class Verilog_Parser : public Parser
{
    public:
        enum iodescription_enum{ UNKNOWN, INPUT, OUTPUT};
        typedef std::pair<std::string,std::string> port2net_t;
        typedef std::vector<port2net_t> port2net_vector_t;
        typedef std::map<std::string, port2net_vector_t> nodename_map_t;
        typedef std::map<std::string, std::string> stringstring_map_t;
        typedef std::map<std::string, iodescription_enum> iodescription_map_t;
        
        
        
        typedef struct{
            stringstring_map_t net2node_map;
            stringstring_map_t node2type_map;
            nodename_map_t nodeport2net_map;
            iodescription_map_t iodescription_map;
            std::string module_name;
        } module_description;
        
        typedef std::map<std::string, module_description> moduledescription_map_t;
        
        Verilog_Parser();
        Verilog_Parser(const char *fname, const char *module);
        ~Verilog_Parser();
    
        
        Graph *get_graph();
        static bool compare_connectionwire_by_netname(connectionwire a, connectionwire b);
        connectionwire binary_search_connectionwire_vector(vector<connectionwire> connectionwire_vector, string name);
        void tokenize_string(const string& str, vector<string>& tokens, const string& delimiters = " ");
    
    private:
        std::string module_name;
        const char *filename;
        ifstream input_stream;
        void remove_newlines_and_tabs(char *line);
        int is_output_port(string s, string t);
        //string get_port_by_node_name(Graph *g, string nodename);
        void create_gates(Graph *graph, moduledescription_map_t *module_map, string submodule_name, string instance_name);
        void connect_gates(Graph *graph, moduledescription_map_t *module_map, string submodule_name, string instance_name);

};

#endif
