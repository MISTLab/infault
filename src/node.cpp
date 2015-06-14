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
#include "node.h"
#include "debug.h"
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

Node::Node()
{
    _init("unknown", 0);
}

Node::Node(int n)
{
    _init("unknown", n);
}

Node::Node(const char *_name, int n)
{
    _init(_name, n);
}


void Node::_init(const char *_name, int n)
{
    name = NULL;
    
    debug_verbose_printf(">> created Node '%s'\n", _name);
    //assign name
    set_name(_name);
    
    //initialise nodecounters
    input_nodes_count = n;
    input_nodes_assigned = 0;
    
    //allocate memory for child pointers
    if (input_nodes_count == 0){
        input_nodes = NULL;
    }else{
        input_nodes = new Node *[input_nodes_count];
        for(int i=0; i<input_nodes_count; i++){
           input_nodes[i] = NULL;
        }
    }
    
    //misc init
    value = 0;
    is_input_node = 0;
    last_printed = 0;
    seu_check_done = 0;
    
    //not safe per default
    is_safe = 0;
    
    
    last_run = -1;
    last_run_value = 0;
    
    child_ff_vector_valid = 0;
    child_ff_vector_valid2  = 0;
    
    reset_node = NULL;
    
    triplet_nodes[0] = NULL;
    triplet_nodes[1] = NULL;
    triplet_nodes[2] = NULL;
    
    processed = 0;
    node_id = -1;
    //
    to_be_removed = 0;
    mark = -1;
    force_return_stored_value = 0;
    //
    
    
    
    //NOTE: everything added here should be placed into Node(&Node) copy constructor below also !!!
    
    child_input_vector_valid = 0;
}

void Node::set_is_deleteable_copy(int i)
{
    is_deleteable_copy = i;
}

void Node::set_triplet(int i, Node *n)
{
    if ((i>0)||(i<3)){
        triplet_nodes[i] = n;
    }else{
        printf("set_triplet() node index %d out of bounds\n",i);
    }
}

Node *Node::get_triplet(int i)
{
    if ((i>0)||(i<3)){
        return triplet_nodes[i];
    }
    //else: invalid, return NULL
    return NULL;
}

int Node::get_triplet_ok()
{
    if ((triplet_nodes[0]==NULL)||(triplet_nodes[1]==NULL)||(triplet_nodes[2]==NULL)){
        return 0;
    }else{
        return 1;
    }
}

//removed flag
int Node::get_to_be_removed(void)
{
    return to_be_removed;
}

void Node::set_to_be_removed(int t)
{
    to_be_removed = t;
}

int Node::get_is_processed()
{
    return processed;
}

void Node::set_is_processed(int i)
{
    processed = i;
}

int Node::get_is_safe()
{
    return is_safe;
}

void Node::set_is_safe(int t)
{
    is_safe = t;
}

void Node::set_reset_node(Node *r)
{
    reset_node = r;
}

Node *Node::get_reset_node()
{
    return reset_node;
}



void Node::mark_node(int id)
{
    int first = 0;
    
    if (id == -1){
        id = (int)this;
        first = 1;
    }
    
    //mark children
    if ((first) || (!is_ff())){
        //if this is the first call _or_ NO FF
        
        for(int i=0; i<get_input_node_count(); i++){
            Node *n = get_input(i);
            n->mark_node(id);
        }
    }
    
    //set own mark
    mark = id;
    force_return_stored_value = 0;
}

void Node::set_force_return_stored_value(int s)
{
    force_return_stored_value = s;
}

int Node::get_node_mark(void)
{
    return mark;
}

vector<Node *> Node::find_marked_nodelist(int id)
{
    //follow the graph down until we find a marked node 
    vector<Node *> result;
    
   /// printf("find marked nodelist (%s)\n",get_name());
    
    for(int i=0; i<get_input_node_count(); i++){
        Node *n = get_input(i);
        if (n->get_node_mark() == id){
            //perfect, match -> add to vector
            ///printf("IN %s: %d ---> %s matched\n",get_name(),i,n->get_name());
            if (!n->is_constant()){
                //do not push constants, not necessary for tests
                result.push_back(n);
            }
        }else{
            ///printf("IN %s: %d NO match -> %d != %d\n",get_name(),i,mark,id);
            //hmm not what we look for, descend
            if (n->is_ff()){
                //oops, we reached a FF with different ID ?! impossible! 
                //we can abort (return) here, makes no sense to cont
                vector<Node *> empty;
                return empty;
            }
            
            //otherwise continue
            vector<Node *> result_child = n->find_marked_nodelist(id);
            if (result_child.size() == 0){
                //ooops empty -> found FF with diff id -> abort
                vector<Node *> empty;
                return empty;
            }else{
                //copy to result
                result.insert(result.end(), result_child.begin(), result_child.end());
            }
        }
    }
    
    //cleanup duplicate elements
    if (result.size() > 0){
        vector<Node *>::iterator start_pos(result.begin());
        vector<Node *>::iterator end_pos  (result.end());
        Node *current = *start_pos;
        while (++start_pos < end_pos){
            end_pos = remove(start_pos, end_pos, current);
            current = *start_pos;
        }
        result.erase( end_pos, result.end() );
    }

    
    return result;
}

void Node::set_name(const char *_name)
{
    //assign name
    if (name != NULL){
        free(name);
    }
    name = strdup(_name);
}

Node::Node(const Node &node)
{
    printf("\n\n\nWARNING: DO NOT USE THIS UNLESS YOU ARE 100% SURE!\nIT WILL CREATE A NODE WITH CLASS NODE AND NOT THE INHERITED NODE CLASS !! -> BAAAAAAAAAAAD THINGS HAPPEN !\n");
    
    _init(node.name, node.input_nodes_count);
    for(int i=0; i<input_nodes_count; i++){
        input_nodes[i] = node.input_nodes[i];
    }
    
    input_nodes_assigned = node.input_nodes_assigned;
    
    //misc init
    value = node.value;
    is_input_node = node.is_input_node;
    last_printed = node.last_printed;
    seu_check_done = node.seu_check_done;
    
    //not safe per default
    is_safe = node.is_safe;
    
    
    last_run = node.last_run;
    last_run_value = node.last_run_value;
    
    
    processed = node.processed;
    node_id = node.node_id;;
    
    //
    to_be_removed = node.to_be_removed;
    mark = node.mark;
    force_return_stored_value = node.force_return_stored_value;
    is_deleteable_copy = 1;
}


Node *Node::copy()
{
    const char *classname = typeid(*this).name();
    classname = strchr(classname, '_') + 1;
    
    //char *name_copy = strdup(get_name());
    
//     printf("DOING NODE COPY FOR %s @0x%X\n",get_name(),this);
    Node *node_new = Node_factory<Node>::create_instance(classname, get_name());
//     printf("RESULT IS  %s @0x%X\n",node_new->get_name(),node_new);
    
    for(int i=0; i<input_nodes_count; i++){
        node_new->set_input(input_nodes[i], i);
    }
    
    //misc init
    node_new->value = value;
    node_new->is_input_node = is_input_node;
    node_new->last_printed = last_printed;
    node_new->seu_check_done = seu_check_done;
    
    //not safe per default
    node_new->is_safe = is_safe;
    
    node_new->last_run = last_run;
    node_new->last_run_value = last_run_value;
    
    
    node_new->processed = processed;
    node_new->node_id = node_id;
    
    node_new->to_be_removed = to_be_removed;
    node_new->mark = mark;
    node_new->force_return_stored_value = force_return_stored_value;
    node_new->is_deleteable_copy = 1;
    return node_new;
}

int Node::get_is_deleteable_copy()
{
    return is_deleteable_copy;
}

Node::~Node()
{
//     printf("delete %s\n",get_name());
    if (input_nodes != NULL){
        delete [] input_nodes;
    }
    free(name);
}


void Node::set_seu_check_done(int i)
{
    seu_check_done = i;
}

int Node::get_seu_check_done()
{
    return seu_check_done;
}

void Node::set_input(Node *input, int n)
{
    if ((input_nodes_count == 0) || (n > input_nodes_count)){
        printf("WARNING: %s::set_input(%s, %d) argument n>%d out of bounds\n",get_name(),input->get_name(),n,input_nodes_count);
        return;
    }
    
    child_ff_vector_valid = 0;
    child_ff_vector_valid2 = 0;
    child_input_vector_valid = 0;
    
    if (input == NULL){
	if(input_nodes[n] != NULL) input_nodes_assigned--;
	input_nodes[n] = input;
	debug_verbose_printf("%s: connected %s::out to %s::in_<%d> (%d of %d inputs connected)\n",__PRETTY_FUNCTION__,"NULL",get_name(),n,input_nodes_assigned,input_nodes_count);
   }else{
       if (input_nodes[n] == NULL) input_nodes_assigned++;
       input_nodes[n] = input;
       debug_verbose_printf("%s: connected %s::out to %s::in_<%d> (%d of %d inputs connected)\n",__PRETTY_FUNCTION__,input->get_name(),get_name(),n,input_nodes_assigned,input_nodes_count);   
   }
}

void Node::update_input(Node *input, int n)
{
    if ((input_nodes_count == 0) || (n > input_nodes_count)){
        printf("WARNING: %s::update_input(%s, %d) argument n>%d out of bounds\n",get_name(),input->get_name(),n,input_nodes_count);
        return;
    }
    
    child_ff_vector_valid = 0;
    child_ff_vector_valid2 = 0;
    child_input_vector_valid = 0;
    
    input_nodes[n] = input;
    
    debug_verbose_printf("%s: connected %s::out to %s::in_<%d> (%d of %d inputs connected)\n",__PRETTY_FUNCTION__,input->get_name(),get_name(),n,input_nodes_assigned,input_nodes_count);
}

void Node::set_child_ff_vector_valid(int i)
{
    child_ff_vector_valid = i;
}

bool Node::set_input(Node *input, string port)
{
    //uppercase:
    transform(port.begin(), port.end(), port.begin(), ::toupper);
    
    if ((port == "SC") || (port == "SD")){
        //scanmode input -> ignore
        return true;
    }
    
    //printf("GOT %d inputs\n",port_map_vector.size());
    for(int i=0; i<port_map_vector.size(); i++){
        if (port_map_vector.at(i) == port){
            //set input
            set_input(input, i);
            //is it reset port ? then set reset ptr
            //printf("port is %s.\n",port.c_str());
            if (port == "CDN"){
                set_reset_node(input);
            }
            return true;
        }
    }
    return false;
}


Node *Node::get_input(int n)
{
    if ((input_nodes_count == 0) || (n > input_nodes_count)){
        printf("WARNING: %s::get_input(%d) argument n>%d out of bounds\n",get_name(),n,input_nodes_count);
        return NULL;
    }
    
    return input_nodes[n];
}

Node *Node::get_input(string port)
{
    //uppercase:
    transform(port.begin(), port.end(), port.begin(), ::toupper);
    
    //printf("GOT %d inputs\n",port_map_vector.size());
    for(int i=0; i<port_map_vector.size(); i++){
        //printf("GOT input %s\n",port_map_vector.at(i).c_str());
        if (port_map_vector.at(i) == port){
            //found input
            return input_nodes[i];
        }
    }
    return NULL;
}


int Node::last_eval;

//for normal nodes evaluate is the same than get_value
//but for storing elements like FFs this starts evaluating
//the input tree (seems odd, but we need this, see Node_FF for more info)
int Node::evaluate()
{
    last_eval++;
//     printf("EVAL %s(%d)\n",get_name(),last_eval);
    return get_value(last_eval);
}

int Node::get_value(int run)
{
    return value;
}

void Node::set_value(int v)
{
//     printf("set %s to %d\n",get_name(),v);
    value = v;
}


//set init value. for normal nodes this is the init value
//for LUTs for example this is the LUT table init
void Node::set_init(int v)
{
    value = v;
}

void Node::set_is_input(int s)
{
    is_input_node = s;
}

int Node::get_is_input()
{
    return is_input_node;
}

const char *Node::get_name()
{
    return name;
}

void Node::flip()
{
    value = 1 - value;
}

//check if this Node is consistent (= has valid inputs)
int Node::check_consistency()
{
    //printf("checking %s\n",get_name());
    
    //if this node is input we do not have inputs..
    if (is_input_node){
        //skip consistency checks, we might have no inputs
        return 1;
    }else if((input_nodes_count == 0) || (input_nodes == NULL)){
        printf("\rWARNING: consistency check failed for %s (%s): no input nodes!\n",get_name(),typeid(*this).name());
        return 0;
    }else if(input_nodes_assigned < input_nodes_count){
        printf("\rWARNING: consistency check failed for %s (%s): not all inputs assigned! (only %d of %d)\n",get_name(),typeid(*this).name(),input_nodes_assigned,input_nodes_count);
        return 0;
    }else{
        //ok, check all input pointers:
        for(int i=0; i<input_nodes_count; i++){
            //printf("input %d points to %x\n",i,input_nodes[i]);
            if ((input_nodes[i]) == NULL){
                printf("\rWARNING: consistency check failed for %s (%s): one input is NULL!\n",get_name(), typeid(*this).name());
                //for(int i=0; i<input_nodes_count; i++){
                //    printf("\rinput %d points to %x\n",i,input_nodes[i]);
                //}
                exit(-1);
                return 0;
            }
        }
        
        //if we get here there was no error! 
        return 1;
    }
}

int Node::get_input_node_count()
{
    return input_nodes_count;
}

//print the object graph (NOTE: builds a tree with some elements printed multiple times !!!):
void Node::print(const char *space, int rek)
{
    
    if (rek == 0){
        printf("%so %s (%d children)\n",space , name, input_nodes_count);
    }
    
    for(int i=0; i<input_nodes_count; i++){
        if (input_nodes[i] != NULL){
            char *s = new char[256];
            
            if (i != (input_nodes_count - 1)){
                sprintf(s, "%s| ",space);
            }else{
                sprintf(s, "%s  ",space);
            }
            if (input_nodes[i]->get_input_node_count() >=2){
                printf("%s|\n",space);
            }
            printf("%s+-o %s (%d children)\n",space,input_nodes[i]->get_name(),input_nodes[i]->get_input_node_count());
            input_nodes[i]->print(s, rek+1);
        }else{
            if (!is_input_node){
                if (i != (input_nodes_count - 1)){
                    printf("%s+-o NULL\n", space);
                }else{
                    printf("%s+-o NULL\n", space);
                }
            }
        }
    }
}

void Node::clear_printhistory()
{
    if (last_printed != 0){
        if (input_nodes != NULL){
            for(int i=0; i<input_nodes_count; i++){
                if (input_nodes[i] != NULL){
                    input_nodes[i]->clear_printhistory();
                }
            }
        }
        last_printed = 0;
    }
}

void Node::print_graphviz_this()
{
    if (is_input_node){
        printf("\t\"%s\\n@0x%0X\" [comment=\"%s\",shape=\"polygon\",sides=\"5\"];\n",name,this,typeid(*this).name());
    }else if(is_ff()){
        printf("\t\"%s\\n@0x%0X\" [comment=\"%s\",fillcolor=\"#AAAAFF\",style=\"filled\"];\n",name,this,typeid(*this).name());                
    }else{
        printf("\t\"%s\\n@0x%0X\" [comment=\"%s\"];\n",name,this,typeid(*this).name());
    }
}

//print the object graph as a graphviz dot output:
///BUT only up to the next FF layer
void Node::print_graphviz(int rek)
{
    //printf("// last %d, rek %d\n",last_printed,rek);
    if (rek == 0){
        last_printed = 0;
    }
    
    //only print every sub path once
    if ((last_printed != 0) && (last_printed <= rek)){
        return;
    }
    last_printed = rek;

    //print node name
    print_graphviz_this();
    
    //print  children:
    if (input_nodes != NULL){
        for(int i=0; i<input_nodes_count; i++){
            if (input_nodes[i] != NULL){
                if ((!force_return_stored_value)  && (!input_nodes[i]->is_ff())){
                    //do recursion
                    input_nodes[i]->print_graphviz(rek+1);
                    
                }else{
                    //this is the last ff stage, so print it
                    input_nodes[i]->print_graphviz_this();
                    
                    //because its useful to know what drives this FF also print main input + connection
                    Node *in = input_nodes[i]->get_ff_input_node();
                    if (in != NULL){
                        in->print_graphviz_this();
                        printf("\t\"%s\\n@0x%0X\" -> \"%s\\n@0x%0X\";\n",in->get_name(),in,input_nodes[i]->get_name(),input_nodes[i]);
                    }else{
                        printf("\t\"%s\\n@0x%0X\" -> \"%s\\n@0x%0X\";\n","NULL",0,input_nodes[i]->get_name(),input_nodes[i]);
                    }
                }
            }else{
                if (!is_input_node){
                    printf("\t\"NULL\\nto 0x%0X\" [comment=\"NULL\",shape=\"polygon\",sides=\"5\"];\n",this);
                }else{
                    printf("\t\"%s_input\" [comment=\"input\",shape=\"polygon\",sides=\"5\"];\n",name);
                }
            }
        }
    }
    
  
    //print node connections to children:
    if (input_nodes != NULL){
        for(int i=0; i<input_nodes_count; i++){
            if (input_nodes[i] != NULL){
                printf("\t\"%s\\n@0x%0X\" -> \"%s\\n@0x%0X\";\n",input_nodes[i]->get_name(),input_nodes[i],name,this);
            }else{
                if (!is_input_node){
                    printf("\t\"NULL\\nto 0x%0X\" -> \"%s\\n@0x%0X\";\n",this,name,this);
                }else{
                    printf("\t\"%s_input\" -> \"%s\\n@0x%0X\";\n",name,name,this);
                }
            }
        }
    }
}

//print the object graph as a graphviz dot output:
void Node::print_graphviz_full(int rek)
{
    //printf("// last %d, rek %d\n",last_printed,rek);
    if (rek == 0){
        last_printed = 0;
    }
    
    //only print every sub path once
    if ((last_printed != 0) && (last_printed <= rek)){
        return;
    }
    last_printed = rek;

    //print node name
    print_graphviz_this();
    
    //print  children:
    if (input_nodes != NULL){
        for(int i=0; i<input_nodes_count; i++){
            if (input_nodes[i] != NULL){
                //do recursion
                input_nodes[i]->print_graphviz_full(rek+1);
            }else{
                if (!is_input_node){
                    printf("\t\"NULL\\nto 0x%0X\" [comment=\"NULL\",shape=\"polygon\",sides=\"5\"];\n",this);
                }else{
                    printf("\t\"%s_input\" [comment=\"input\",shape=\"polygon\",sides=\"5\"];\n",name);
                }
            }
        }
    }
    
  
    //print node connections to children:
    if (input_nodes != NULL){
        for(int i=0; i<input_nodes_count; i++){
            if (input_nodes[i] != NULL){
                printf("\t\"%s\\n@0x%0X\" -> \"%s\\n@0x%0X\";\n",input_nodes[i]->get_name(),input_nodes[i],name,this);
            }else{
                if (!is_input_node){
                    printf("\t\"NULL\\nto 0x%0X\" -> \"%s\\n@0x%0X\";\n",this,name,this);
                }else{
                    printf("\t\"%s_input\" -> \"%s\\n@0x%0X\";\n",name,name,this);
                }
            }
        }
    }
}

bool Node::compare_by_child_ff_count(Node *a, Node *b)
{
    if ((a==NULL) || (b==NULL)){
        return false;
    }

    int count_a = a->get_child_ff_nodes().size();
    int count_b = b->get_child_ff_nodes().size();
    
    return (count_a < count_b);
}

bool Node::compare_by_name(Node *a, Node *b)
{
    if (a == NULL){
        printf("WARNING: %s argument 1 is NULL\n",__PRETTY_FUNCTION__);
        return false;
    }else if (b == NULL){
        printf("WARNING: %s argument 2 is NULL\n",__PRETTY_FUNCTION__);
        return true;
    }
    
    //printf("CMP(%s,%s) = %d\n",a->get_name(), b->get_name(),strcmp(a->get_name(), b->get_name()));
    if (strcmp(a->get_name(), b->get_name()) >= 0){
        return true;
    }else{
        return false;
    }
}

bool Node::compare_by_child_address(Node *a, Node *b)
{
    if (a == NULL){
        printf("WARNING: %s argument 1 is NULL\n",__PRETTY_FUNCTION__);
        return false;
    }else if (b == NULL){
        printf("WARNING: %s argument 2 is NULL\n",__PRETTY_FUNCTION__);
        return true;
    }

    return (a->get_ff_input_node() > b->get_ff_input_node());
 }

bool Node::compare_by_address(Node *a, Node *b)
{
     if (a == NULL){
         printf("WARNING: %s argument 1 is NULL\n",__PRETTY_FUNCTION__);
         return false;
     }else if (b == NULL){
         printf("WARNING: %s argument 2 is NULL\n",__PRETTY_FUNCTION__);
         return true;
     }

     return (a > b);
}


int Node::get_direct_ff_connection_count()
{
    int res = 0;
    for(int t=0; t<get_input_node_count(); t++){
        if (get_input(t)->is_ff()){
            res++;
        }
    }
    return res;
}

int Node::get_first_logic_connection_pos()
{
    for(int t=0; t<get_input_node_count(); t++){
        if (! get_input(t)->is_ff()){
            return t;
        }
    }
    return 0;
}

int Node::get_lut_init(int i) const
{
    //not used for default nodes
    return 0;
}


vector<Node *> Node::get_child_ff_nodes()
{
    if (child_ff_vector_valid == 0){
         //not yet calculated, do so:
         child_ff_vector = _get_child_ff_nodes(this, 0);
         
         if (child_ff_vector.size() > 1){
            //cleanup duplicate elements
            vector<Node *>::iterator start_pos(child_ff_vector.begin());
            vector<Node *>::iterator end_pos  (child_ff_vector.end());
    
            Node *current = *start_pos;
            
            if (current == NULL){
                printf("NULL ?!\n");
                exit(0);
            }
            
            while (++start_pos < end_pos){
                end_pos = remove(start_pos, end_pos, current);
                current = *start_pos;
            }
    
            child_ff_vector.erase( end_pos, child_ff_vector.end() );
        }
        child_ff_vector_valid = 1;
     }
     
     return child_ff_vector;
 }

 vector<Node *> Node::get_unsafe_child_ff_nodes()
 {
     if (child_ff_vector_valid2 == 0){
         //not yet calculated, do so: 
         child_ff_vector2 = _get_child_ff_nodes(this, 1);

         if (child_ff_vector2.size() != 0){
            //cleanup duplicate elements
             vector<Node *>::iterator start_pos(child_ff_vector2.begin());
             vector<Node *>::iterator end_pos  (child_ff_vector2.end());
    
             Node *current = *start_pos;
             while (++start_pos < end_pos){
                 end_pos = remove(start_pos, end_pos, current);
                 current = *start_pos;
             }
    
             child_ff_vector2.erase( end_pos, child_ff_vector2.end() );
         }
         child_ff_vector_valid2 = 1;
     }
     
     return child_ff_vector2;
 }
 
 
 
//ascend ff tree until we reach a FF node
vector<Node *> Node::_get_child_ff_nodes(Node *node, int unsafe_only)
{
     vector<Node*> result;
    
    //if node is invalid, return empty list
     if (node == NULL){
         return result;
     }
    
     for(int i=0; i<node->get_input_node_count(); i++){
         Node *child = node->get_input(i);
         if (child == NULL){
             printf("WARNING: %s: child %d of %s is NULL.\n",__PRETTY_FUNCTION__,i,node->get_name());
         }else{
             if (child->is_ff()){
                //ff/input found: add to vector
                result.push_back(child);
                //printf("PUSH CHILD %s\n",child->get_name());
             }else{
                //no ff, so descend deeper (recursive)
                 if ((unsafe_only == 0) || (!child->get_is_safe())){
                    vector<Node*> rek_result = _get_child_ff_nodes(child, unsafe_only);
                    for(int j=0; j<rek_result.size(); j++){
                        if (rek_result.at(j) != NULL){
                            result.push_back(rek_result.at(j));
                        }else{
                            printf("WARNING: %s: child from rekursive call for node %s is NULL.\n",__PRETTY_FUNCTION__,node->get_name());
                        }
                    }
                 }else{
                    ///printf("SAFE> %s\n",child->get_name());
                 }
             }
         }
     }
    
     return result;
}


vector<Node *> Node::get_child_input_nodes()
{
    if (child_input_vector_valid == 0){
         //not yet calculated, do so: 
        child_input_vector = _get_child_input_nodes(this, 0);

        if (child_input_vector.size() != 0){
            //cleanup duplicate elements
            vector<Node *>::iterator start_pos(child_input_vector.begin());
            vector<Node *>::iterator end_pos  (child_input_vector.end());
    
            Node *current = *start_pos;
            while (++start_pos < end_pos){
                end_pos = remove(start_pos, end_pos, current);
                current = *start_pos;
            }
    
            child_input_vector.erase( end_pos, child_input_vector.end() );
        }
        child_input_vector_valid = 1;
    }
     
    return child_input_vector;
}

 
//ascend ff tree until we reach a input node
//BUT do not overstep FF boundaries (ie stop when reaching a FF)
vector<Node *> Node::_get_child_input_nodes(Node *node, int unsafe_only)
{
    vector<Node*> result;
    
    //if node is invalid, return empty list
    if (node == NULL){
        return result;
    }
    
    for(int i=0; i<node->get_input_node_count(); i++){
        Node *child = node->get_input(i);
        if (child == NULL){
            printf("WARNING: %s: child %d of %s is NULL.\n",__PRETTY_FUNCTION__,i,node->get_name());
        }else{
            if (child->is_ff()){
                //do not follow over FF borders
            }else if (child->is_input()){
                //ff/input found: add to vector
                result.push_back(child);
                //printf("PUSH CHILD %s\n",child->get_name());
            }else{
                //no ff, so descend deeper (recursive)
                if ((unsafe_only == 0) || (!child->get_is_safe())){
                    vector<Node*> rek_result = _get_child_input_nodes(child, unsafe_only);
                    for(int j=0; j<rek_result.size(); j++){
                        if (rek_result.at(j) != NULL){
                            result.push_back(rek_result.at(j));
                        }else{
                            printf("WARNING: %s: child from rekursive call for node %s is NULL.\n",__PRETTY_FUNCTION__,node->get_name());
                        }
                    }
                }else{
                    ///printf("SAFE> %s\n",child->get_name());
                }
            }
        }
    }
    
    return result;
}

//static function: return true if node* exists in vector
bool Node::node_in_vector(vector<Node *> *vector, Node * node)
{
    for(int f=0; f < vector->size(); f++){
        if (vector->at(f) == node){
            return true;
        }
    }
    return false;
}

//static function: push node only to vector if not already inside
void Node::safe_push(vector<Node *> *vector, Node * node)
{
    if (Node::node_in_vector(vector, node)){
        return;
    }
    //push only if it does not exist in the vector
    vector->push_back(node);
}

void Node::print_inputmap()
{
    printf(",NODE: '%s' (@0x%0X)\n",get_name(),this);
    for(int i=0; i<input_nodes_count; i++){
        Node *in = get_input(i);
        if (in == NULL){
            printf("+ in %3s = NULL\n",get_input_name(i).c_str());
        }else{
            printf("+ in %3s = '%s'\n",get_input_name(i).c_str(),in->get_name());
        }
    }
    printf("\n");
}

//when called descend down to next FF layer and
//set a node id to all FFs
/*void Node::recalc_node_id_rek(string s)
{
    if (is_ff() && (s != "")){
        //print/set
        ///printf("Node %30s = %s\n",get_name(),s.c_str());
        set_node_id(s);
    }else{
        for(int i=0; i < get_input_node_count(); i++){
            char num[256];
            sprintf(num, "%d", i);
            get_input(i)->recalc_node_id_rek(s + num);
            if (i>9){
                printf("WARNING: this has been designed for gates with less than 10 inputs! fixme! exitiing\n");
                exit(-1);
            }
        }
    }
}*/

std::string Node::get_formula()
{
    return "-? ? ?-";
}

void Node::set_node_id(int id)
{
    node_id = id;
}

int Node::get_node_id()
{
    return node_id;
}

