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
#include "serializer.h"
#include "node.h"
#include "node_factory.h"
#include <cstdlib>
#include <fstream>

using namespace std;

Serializer::Serializer()
{
}


Serializer::~Serializer()
{
}

//store a given graph as human readable data to a file
void Serializer::save_graph_to_file(Graph *graph, string filename)
{
    FILE *pFile;
    pFile = fopen(filename.c_str(),"w+");
    
    //store file version info:
    fprintf(pFile, "I 1 infault_human_readable_dump_file\n");
    
    //first step: store all nodes to file:
    //format:
    // N <node_id> <node_classname> <node_name>\n
    
    for(int i=0; i<graph->get_node_count(); i++){
        Node *node = graph->get_node_by_id(i);
        
        if (node == NULL){
            printf("NULL NODE. EXITING.\n");
            exit(-1);
        }
        
        //extract class name
        const char *classname = typeid(*node).name();
        classname = strchr(classname, '_') + 1;
        //write to file
        fprintf(pFile, "N %d %s %s\n", i, classname, node->get_name());
        
        //assign an unique node id
        node->set_node_id(i);
            
    }
    
    //now write the connections
    //format:
    // L <node_id> <input id> <node_target_id>\n
    for(int i=0; i<graph->get_node_count(); i++){
        Node *node = graph->get_node_by_id(i);
        if (node == NULL){
            printf("NULL NODE. EXITING.\n");
            exit(-1);
        }
        
        //ok get all outputs 
        for(int j=0; j<node->get_input_node_count(); j++){
            Node *in = node->get_input(j);
            if (in == NULL){
                printf("NULL INPUT, EXIT\n");
                exit(-1);
            }
            
            int in_node_id = in->get_node_id();
            
            if (in_node_id == -1){
                printf("INVALID IN NODE %s-->%s! NOT IN GRAPH! EXIT\n", in->get_name(), node->get_name());
                exit(-1);
            }
            
            //write to file
            fprintf(pFile, "L %d i:%d %d\n", i, j, in_node_id); //i: -> integer value, might also be s: -> string info
        }
    }
    fclose (pFile);
}

//load a graph from human readable data
void Serializer::load_graph_from_file(Graph *graph, string filename)
{
    FILE *pFile;
    int count=0;
    int version;
    char *buf[1024];
    
    //open file
    pFile = fopen(filename.c_str(),"r+");
    
    
    //get filesize
    fseek(pFile, 0L, SEEK_END);
    int maxpos = ftell(pFile);
    fseek(pFile, 0L, SEEK_SET);
    
    //read file version
    fscanf (pFile, "I %d %s\n", &version, buf);
    
    if (version != 1){
        printf("WRONG FILE VERSION. EXIT\n");
        exit(-1);
    }
    //ok, show file info
    printf("> file open ok, version %d '%s'\n",version,buf);
    
    
    //create all objects
    int node_id;
    char node_input_id_type;
    int node_input_id;
    char node_input_id_string[4]; //3 chars + \0
    int node_target_id;
    char class_name[1024];
    char node_name[1024];
    
    
    while(!feof(pFile)){
        if (count++ == 2048){
            printf("                                   \r> parsing file %3d%% done  ",ftell(pFile)*100/maxpos);
            cout.flush();
            count=0;
        }
        
        char type = fgetc(pFile);
        
        if (type == 'N'){
            //ok, new node line N <node_id> <node_classname> <node_name>\n
            //read node id:
            fscanf(pFile, " %d ", &node_id);
            //read class name
            fscanf(pFile, "%s", &class_name);
            //read node name
            fscanf(pFile, "%s", &node_name);
            /*
            printf("node id    %d\n",node_id);
            printf("node class %s\n",class_name);
            printf("node name  %s\n",node_name);
            */
            //read newline
            fgetc(pFile);
            
            //create node
            string class_name_s;
            class_name_s = class_name;
            Node *node = Node_factory<Node>::create_instance(class_name_s, node_name);
            
            if (node == NULL){
                printf("ERROR CREATING NODE WITH TYPE %s. EXIT\n",class_name);
                exit(-1);
            }
            
            //remove all connected nodes (for example outputs nodes have dummy ios connected)
            for(int j=0; j<node->get_input_node_count(); j++){
                node->set_input(NULL, j);
            }
            
            //printf("created %s\n",node->get_name());
            if (graph->get_node_count() != node_id){
                printf("ERROR: expected id %d, got %d\nNode_ids have to be in a row (0,1,2,3....)! not the case. exit\n",graph->get_node_count(),node_id);
                exit(-1);
            }
            graph->add_node(node);
            
            
        }else if (type == 'L'){
            //ok, link cmd line L <node_id> <input id> <node_target_id>\n
            //read node id:
            fscanf(pFile, " %d", &node_id);
	    //read input id type
	    fscanf(pFile, " %c:", &node_input_id_type);
            //read input id:
	    if (node_input_id_type == 's'){
	    	//string describing input:
		fscanf(pFile, "%s3", &node_input_id_string);
		node_input_id = -1;
	    }else{
	        //direct integer
                fscanf(pFile, "%d", &node_input_id);
	    }
            //read target id"
            fscanf(pFile, "%d", &node_target_id);
            //read newline
            fgetc(pFile);
            
	    /*
            if (node_input_id == -1){
	    	printf("need to connect %d --> %d:%s \n",node_target_id,node_id, node_input_id_string);
	    }else{
	        printf("need to connect %d --> %d:%d \n",node_target_id,node_id, node_input_id);
	    }*/
            
            Node *source = graph->get_node_by_id(node_id);
            Node *target = graph->get_node_by_id(node_target_id);
            
            if ((source == NULL) || (target == NULL)){
                printf("SRC OR TARGET NULL. EXIT\n");
                exit(-1);
            }
            
            if (node_input_id != -1){
	    	//oh we found an integer
	    	source->set_input(target,node_input_id);
	    }else{
	    	//input was given as string
		source->set_input(target,node_input_id_string);
	    }



            //printf("%s:%d <-- %s\n",source->get_name(),node_input_id,target->get_name());
	}else if (type == '#'){
	    //ignore comments
        }else if (type == -1){
            //ignore last line
        }else{
            printf("INVALID LINE ID '%c'\n",type);
        }
    }
    
    printf("\n> done.\n");
    
    fclose(pFile);
}
