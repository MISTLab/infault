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
#include "graph.h"

using namespace std;

Graph::Graph()
{
    node_vector.clear();
    conn_vector.clear();
    node_vector_is_sorted = 1;
}


Graph::~Graph()
{
}

void Graph::remove_node(Node *node)
{
    if (node == NULL){
        printf("ERROR: request to remove a NULL node! exiting\n");
        exit(-1);
    }else{
        //check if node has children!
        vector<Node *>::iterator node_it;
        for (node_it = node_vector.begin(); node_it != node_vector.end(); node_it++){
            Node *node2 = *node_it;
            if (node2 == node){
                printf("# erased node %s from node_vector\n",node->get_name());
                node_vector.erase(node_it);
                return;
            }
        }
        
    }
}

void Graph::add_node(Node *node)
{
    if (node == NULL){
        printf("ERROR: request to add a NULL node! exiting\n");
        //exit(-1);
    }else{
        node_vector.push_back(node);
        debug_verbose_printf("# pushed node %s to node_vector at pos %d\n",node->get_name(),node_vector.size()-1);
        
        //check if node has children!
        for(int i=0; i<node->get_input_node_count(); i++){
            if (node->get_input(i) != NULL){
                //push child as well
                node_vector.push_back(node->get_input(i));
                debug_verbose_printf("# pushed node %s to node_vector\n",node->get_input(i)->get_name());
            }
        }
        
        node_vector_is_sorted = 0;
    }
}

void Graph::sort_node_vector_by_ff_child_count()
{
    printf("WARNING: Graph::sort_node_vector_by_ff_child_count() called. THIS WILL TAKE A LOOONG TIME!\n");
    //mark node vect as no longer sorted (by the means of name)
    node_vector_is_sorted = 0;
    
    //sort graph by childcount:
    stable_sort(node_vector.begin(), node_vector.end(), Node::compare_by_child_ff_count);
    
}


void Graph::sort_node_vector()
{
    if (node_vector_is_sorted == 0){
        //NOTE: strange, sort() calles the compare function with NULL pointers 
        //      even if they do not exist in the node_vcetor... STL BUG ?!
        //      workaround: use stable_sort() instead
        stable_sort(node_vector.begin(), node_vector.end(), Node::compare_by_name);
        node_vector_is_sorted = 1;
    }
}

int Graph::get_node_vector_ff_count()
{
    int total_ffs = 0;
    for(int i=0; i<get_node_count(); i++){
        Node *node = get_node_by_id(i);
        if (node->is_ff() || node->is_output()){
            total_ffs++;
        }
    }
    return total_ffs;
}

Node *Graph::binary_search_node_vector(string name)
{
    // Keep halfing the search space until we reach the end of the vector
    vector<Node *>::iterator Middle;
    vector<Node *>::iterator begin = node_vector.begin();
    vector<Node *>::iterator end   = node_vector.end();
    
    //printf("\n\nlooking for '%s':\n",name.c_str());
    while(begin < end){
        // Find the median value between the iterators
        Middle = begin + (std::distance(begin, end) / 2);

        // Re-adjust the iterators based on the median value
        Node *node = *Middle;
        
        if(node == NULL){
            printf("WARNING: %d NULL node in vector ?!\n");
        }else{
            //printf("\tchecking '%s' -> %d\n",node->get_name(),strcmp(node->get_name(), name.c_str()));
            if(strcmp(node->get_name(), name.c_str()) == 0){
                return node;
            }else if(strcmp(node->get_name(), name.c_str()) < 0){
                end = Middle;
            }else{
                begin = Middle + 1;
            }
        }
    }

    return NULL;
}


Node *Graph::get_node_by_id(int id)
{
    if (id < node_vector.size()){
        return node_vector.at(id);
    }
    return NULL;
}

int Graph::get_node_count()
{
    return node_vector.size();
}
    
Node *Graph::get_node_by_name_quiet(string name)
{
    return get_node_by_name(name, 1);
}
        
Node *Graph::get_node_by_name(string name, int quiet)
{
    //printf("get node by name %s\n",name.c_str());
    
    //sort node vector (is intelligent, only sorts if changed)
    sort_node_vector();
    
    Node *res = binary_search_node_vector(name);
    if (res == NULL){
        if (!quiet) printf("ERROR: Node '%s' not found. returning NULL !\n",name.c_str());
        //res = new Node_VCC(name.c_str());
    }
    
    return res;
/*    
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        if (node == NULL){
            printf("WARN: Node at location %d in node_vector is NULL!\n",n);
        }else if (strcmp(node->get_name(), name.c_str()) == 0){
            return node;
        }else{
            //printf("chk '%s'\n",node->get_name());
        }
    }
    
    
    
    
    printf("ERROR: Node '%s' not found. returning NULL !\n",name.c_str());
    //exit(EXIT_FAILURE);
    return NULL;*/
}

//temporary store a connection request like "AND0, I0"
void Graph::prepare_connection(wire wire) //string instance, string port)
{
    if (get_node_by_name(wire.instance) != NULL){
        //printf("PUSHED %s::%s\n",instance.c_str(),port.c_str());
        //conn_vector.push_back(instance);
        //conn_vector.push_back(port);
        
        //make portname uppercase
        transform(wire.port.begin(), wire.port.end(), wire.port.begin(), ::toupper);
        
        //printf("PUSHED %s::%s\n",wire.instance.c_str(),wire.port.c_str());
        if (wire.port != "C"){
            conn_vector.push_back(wire);
        }else{
            debug_verbose_printf("WARN: ignoring C as port\n");
        }
    }else{
        printf("WARN: cannot push wire req %s::%s, node %s not found\n",wire.instance.c_str(),wire.port.c_str(),wire.instance.c_str());
        exit(-1);
    }
}

//execute the connection requests stored before. FIXME: find a more intelligent way later
void Graph::execute_connections()
{
    if (conn_vector.size() == 0){
        return;
    }
    
    printf("x");
    
    //in order to execute this request we need to find the output
    //in the connection requests:
    string output_node_name = "";
    for(int i=0;i < conn_vector.size(); i++){
        wire wire = conn_vector.at(i); 
        //string instance = conn_vector.at(i+0);
        //string port     = conn_vector.at(i+1);
        if ((wire.port == "Q") || (wire.port == "O") || (wire.port == "G") || (wire.port == "LO")|| (wire.port == "P") || (wire.port == "Z") || (wire.port == "ZN")){
            output_node_name = wire.instance;
        }
    }
    
    if (output_node_name == ""){
        printf("WARNING: output node not found in list of wires:\n");
        for(int i=0;i < conn_vector.size(); i++){
            wire target_wire = conn_vector.at(i);
            printf("\t%s::%s\n",target_wire.instance.c_str(),target_wire.port.c_str());
        }
        exit(-1);
    }
    
    
    for(int i=0;i < conn_vector.size(); i++){
        wire target_wire = conn_vector.at(i);
        debug_printf("\rneed to exec conn %s::%s\n",target_wire.instance.c_str(),target_wire.port.c_str());
    }
    
    //ok now we connect all other requests to this output:
    for(int i=0;i < conn_vector.size(); i++){
        //string target_node_name = conn_vector.at(i+0);
        //string target_node_port = conn_vector.at(i+1);
        wire target_wire = conn_vector.at(i);
        
        if (target_wire.instance != output_node_name){
            debug_printf("need to connect %s::out to %s::%s\n",output_node_name.c_str(),target_wire.instance.c_str(), target_wire.port.c_str());
            
            //if ((target_wire.port == "C")|| (target_wire.port == "PRE")|| (target_wire.port == "CLK")|| (target_wire.port == "CLKA") || (target_wire.port == "CLKB")|| (target_wire.port == "CDN")|| (target_wire.port == "CP")|| (target_wire.port == "SC")|| (target_wire.port == "SD")){
            if ((target_wire.port == "C")|| (target_wire.port == "PRE") || (target_wire.port == "CLK")|| (target_wire.port == "CLKA") || (target_wire.port == "CLKB") || (target_wire.port == "CP")){
                //ignore those requests for now, do it here to speed up
            }else{
                Node *source = get_node_by_name(output_node_name);
                Node *target = get_node_by_name(target_wire.instance);
                
                if ((source == NULL) || (target == NULL)){
                    printf("WARNING: source or target is NULL. not making the connection.\n");
                }else{
                    //connect
                    bool connected = false;
                    
                    if (target_wire.port ==  "CLR"){
                        target->set_reset_node(source);
                        connected = true;
                    }else if (target->set_input(source, target_wire.port)){
                        connected = true;
                    }else{
                        //hmm not found. maybe this is a special node
                        target = get_node_by_name(target_wire.instance + "__" + target_wire.port);
                        if (target != NULL){
                            //ok found, try to connect
                            if (target->set_input(source, "D")){
                                connected = true;
                            }
                        }
                    }
                    
                    if (!connected){
                        printf("WARNING: do not understand port '%s' (instance %s)\n",target_wire.port.c_str(),target_wire.instance.c_str());
                        if (get_node_by_name(target_wire.instance) != NULL){
                            printf("          -> class is  %s\n",typeid(*get_node_by_name(target_wire.instance)).name());
                        }
                        exit(0);
                    }
                }
            }
        }
    }
    
    //done. remove all objects from conn list request
    conn_vector.clear();
}


Node *Graph::create_ram_node_UNISIM(string instance, string celltype, int initvalue, int addra_len, int addrb_len, int dia_len, int dib_len, int dipa_len, int dipb_len, int doa_len, int dopa_len, int dob_len, int dopb_len)
{
    Node *result;
    char buf[255];
    int input = 0;

    //ok, we have to build a RAM. we are only interested into all
    //in and outputs, not in the function itself. we treat all
    //i/os as FFs -> search only for SEU problems up to there!
    //
    //because we can return only one net here, we simply create 
    //a the main ram node which has all we,ssr,en control signals and 
    //all other io FFs as children.
    
    Node *tmp;
    
    //process the addr inputs
    if (addrb_len == 0){
        //input count is +3 because of the WE, SSR, EN control signals
        result = new Node(instance.c_str(), addra_len+addrb_len+dia_len+dib_len+dipa_len+dipb_len+doa_len+dopa_len+dob_len+dopb_len+3);
        
        //singleport
        for(int i=0; i<addra_len; i++){
            sprintf(buf, "%s__ADDR__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dia_len; i++){
            sprintf(buf, "%s__DI__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dipa_len; i++){
            sprintf(buf, "%s__DIP__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<doa_len; i++){
            sprintf(buf, "%s__DO__%d",instance.c_str(),i);
            tmp = new Node_INPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dopa_len; i++){
            sprintf(buf, "%s__DOP__%d",instance.c_str(),i);
            tmp = new Node_INPUT(buf);
            result->set_input(tmp, input++);
        }
        
        sprintf(buf, "%s__WE",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
        sprintf(buf, "%s__SSR",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
        sprintf(buf, "%s__EN",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
    }else{
        //input count is +6 because of the WEx, SSRx, ENx control signals
        result = new Node(instance.c_str(), addra_len+addrb_len+dia_len+dib_len+dipa_len+dipb_len+doa_len+dopa_len+dob_len+dopb_len+2*3);
        
        //dualport
        for(int i=0; i<addra_len; i++){
            sprintf(buf, "%s__ADDRA__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<addrb_len; i++){
            sprintf(buf, "%s__ADDRB__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dia_len; i++){
            sprintf(buf, "%s__DIA__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dib_len; i++){
            sprintf(buf, "%s__DIB__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dipa_len; i++){
            sprintf(buf, "%s__DIPA__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dipb_len; i++){
            sprintf(buf, "%s__DIPB__%d",instance.c_str(),i);
            tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<doa_len; i++){
            sprintf(buf, "%s__DOA__%d",instance.c_str(),i);
            tmp = new Node_INPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dob_len; i++){
            sprintf(buf, "%s__DOB__%d",instance.c_str(),i);
            tmp = new Node_INPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dopa_len; i++){
            sprintf(buf, "%s__DOPA__%d",instance.c_str(),i);
            tmp = new Node_INPUT(buf);
            result->set_input(tmp, input++);
        }
        for(int i=0; i<dopb_len; i++){
            sprintf(buf, "%s__DOPB__%d",instance.c_str(),i);
            tmp = new Node_INPUT(buf);
            result->set_input(tmp, input++);
        }
        
        sprintf(buf, "%s__WEA",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
        sprintf(buf, "%s__SSRA",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
        sprintf(buf, "%s__ENA",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
        sprintf(buf, "%s__WEB",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
        sprintf(buf, "%s__SSRB",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
        sprintf(buf, "%s__ENB",instance.c_str());
        tmp = new Node_OUTPUT(buf);
        result->set_input(tmp, input++);
        
    }
    
    return result;
}


Node *Graph::create_node_VERILOG(string instance, string celltype, int initvalue)
{
    Node *result = NULL;
    char buf[255];
    int input = 0;
    
    printf("c");
    
    string cellclass = "ATC18_" + celltype;
    
    //call node factory
    result = Node_factory<Node>::create_instance(cellclass, instance.c_str());
    if (result == NULL){
        printf("NULL\n");
        //result = new Node(instance.c_str(), 0);
        printf("cant create VERILOG::%s node\n",cellclass.c_str());
        exit(0);
    }else{
        ///printf("Node is %s (%s)\n",typeid(*result).name(),result->get_name());
    }
    return result;
}

Node *Graph::create_node_UNISIMS(string instance, string celltype, int initvalue)
{
    Node *result = NULL;
    char buf[255];
    int input = 0;
    
    printf("c");
    
    if (celltype == "FD"){
        //normal FF
        result = new Node_UNISIM_FF(instance.c_str());
        result->set_init(initvalue);
        
    }else if (celltype == "FDC"){
        //normal FF
        result = new Node_UNISIM_FF(instance.c_str());
        result->set_init(initvalue);
        
    }else if (celltype == "FDP"){
        //normal FF
        result = new Node_UNISIM_FF(instance.c_str());
        result->set_init(initvalue);
     
    }else if (celltype == "INV"){
        //inverter
        result = new Node_UNISIM_NOT(instance.c_str());
    
    }else if((celltype == "LUT4") || (celltype == "LUT4_D") || (celltype == "LUT4_L")){
        //LUT4: use LUT4 
        result = new Node_UNISIM_LUT4(instance.c_str());
        result->set_init(initvalue);
    }else if((celltype == "LUT3") || (celltype == "LUT3_D") || (celltype == "LUT3_L")){
        //LUT3: use LUT4 with I3 connected to GND
        result = new Node_UNISIM_LUT4(instance.c_str());
        result->set_init(initvalue);
        Node *gnd = new Node_UNISIM_GND("GND");
        result->set_input(gnd, 3);
    }else if((celltype == "LUT2") || (celltype == "LUT2_D") || (celltype == "LUT2_L")){
        //LUT2: use LUT4 with I3,I2 connected to GND
        result = new Node_UNISIM_LUT4(instance.c_str());
        result->set_init(initvalue);
        Node *gnd = new Node_UNISIM_GND("GND");
        result->set_input(gnd, 2);
        result->set_input(gnd, 3);
        
    }else if((celltype == "LUT1") || (celltype == "LUT1_D") || (celltype == "LUT1_L")){
        //LUT1: use LUT4 with I3,I2,I1 connected to GND
        result = new Node_UNISIM_LUT4(instance.c_str());
        result->set_init(initvalue);
        Node *gnd = new Node_UNISIM_GND("GND");
        result->set_input(gnd, 1);
        result->set_input(gnd, 2);
        result->set_input(gnd, 3);

    }else if((celltype == "MUXF5") || (celltype == "MUXF6") || (celltype == "MUXF7") || (celltype == "MUXF8") ){
        //MUX*:
        result = new Node_UNISIM_MUX2(instance.c_str());
    }else if((celltype == "MUXCY")){
        //MUX*:
        result = new Node_UNISIM_MUX2CY(instance.c_str());
    }else if (celltype == "XORCY"){
        //normal FF
        result = new Node_UNISIM_XOR2(instance.c_str());

    }else if (celltype == "GND"){
        //GND
        result = new Node_UNISIM_GND("GND");

    }else if (celltype == "VCC"){
        //VCC
        result = new Node_UNISIM_VCC("VCC");
    
    
    }else if (celltype == "MULT18X18"){
        result = new Node("MULT18X18__DUMMY__",18+18+36);
        
        for(int i=0; i<=17; i++){
            sprintf(buf, "%s__A__%d",instance.c_str(),i);
            Node *tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        
        for(int i=0; i<=17; i++){
            sprintf(buf, "%s__B__%d",instance.c_str(),i);
            Node *tmp = new Node_OUTPUT(buf);
            result->set_input(tmp, input++);
        }
        
        for(int i=0; i<=35; i++){
            sprintf(buf, "%s__P__%d",instance.c_str(),i);
            Node *tmp = new Node_INPUT(buf);
            result->set_input(tmp, input++);
        }
        
    }else if (celltype == "RAMB16_S36_S36"){
        //printf("******** ram %s\n",instance.c_str());
        result = create_ram_node_UNISIM(instance, celltype, initvalue, 9, 9, 32, 32, 4, 4, 32, 4, 32, 4);
    }else if (celltype == "RAMB16_S36"){
        result = create_ram_node_UNISIM(instance, celltype, initvalue, 9, 0, 32, 0, 4, 0, 32, 4, 0, 0);
    }else if (celltype == "RAMB16_S9"){
        result = create_ram_node_UNISIM(instance, celltype, initvalue, 11, 0, 8, 0, 1, 0, 8, 1, 0, 0);
        
    }else{
        printf("ERROR: celltype 'UNISIMS::%s' is currently not supported. resulting object will be NULL\n",celltype.c_str());
        exit(-1);
    }
    
    return result;
}

Node *Graph::create_node(string library,  string instance, string celltype, int initvalue)
{
    //celtype to uppercase
    transform(celltype.begin(), celltype.end(), celltype.begin(), ::toupper);
    
    debug_verbose_printf("creating object <%s>\tusing lib %s::%s\t/ initval=0x%0X\n",instance.c_str(),library.c_str(),celltype.c_str(),initvalue);
    
    
    
    if (library == "UNISIMS"){
        //handle unisim library
        return create_node_UNISIMS(instance,celltype,initvalue);
    }else if (library == "VERILOG"){
        //handle unisim library
        return create_node_VERILOG(instance,celltype,initvalue);
    }else{
        printf("ERROR: libary '%s' is currently not supported. resulting object will be NULL.\n",library.c_str());
        return NULL;
    }
    
    //never reached!
    return NULL;
}

Node *Graph::get_buffer_source(Node *buf)
{
    if (!buf->is_buffer()){
        //end of rek
        return buf;
    }else{
        //this is a buffer, find source:
        Node *in = buf->get_input(0);
        
        if (in == buf){
            printf("\rWARNING: buffer '%s' input is buffer itself! (LOOP!) ?!?\n", buf->get_name());
            return in;
        }
        
        return get_buffer_source(in);
    }
}
        
//remove all buffers inside the graph
int Graph::remove_buffers()
{
    int count =0;
    printf("> removing buffers...\n");
    
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        //check for all inputs if they are connected to a buffer
        for(int j=0; j<node->get_input_node_count(); j++){
            if (node->get_input(j) != NULL){
                if (node->get_input(j)->is_buffer()){
                    Node *newsource = get_buffer_source(node->get_input(j));
                    
                    if (newsource == NULL){
                        printf("ERROR: remove_buffers() encountered NULL input! exiting\n");
                        exit(-1);
                    }
                    
                    debug_verbose_printf("> replacing buffer input to %s:%d to be driven by %s directly\n",node->get_name(),j,newsource->get_name());
                    cout.flush();
                    
                    node->update_input(newsource, j);
                    count++;
                }
            }
        }
    }
    printf("> done. removed %d buffers\n", count);
    
    return count;
}

//optimise away all unused scanmode chain muxes
int Graph::optimize_muxes()
{
    int count =0;
    int count_not =0;
    printf("> removing scanmode muxes...\n");
    
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        //check for all inputs if they are connected to a buffer
        for(int j=0; j<node->get_input_node_count(); j++){
            if (node->get_input(j) != NULL){
                if (node->get_input(j)->is_mux() == 1){
                    //ok this is a mux, we have 
                    //select input
                    //input 0
                    //input 1
                    //now check the select input if fixed to a constant:
                    //__CONST_0__ or __CONST_1__ 
                    
                    //printf("node is mux\n"); cout.flush();
                    //printf("node name %s\n", node->get_input(j)->get_name()); cout.flush();
                
                    
                    Node *selectnode = node->get_input(j)->get_input("S");
                    
                    if (selectnode == NULL){
                        printf("ERROR: optimize_muxes() encountered NULL \"S\" input for node %s! exiting\n", node->get_input(j)->get_name());
                        exit(-1);
                    }
                    
                    //check if this is a constant:
                    if (selectnode->is_constant()){
                        //fixed select -> optimize this mux away
                        Node *selectedinputnode = NULL;
                        if (selectnode->get_value(count) == 0){
                            selectedinputnode = node->get_input(j)->get_input("I0");//input 0 selected
                        }else{
                            selectedinputnode = node->get_input(j)->get_input("I1");//input 1 selected
                        }
                        
                        if (selectedinputnode == NULL){
                            printf("ERROR: optimize_muxes() encountered NULL input connected to mux ?! exiting\n");
                            exit(-1);
                        }
                        
                        debug_verbose_printf("> optimizing mux %s, select fixed to input %s\n",node->get_name(),selectedinputnode->get_name());
                        
                        node->update_input(selectedinputnode,j);
                        
                        count++;
                    }else{
                        //not constant select, so do nothing
                        debug_verbose_printf("> cant optimize mux %s, no constant select input\n",node->get_input(j)->get_name());
                        cout.flush();
                        count_not++;
                    }
                }
            }
        }
    }
    printf("> done. optimized %d muxes (%d not optimized)\n", count, count_not);
//     exit(0);
    return count;
}


int Graph::check_for_functional_identical(Node *a, Node *b)
{
//     printf("check...identical(%s, %s) with types %s %s\n",a->get_name(),b->get_name(),typeid(*a).name(),typeid(*b).name());
    
    //1) check if same gate type
    if (typeid(*a) != typeid(*b)){
        //no, different type, cant be identical
        //printf("DIFF TYPEID\n");
        return 0;
    }
    
    if (a->is_constant() && b->is_constant()){
        if (a->get_value(0) == b->get_value(0)){
//             printf("EQUAL CONST\n");
            return 1;
        }else{
            //printf("DIFF CONST VAL\n");
            return 0;
        }
    }
    
    //2) because of 1) we know they have the same inputcount:
    for(int i=0; i<a->get_input_node_count(); i++){
        if (a->get_input(i) != b->get_input(i)){
            //pointing to different gate -> not identical
            //printf("DIFF PTR DEST\n");
            return 0;
        }
    }
    return 1;
}

//optimise away all duplicate gates
int Graph::optimize_duplicates_to_ffs()
{
    int count =0;
    int count_not =0;
    printf("> removing duplicate gates connected to FFs...\n");
    
    vector <Node *> ff_vector;
    
    //find all FFs
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        if (node->is_ff()){
            //this is a FF, so push to test vector
            ff_vector.push_back(node);
        }
    }
    
    //now check for all those FFs if there are
    //gates connected to the FF that are
    //a different object, but share exactly the same inputs
    for(int n=0;n < ff_vector.size(); n++){
        if ((n & 0x0000001F) == 0){
            printf("\r> processing... %5.2f %% done        ",n*100.0/ff_vector.size());
            cout.flush();
        }
        
        for(int m=0; m < ff_vector.size(); m++){
            Node *node_a = ff_vector.at(n);
            Node *node_b = ff_vector.at(m);
            
            for(int i=0; i<node_a->get_input_node_count(); i++){
                for(int j=0; j<node_b->get_input_node_count(); j++){
                    //check the drivers
                    Node *driver_a = node_a->get_input(i);
                    Node *driver_b = node_b->get_input(j);
                    
                    //are they NOT identical ? (=pointing to the same address ?)
                    
                    if ((driver_a != driver_b) && (!driver_a->get_to_be_removed())){
                        //printf("driver %d != %d\n",driver_a,driver_b);
                        //not pointing to the same address...
                        //now check if they are functional identical:
                        if (check_for_functional_identical(driver_a, driver_b)){
                            //yes, they have the same function!
                            //-> assign driver_a as driver for node_b too
                            if (!driver_a->is_constant()) 
//                                 printf("\r> Node %d:%d and %d:%d share functional identical input (%s) -> replacing\n",n,i,m,j,driver_a->get_name());
                            
                            //replace (=relinking)
                            node_b->update_input(driver_a,j);
                            
                            //mark for removal
                            driver_b->set_to_be_removed(1);
                            
                            count++;
                        }
                    }
                }
            }
        }
    }
    
    printf("\r> done. optimized %d gates\n", count);
//     exit(0);
    return count;
}

//optimise away all not not chains
int Graph::optimize_double_inverters()
{
    int count =0;
    int count_not =0;
    printf("> removing duplicate inverters...\n");
    
    vector <Node *> inv_vector;
    
    //find all nodes that have an inverter input
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        for(int m=0;m < node->get_input_node_count(); m++){
            Node *in = node->get_input(m);
            if (in->is_inverter()){
                //this is a inverter, so push to test vector
                inv_vector.push_back(node);
            }
        }
    }
    
    //now check for all those invs if they are followed by another inverter:
    for(int n=0;n < inv_vector.size(); n++){
        if ((n & 0x0000001F) == 0){
            printf("\r> processing... %5.2f %% done        ",n*100.0/inv_vector.size());
            cout.flush();
        }
        Node *node_driven = inv_vector.at(n);
        
        for(int m=0;m < node_driven->get_input_node_count(); m++){
            Node *node_a = node_driven->get_input(m);
            if (node_a->is_inverter()){
                if (node_a->get_input_node_count() > 0){
                    Node *node_b = node_a->get_input(0);
                
                    if (node_b->is_inverter()){
                        // N0 ---> [NOT_b ----> NOT_a] ----> N1
                        //-> replace chain --> [...] -->  with --> [BUF] -->
                        string name = node_b->get_name() + string("__BUF1__opt_buf__"); 
                        Node *newb = new Node_BUF1(name.c_str());
                        newb->set_input(node_b->get_input(0),0);
                        //now change node_driven that its driven by this new buf
                        node_driven->set_input(newb,m);
                        count++;
                    }
                }
            }
        }
    }
    
    printf("\r> done. optimized %d gates (of %d)   \n", count,inv_vector.size());
//     exit(0);
    return count;
}



//check if a given node is a child of any other node:
int Graph::is_child_node(Node *child)
{
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        for(int j=0; j<node->get_input_node_count(); j++){
            if (node->get_input(j) != NULL){
                if (node->get_input(j) == child){
                    return 1;
                }
            }
        }
    }
    //not a child of any node
    return 0;
}

void Graph::print_graphviz()
{
    printf("// %d nodes in total\ndigraph G\n{\n",node_vector.size());
    
    node_printed_vector.clear();
    
    //print all nodes starting only from "super" parent nodes
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        if ((node != NULL) && !is_child_node(node)){
            printf("\t// printing node '%s' @0x%0X\n",node->get_name(),node);
            cout.flush();
            node->clear_printhistory();
            node->print_graphviz_full(); //print_graphviz_recursive(node);
        }
    }
    
    printf("}\n\n");
}

int Graph::check_consistency()
{
    int valid = 1;
    
    printf("> checking graph constistency...\n");
    printf("> need to check %d nodes.",node_vector.size());
    
    //check all nodes starting only from "super" parent nodes
    for(int n=0;n < node_vector.size(); n++){
        Node *node = node_vector.at(n);
        if (node == NULL){
            printf("\rWARNING: Node is NULL\n");
        }else{
            if ((n&0xFF) == 0){
                printf("\r                                                                                                                           \r> checking %d nodes %3.0f%% done : checking %s",node_vector.size(),n*100.0/node_vector.size(),node->get_name());
                cout.flush();
            }
        
            //normally we would check only childnodes...
            //if (!is_child_node(node)){
            //BUT: its faster to simply check all nodes (runtime of is_child_node is huge)
            if (!node->check_consistency()){
                printf("\rWARNING: Node '%s' (@0x%0X) has invalid NULL inputs\n",node->get_name(),node);
                valid = 0;
            }
            
        }
    }
    
    printf("\n> done.\n");
    
    return valid;
}



