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
#include "radiator.h"
#include "node_include.h"
#include <math.h>


#define SHOW_BITVECTORS 0
//#define SPLITSIZE (3*18)
#define SPLITSIZE (3*5)
#define RADIATOR_QUIET 1
#define RADIATOR_QUIET2 1
#define RADIATOR_TRIPLET_ID_QUIET 1
#define RADIATOR_NEW_OPT_QUIET 1
#define RADIATOR_INJECT_TARGET_QUIET 1

using namespace std;


Radiator::Radiator()
{
    ffs_done = 0;
    seu_sensitive_ff_count = 0;
    
    
    stat_seu_sensitive_ffs=0;
    stat_splitcounter=0;
    for(int i=0; i<4096;i++){
        stat_subgraph_size_after_split[i]=0;
        stat_subgraph_size_before_split[i]=0;;
    }
}


Radiator::~Radiator()
{
}

void Radiator::log_sensitive_ff(Node *n)
{
    int found = 0;
    for(int i=0; i<stat_sensitive_ffs.size(); i++){
        if (stat_sensitive_ffs.at(i) == n){
            found = 1;
            i=stat_sensitive_ffs.size()+1; //abort
        }
    }
    if (!found){
        stat_seu_sensitive_ffs++;
        stat_sensitive_ffs.push_back(n);
    }
}

int Radiator::do_seu_search(Graph *g, Node *node)
{
    if (node == NULL){
        printf("ERROR: %s called with NULL node. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
    if (g == NULL){
        printf("ERROR: %s called with NULL graph. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
    int total_ffs = g->get_node_vector_ff_count();
    
    //log size before split
    if (node->get_child_ff_nodes().size()/3>4096){
        printf("ERROR: cannot log, only space for 4096 entries. FIX: change size in radiator.h\n\n");
        exit(-1);
    }
    stat_subgraph_size_before_split[node->get_child_ff_nodes().size()/3]++;
     
    //node->print_graphviz(0);
    //if necessary split the graph connected to node into subgraphs:
    vector <Node*> subgraph_in = recursive_split(node);
    vector <Node*> subgraph;
    
    //remove any duplicates & nodes already processed
    for(int j=0; j<subgraph_in.size(); j++){
        int already_there = 0;
        Node *a = subgraph_in.at(j);
        if (!a->get_seu_check_done()){
            for(int k=0; k<subgraph.size(); k++){
                Node *b = subgraph.at(k);
                if (a==b){
                    already_there = 1;
                }
            }
            if ((!already_there) && (!a->get_seu_check_done())){
                subgraph.push_back(a);
            }
        }
    }
    printf("> removed duplicates & already checked nodes -> %d gates removed -> %d to check\n", (subgraph_in.size()-subgraph.size()),subgraph.size());
    
    if (node->get_child_ff_nodes().size() > 3000){
        for(int j=0; j<node->get_child_ff_nodes().size(); j++){
            Node *a = node->get_child_ff_nodes().at(j);
            printf("%4d, %s\n",j,a->get_name());
        }
        exit(0);
    }
    
    if (subgraph.size() > 1){
        //we splitted this node into some parts, print info:
        printf("\r> [%5d/%5d FFs done = %3d%% ] SPLIT   '%s'<%d> into %d subgraphs                                  \n",ffs_done,total_ffs,ffs_done*100/total_ffs,node->get_name(),node->get_child_ff_nodes().size(),subgraph.size());
        
        //mark all ffs that have the same input as node as done:
        for(int j=0; j<g->get_node_count(); j++){
            Node *dup = g->get_node_by_id(j);
            if ((dup != NULL) && (dup->is_ff()) && (node->is_ff())){
                if (dup->get_ff_input_node() == node->get_ff_input_node()){
                    //we do not have to check this FF again
                    if (!dup->get_seu_check_done()){
                        dup->set_seu_check_done(1);
                        ffs_done++;
                    }
                }
            }
        }
        
        //count & mark node also if output
        if ((!node->get_seu_check_done()) && (node->is_output())){ 
            node->set_seu_check_done(1);
            ffs_done++;
        }
        
    }

    for (int s=0; s<subgraph.size(); s++){
        Node *nut = subgraph.at(s);
        
        //printf("NUT %d/%d -> %s\n",s,subgraph.size(),nut->get_name());
        if (!nut->get_seu_check_done()){
            printf("\r> [%5d/%5d FFs done = %3d%% ] testing '%s'<%d> (@0x%X) %s %d/%d                                  \n",ffs_done,total_ffs,ffs_done*100/total_ffs,nut->get_name(),nut->get_child_ff_nodes().size(),nut,(nut->is_ff()?"(FF)":(nut->is_output()?"(OUTPUT)":"(LOGIC)")),s,subgraph.size());
            do_seu_test_for_node(g, nut);
            
            //hmm maybe still too small ?! split more!
            /*if (nut->get_child_ff_nodes().size() > SPLITSIZE){
                ffs_done = do_seu_search(g, nut, ffs_done);
            }else{
                ffs_done += do_seu_test_for_node(g, nut);
            }*/
        }
    }
    
    node->set_seu_check_done(1);
    if ((!node->get_seu_check_done()) && (node->is_ff() || node->is_output())){ ffs_done++; }
    
    //remove node copies 
//     for(int j=0; j<subgraph_in.size(); j++){
//         Node *a = subgraph_in.at(j);
//         if (a->get_is_deleteable_copy()){
//             printf("d");
//             delete a;
//         }
//     }
    
    //garbage collection
    printf("\r> garbage collection: %d nodes to delete\n",garbage_vector.size());
    vector <Node *> unique_garbage_vector;
    
    for(int j=0; j<garbage_vector.size(); j++){
        int found = 0;
        Node *a = garbage_vector.at(j);
        for(int k=0; (k<unique_garbage_vector.size() && (!found)); k++){
            Node *b = unique_garbage_vector.at(k);
            if (a==b){
                found = 1;
            }
        }
        if (!found){
            unique_garbage_vector.push_back(a);
        }
    }
    for(int j=0; j<unique_garbage_vector.size(); j++){
        Node *a = unique_garbage_vector.at(j);
//         for(int k=0; k<g->get_node_count(); k++){
//             Node *b = g->get_node_by_id(k);
//             if (a==b){
//                 printf("STRANGE GARBAGE NODE IN GRAPH ?! %s @0x%0X\n",a->get_name(),a);
//                 exit(0);
//             }
//         }
        if (a != NULL){
//                 printf("deleting %s @0x%X\n",a->get_name(),a);
                delete a;
        }
    }
    unique_garbage_vector.clear();
    garbage_vector.clear();
    return ffs_done;
}

void Radiator::do_seu_search(Graph *g)
{
    if (g == NULL){
        printf("ERROR: %s called with NULL graph. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
    printf("> running full SEU test...\n");
    
    int total_ffs = g->get_node_vector_ff_count();
    
    for(int i=0; i<g->get_node_count(); i++){
        Node *node = g->get_node_by_id(i);
        if ((node != NULL) && ((node->is_ff())||(node->is_output()))){
            if (node->get_seu_check_done() == 1){
                //test already done
                printf("\r> [%5d/%5d FFs done = %3d%% ] SKIP    '%s'<%d> (FF with same input already processed) \n",ffs_done,total_ffs,ffs_done*100/total_ffs,node->get_name(),node->get_child_ff_nodes().size());
                
            }else{
                do_seu_search(g,node);
                
                //if necessary also test the reset:
                Node *rst = node->get_reset_node();
                if (rst != NULL){
                    //printf("\n testing reset node %s\n",rst->get_name());
                    do_seu_search(g,rst);
                }
            }
        }
    }
    
    printf("\r> seu search done. checking for untested FFs:\n");
    
    for(int i=0; i<g->get_node_count(); i++){
        Node *node = g->get_node_by_id(i);
        if ((node != NULL) && (node->is_ff() || node->is_output()) && (node->get_seu_check_done() == 0)){
            printf("> oops forgot node %s ?!\n",node->get_name());
        }
    }
    printf("> done.\n");
    
    //show statistics
    fprintf(stderr, "> statistics:\n");
    fprintf(stderr, "splitsize: %3d, splitcounter %6d\n",SPLITSIZE/3,stat_splitcounter);
    fprintf(stderr, "ffs sensitive: %d\n",stat_seu_sensitive_ffs);
    fprintf(stderr, "\n");
    fprintf(stderr, "subgraph sizes before split:\n");
    for(int i=0; i<17; i++){ fprintf(stderr, "%2d=%d, ",i,stat_subgraph_size_before_split[i]); }
    int sum=0;
    for(int i=17; i<4096; i++){ sum += stat_subgraph_size_before_split[i]; }
    fprintf(stderr, ">17=%d\n",sum);
    fprintf(stderr, "\n");
    fprintf(stderr, "subgraph sizes after split:\n");
    for(int i=0; i<17; i++){ fprintf(stderr, "%2d=%d, ",i,stat_subgraph_size_after_split[i]); }
    sum=0;
    for(int i=17; i<4096; i++){ sum += stat_subgraph_size_after_split[i]; }
    fprintf(stderr, ">17=%d\n\n",sum);
    fprintf(stderr, "\n");
    
    //print sensitive FFs
    for(int i=0; i<stat_sensitive_ffs.size(); i++){
        printf("SSS %s\n",stat_sensitive_ffs.at(i)->get_name());
    }

    
}


void Radiator::_recursive_split_special(Node *node, vector<Node *> *result, int rek)
{
    //this node has direct FF inputs!
    //we need to process this in a special way:
    
    //lets assume we have a LUT4
    //I0 = FF0
    //I1 = FF1
    //I2 = LOGIC
    //I3 = FF2
    //we want to descend deeper into this subgraph
    //but the FFs prevent us from doing so.
    //there is a workaround:
    //1) copy this node 2 times
    //2) set the non FF values to all possible values
    //   (here: LUT4(FF0,FF1,0,FF2) and LUT4(FF0,FF1,1,FF2))
    //3) push those copies onto todo stack
    //4) do recursion on LOGIC Node
    //
    //Problems:
    //- the FFs might be splitted up into several luts
    
    
    //For now lets only handle 4 input nodes for now:
    if (node->get_input_node_count() != 4){
        //we can not handle this yet, push this into todo stack & return
        Node::safe_push(result, node);
        return;
    }
    
    //only handle this if we have 3 direct FF connections
    if (node->get_direct_ff_connection_count() != 3){
        //dont handle this yet
        Node::safe_push(result, node);
        return;
    }
    
    //find out where we have the logic element:
    int non_ff_pos = node->get_first_logic_connection_pos();
    
    //ok we have 3 FFs connected, create three copies
    Node *copy1 = new Node(*node);
    garbage_vector.push_back(copy1);
    
    Node *copy2 = new Node(*node);
    garbage_vector.push_back(copy2);
    
    string s(node->get_name());
    copy1->set_name((s+"_COPY_GND").c_str());
    copy2->set_name((s+"_COPY_VCC").c_str());
    
    //we also need gnd and vcc
    Node *gnd = new Node_GND("GND");
    garbage_vector.push_back(gnd);
    Node *vcc = new Node_VCC("VCC");
    garbage_vector.push_back(vcc);
    //assign gnd/vcc:
    copy1->set_input(gnd, non_ff_pos);
    copy2->set_input(vcc, non_ff_pos);
    
    //push onto todo stack
    Node::safe_push(result, copy1);
    Node::safe_push(result, copy2);
    
    //recursive search on logic child
    _recursive_split(node->get_input(non_ff_pos),result,rek+1);

}

Node *Radiator::create_dummy_triplet(string s)
{
    Node *replacement_triplet_node_VCC = new Node_VCC("__CONST_1__");
    garbage_vector.push_back(replacement_triplet_node_VCC);
    
    string name;
    name = s + "/replacement_triplet_FF_1";
    Node *replacement_triplet_node_FF_1 = new Node_UDP_DFF(name.c_str());
    name = s + "/replacement_triplet_FF_2";
    Node *replacement_triplet_node_FF_2 = new Node_UDP_DFF(name.c_str());
    name = s + "/replacement_triplet_FF_3";
    Node *replacement_triplet_node_FF_3 = new Node_UDP_DFF(name.c_str());
    garbage_vector.push_back(replacement_triplet_node_FF_1);
    garbage_vector.push_back(replacement_triplet_node_FF_2);
    garbage_vector.push_back(replacement_triplet_node_FF_3);
    
    Node *replacement_triplet_node_AND2_1 = new Node_AND2("/replacement_triplet_AND2_1");
    Node *replacement_triplet_node_AND2_2 = new Node_AND2("/replacement_triplet_AND2_2");
    Node *replacement_triplet_node_AND2_3 = new Node_AND2("/replacement_triplet_AND2_3");
    garbage_vector.push_back(replacement_triplet_node_AND2_1);
    garbage_vector.push_back(replacement_triplet_node_AND2_2);
    garbage_vector.push_back(replacement_triplet_node_AND2_3);
    
    Node *replacement_triplet_node_OR3_1 = new Node_OR3("/replacement_triplet_OR3");
    garbage_vector.push_back(replacement_triplet_node_OR3_1);
    
    replacement_triplet_node_FF_1->set_input(replacement_triplet_node_VCC, "D");
    replacement_triplet_node_FF_1->set_input(replacement_triplet_node_VCC, "CP");
    replacement_triplet_node_FF_1->set_input(replacement_triplet_node_VCC, "SDN");
    replacement_triplet_node_FF_1->set_input(replacement_triplet_node_VCC, "CDN");
    
    replacement_triplet_node_FF_2->set_input(replacement_triplet_node_VCC, "D");
    replacement_triplet_node_FF_2->set_input(replacement_triplet_node_VCC, "CP");
    replacement_triplet_node_FF_2->set_input(replacement_triplet_node_VCC, "SDN");
    replacement_triplet_node_FF_2->set_input(replacement_triplet_node_VCC, "CDN");
    
    replacement_triplet_node_FF_3->set_input(replacement_triplet_node_VCC, "D");
    replacement_triplet_node_FF_3->set_input(replacement_triplet_node_VCC, "CP");
    replacement_triplet_node_FF_3->set_input(replacement_triplet_node_VCC, "SDN");
    replacement_triplet_node_FF_3->set_input(replacement_triplet_node_VCC, "CDN");
                
    replacement_triplet_node_AND2_1->set_input(replacement_triplet_node_FF_1, 0);
    replacement_triplet_node_AND2_1->set_input(replacement_triplet_node_FF_2, 1);
                
    replacement_triplet_node_AND2_2->set_input(replacement_triplet_node_FF_2, 0);
    replacement_triplet_node_AND2_2->set_input(replacement_triplet_node_FF_3, 1);
                
    replacement_triplet_node_AND2_3->set_input(replacement_triplet_node_FF_3, 0);
    replacement_triplet_node_AND2_3->set_input(replacement_triplet_node_FF_1, 1);
                
    replacement_triplet_node_OR3_1->set_input(replacement_triplet_node_AND2_1, 0);
    replacement_triplet_node_OR3_1->set_input(replacement_triplet_node_AND2_2, 1);
    replacement_triplet_node_OR3_1->set_input(replacement_triplet_node_AND2_3, 2);
    
    
    return replacement_triplet_node_OR3_1;
}

void Radiator::_recursive_split(Node *node, vector<Node *> *result, int rek)
{
    //printf("%d <%s> %d has %d child ffs \n",rek,node->get_name(),0,node->get_child_ff_nodes().size());        
    //for(int f=0; f < result->size(); f++){
        //printf("%d = %s \n",f,result->at(f)->get_name());
    //}
    if (node->get_seu_check_done()){
        //nothing todo, already checked
        return;
    }
        
    
    if (node->get_child_ff_nodes().size() <= SPLITSIZE){
        //push only if it does not exist in the vector
        Node::safe_push(result, node);
        
    }else{
        stat_splitcounter++;
        //first check: prevent this from ascending down to FFs:
        //but only if this element is not a FF
        if ((!node->is_ff()) && (node->get_direct_ff_connection_count() != 0)){
            //ooops there is a FF connected to this element.
            //call special recursion operator
            printf("SPECIAL SPLIT\n");
            _recursive_split_special(node,result,rek+1);
            return;
        }
        
        //try a split trick:
        //nodes with too many child FFs will be cut and 
        //replaced by a valid triplet/voter:
        //
        // node_a --- node_b <--- {list with 3 child FFs}
        //      \
        //       `--- node_c <--- {list with 999 child FFs}
        //
        // replace with
        // node_a2 --- node_b <--- {list with 3 child FFs}
        //      \
        //       `--- {new valid triplet}
        // node_c
        //
        // and check node_a and node_c independently
        
        //copy node_a because we should not touch the main 
        //netlist. the normal node_a is needed for triplet identification etc!
        //watchout !
        //printf("call to node copy\n");
        Node *node_a2 = node->copy();
        node_a2->set_is_deleteable_copy(1);
        //printf("GVEC push %s @0x%0X\n",node_a2->get_name(),node_a2);
        garbage_vector.push_back(node_a2);
        
        //Node *node_a2 = &node_new;
        
        for(int t=0; t<node_a2->get_input_node_count(); t++){
            Node *chx = node_a2->get_input(t);
            #if !RADIATOR_NEW_OPT_QUIET
            printf("%s: child %d -> %d FF childs\n",node_a2->get_name(),t,chx->get_child_ff_nodes().size());
            #endif
            if (chx->get_child_ff_nodes().size() > SPLITSIZE){
                //too big, replace by valid triplet
                //does not matter that FF triplet is driven by const value, we inject in FFs anyway
                //Node *replacement = create_dummy_triplet(chx->get_name());
                
                //the new handler for input nodes allows us to connect 
                //a single input (will be simulated for 0 and 1!)
                string inputname = chx->get_name() + string("__REPLACEMENT_DUMMY");
                Node *replacement = new Node_INPUT( inputname.c_str() );
                
                node_a2->update_input(replacement, t);
                
                #if !RADIATOR_NEW_OPT_QUIET
                printf("%s: replaced input %d with dummy load\n",node_a2->get_name(),t);
                #endif
                
                //node->print_inputmap();
                //node_a2->print_inputmap();
        
                //and process the rest
                _recursive_split(chx,result,rek+1);
            }else{
            }
        }

        
        //new node should now be small enough
        Node::safe_push(result, node_a2);
        node_a2->set_child_ff_vector_valid(0);
        
        //done with this node
        node->set_seu_check_done(1);
        if ((!node->get_seu_check_done()) && (node->is_ff() || node->is_output())){ ffs_done++; }
        
        /*
        //check ok, descent
        for(int t=0; t<node->get_input_node_count(); t++){
            Node *chx = node->get_input(t);
            printf("%s: child %d -> %d FF childs\n",node->get_name(),t,chx->get_child_ff_nodes().size());
            if (chx->get_child_ff_nodes().size() > SPLITSIZE){
                //too big, do recursive search
                if (!Node::node_in_vector(result, chx)){
                    //printf("REK for node %s:%d, checking child %s\n",node->get_name(),t,chx->get_name());        
                    _recursive_split(chx,result,rek+1);
                }
            }else{
                //small enough, process
                //push only if it does not exist in the vector
                Node::safe_push(result, chx);
            }
        }
        */
    
    }
}

vector<Node *> Radiator::recursive_split(Node *node, int rek)
{
    vector<Node *> result;
    _recursive_split(node, &result, 0);
    return result;
}










//returns    0 if they are not functional equal for sure
//returns +val if they might match, val describing how many configs match
int Radiator::compare_nodes_for_equality(Node *node_a, Node *node_b){
    vector<Node *> node_a_child_ff_vector = node_a->get_child_ff_nodes();
    vector<Node *> node_b_child_ff_vector = node_b->get_child_ff_nodes();
     
    //do a range of tests, starting with those which run in the shortest time
    //and get more and more complex
    
    //1) test number of child ffs
    if (node_a_child_ff_vector.size() != node_b_child_ff_vector.size()){
            //ok, cant be equal, different childcount
            #if !RADIATOR_TRIPLET_ID_QUIET
        printf("# compare(%s,%s) -> not equal num of child ffs (%d VS %d)\n",node_a->get_name(), node_b->get_name(),node_a_child_ff_vector.size(),node_b_child_ff_vector.size());
            for(int i=0; i<node_a_child_ff_vector.size(); i++){
                printf("A:%d %s\n",i,node_a_child_ff_vector.at(i)->get_name());
            }
            for(int i=0; i<node_b_child_ff_vector.size(); i++){
                printf("B:%d %s\n",i,node_b_child_ff_vector.at(i)->get_name());
            }
            #endif
            return 0;
    }
    
    //2)they should have the same list of child ffs
    //in order to compare: sort vector first by child addr
    sort(node_a_child_ff_vector.begin(),node_a_child_ff_vector.end(),Node::compare_by_address);
    sort(node_b_child_ff_vector.begin(),node_b_child_ff_vector.end(),Node::compare_by_address);
    
    //ok now compare element by element
    for(int i=0; i<node_a_child_ff_vector.size(); i++){ //both have equal length
        #if !RADIATOR_TRIPLET_ID_QUIET
        //printf("# child %d (%s,%s)\n",i,node_a_child_ff_vector.at(i)->get_name(), node_b_child_ff_vector.at(i)->get_name());
        //printf("# child %d (0x%x,0x%x)\n",i,node_a_child_ff_vector.at(i), node_b_child_ff_vector.at(i));
        #endif
        if (node_a_child_ff_vector.at(i) != node_b_child_ff_vector.at(i)){
            //no, different child !! exit
            #if !RADIATOR_TRIPLET_ID_QUIET
            printf("# compare(%s,%s) -> different child %d\n",node_a->get_name(), node_b->get_name(),i);
            #endif
            return 0;
        }
        
    }
    
    //3) now we need to verify that they both have the same logic fomula
    //as descending to all FFs is to costly (up to 750FFs -> 2^750 values to test :-X)
    //we try to mark the tree down for node_a and try to find the same path for node_b:
//     printf("marking\n");
    node_a->mark_node();
//     printf("marking done\n");
    
    vector <Node *> res = node_b->find_marked_nodelist(node_a->get_node_mark());
    if (res.size() == 0){
        //impossible to be equal (we return empty list on descending to different FF)
        return 0;
    }
    
    //debug info
    #if !RADIATOR_TRIPLET_ID_QUIET
    for(int i=0; i<res.size(); i++){
        Node *c = res.at(i);
        printf("[%d] %s\n",i,c->get_name());
    }
    #endif
    
    if (res.size()>32){
        //cant do it for now -> abort
        printf("ERROR: cant do comparism for more than 32 inputs (%d)... FIXME!\n\n",res.size());
        exit(-1);
    }
    
    //set up for fingerprinting
    for(int i=0; i<res.size(); i++){
         res.at(i)->set_force_return_stored_value(1);
    }
    
    //ok, do compare fingerprinting
    unsigned int maxconfig = (1<<(res.size()));
//     printf("> %d configs to check\n",maxconfig);
    int equal_count=0;
    for(unsigned int config=0; config<maxconfig; config++){
        
        for(int i=0; i<res.size(); i++){
            Node *in = res.at(i);
            if (config & (1<<i)){
                in->set_value(1);
            }else{
                in->set_value(0);
            }
//             printf("node %s -> set %d, got %d\n",in->get_name(),(config & (1<<i))?1:0,in->get_value(0));
        }
        
        int val_a = node_a->evaluate();
        int val_b = node_b->evaluate();
        
//        printf("%d<->%d\n", val_a, val_b);
        //printf("%d<->%d\n", node_a->get_input(0)->get_input(0)->get_value(config), node_b->get_input(0)->get_input(0)->get_value(config));
        
        #if !RADIATOR_TRIPLET_ID_QUIET
        printf("[%10d] --> %d VS %d\n",config,val_a,val_b);
        #endif
        if (val_a == val_b){
            equal_count++;
        }else{
            #if !RADIATOR_TRIPLET_ID_QUIET
            printf("MISMATCH -> not identical\n");
            /*printf("digraph a{\n");
            node_a->print_graphviz(0);
            printf("\n\/\/second\n\n");
            node_b->print_graphviz(0);
            printf("}\n");
            exit(0);*/
            #endif
            //cleanup
            //for(int i=0; i<res.size(); i++){
                //res.at(i)->set_force_return_stored_value(0);
            //}
            //return 0;
        }
    }
    
    //cleanup
    for(int i=0; i<res.size(); i++){
        res.at(i)->set_force_return_stored_value(0);
    }
    
    return equal_count;
    
    //perfect, should be identical now!
    return 1;
}

/*
int Radiator::do_seu_test_for_node(Graph *g, Node *target)
{
    if (target->get_child_ff_nodes().size() == 0){
        //no check necessary, not driven by any FFs
        return 1;
    }

    //assign all inputs to all possible fixed values
    vector<Node *> input_vector = target->get_child_input_nodes();
    
    
    int ffsdone = 0;
    if (input_vector.size() > 0){
        unsigned int maxconfig = (1<<(input_vector.size()));
        for(unsigned int config=0; config<maxconfig; config++){
            if ((config & 0x00FF) == 10){
                printf("            \r> %d INPUT and %d FF nodes to Test = %.2fM configurations to check  [ %5.2f%% ]",
                    target->get_child_input_nodes().size(),target->get_child_ff_nodes().size(), (1L<<target->get_child_input_nodes().size())*(1L<<target->get_child_ff_nodes().size())/1000000.0,config*100.0/maxconfig);
                cout.flush();
            }
    //         printf("\r> testing input vector [");
            for(int i=0; i<input_vector.size(); i++){
                Node *in = input_vector.at(i);
                if (config & (1<<i)){
    //                 printf("1");
                    in->set_value(1);
                }else{
    //                 printf("0");
                    in->set_value(0);
                }
            }
    //         printf("]\n");
            
            //do seu test
            ffsdone = _do_seu_test_for_node(g,target);
            
        }
        printf(" done.");
        cout.flush();
    }else{
        //no input nodes to test
        ffsdone = _do_seu_test_for_node(g,target);
    }
    
    return ffsdone;
}*/
    

int Radiator::do_seu_test_for_node(Graph *g, Node *target)
{   
    int i,j;
    
    if (target == NULL){
        printf("ERROR: target is NULL. exiting\n");
        exit(EXIT_FAILURE);
    }
    
    if (target->get_child_ff_nodes().size() == 0){
        //no check necessary, not driven by any FFs
        if ((!target->get_seu_check_done()) && (target->is_ff() || target->is_output())){ ffs_done++; }
        stat_subgraph_size_after_split[0]++;
        return 0;
    }
    
    //printf("   \r> fetching child FFs... ");
    //cout.flush();

    //step1:
    // retrieve the List of FFs feeding this target FF
    vector <Node*> source_vector = target->get_child_ff_nodes();
    
    //printf("done");
    //cout.flush();
    
    //log size after split
    if (source_vector.size()/3>4096){
        printf("ERROR: cannot log, only space for 4096 entries. FIX: change size in radiator.h\n\n");
        exit(-1);
    }
    stat_subgraph_size_after_split[source_vector.size()/3]++;
    
    //for(int i=0; i<source_vector.size(); i++){
    //    printf("CHILD %d = %s\n",i,source_vector.at(i)->get_name());
    //}
    
    //step2:
    // ok we have the source_vector with the input ffs.
    // in order to get "valid" start configurations for those
    // FFs we need to check if some of the FFs share the same input.
    // in order to do this we:
    // - sort nodes while using the cmp_child_address function
    //   as comparism 
    // - result is a list with ffs which have the same childnode next to each other
    //
    sort(source_vector.begin(),source_vector.end(),Node::compare_by_child_address);
    
    
    //unique node to group id mapping array,
    //the idea behind this is like this:
    //- we have an array groupid
    //- for a given position i the object in source_vector groupid[i] is the "group" this obj belongs to
    //int *groupid = new int[source_vector.size()];
    int groupid[source_vector.size()];
    int last = 0;
    int group_counter = 0;
    
    vector<int> no_triplet_ids_vector;
    vector<int> triplet_vector;
    
    //calculate the array values
    for(int i=0; i<source_vector.size(); i++){
        if ((i!=0) && (source_vector.at(i-1)->get_ff_input_node() != source_vector.at(i)->get_ff_input_node())){
            last++;
            if ((group_counter%3) != 0){
//                  printf("group not valid\n");
                no_triplet_ids_vector.insert(no_triplet_ids_vector.end(), triplet_vector.begin(), triplet_vector.end());
            }
            group_counter = 1;
            triplet_vector.clear();
            //printf("------------\n");
        }else{
            group_counter++;
        }
//         printf("GROUP %d: %s\n",1<<last,source_vector.at(i)->get_name());
        groupid[i] = (1<<last);
        triplet_vector.push_back(i);
    }
    if ((group_counter%3) != 0){
        printf("group not valid\n");
        no_triplet_ids_vector.insert(no_triplet_ids_vector.end(), triplet_vector.begin(), triplet_vector.end());
    }
    
    for (int n=0; n<no_triplet_ids_vector.size(); n++){
        printf("entry %d -> %d (group %d)\n",n,no_triplet_ids_vector.at(n),groupid[no_triplet_ids_vector.at(n)]);
    }
    
    if (no_triplet_ids_vector.size() != 0){
        //ooops we were unable to detect some triplets!
        debug_printf("\r> unable to detect some triplets, trying harder... (might take some time)\n");
        //todo: find group estimation
        
        //
        for (int n=0; n<no_triplet_ids_vector.size(); n++){
            int compare_nodes[no_triplet_ids_vector.size()];
            
            int id_a = no_triplet_ids_vector.at(n);
            Node *node_a = source_vector.at(id_a);
                    
            for (int m=0; m<no_triplet_ids_vector.size(); m++){
                int id_b = no_triplet_ids_vector.at(m);
                
                if (groupid[id_a] != groupid[id_b]){
                    //now compare nodes
                    Node *node_b = source_vector.at(id_b);
//                     printf("comparing\n");
                    compare_nodes[m] = compare_nodes_for_equality(node_a, node_b);
                }else{
                    //never assign when same group already
                    compare_nodes[m] = 0;
                }
            }
            
            //ok, compared node n against m nodes, now set group to max match
            int max_val=0;
            int max_pos=-1;
            for(int m=0; m<no_triplet_ids_vector.size(); m++){
                if (compare_nodes[m] > max_val){
                    max_pos = m;
                    max_val = compare_nodes[m];
                }
            }
            if (max_pos==-1){
                //not equal!
            }else{
                int id_b = no_triplet_ids_vector.at(max_pos);
                Node *node_b = source_vector.at(id_b);
                printf("> node %s equals %s (matches=%d), setting groupid to %d\n",node_a->get_name(), node_b->get_name(), max_val, id_b);
                
                //change now all groups that equal group[id_a] to new val
                int old_group = groupid[id_a];
                for(int i=0; i<source_vector.size(); i++){
                    if(groupid[i] == old_group){
                        groupid[i] = groupid[id_b];
                    }
                }
            }
        }
//     }else{
//    return 1;
    }
    
    //store the last group
    int last_group = last;
    
    
    //we use a 32bit int for bit manipulation position. thus we can only do up to 32 bits!
    //see below where we do the source_vector.at()->set_value()
    if (last_group >= 32){
        printf("\rFIXME: can do only 32 input tests... requested %d !! skipped the node above. EXITING NOW - NOT FINISHED !\n(maybe adjust SPLITSIZE in radiator.cpp)\n\n",last_group);
        //exit(EXIT_FAILURE);
        return 0;
    }
   
    
    
    //debug info
    #if !RADIATOR_INJECT_TARGET_QUIET
    printf("\r--- list of children for %s ---\n", target->get_name());
    for(int i=0; i<source_vector.size(); i++){
    printf("> node %s (@%d) %s--> group %d \n",source_vector.at(i)->get_name(),source_vector.at(i),(source_vector.at(i)->is_input()?"[INPUT]":""),groupid[i]);
    }
    printf("--- done ---\n");
    #endif
    
    //printf("                             \r> %d input FFs -> %5.2f%% done ",last_group,0.0);
    //cout.flush();
    
    time_t start_time = time(NULL);
    
    //now we calculate all possible "valid" configurations for the input ffs (00..0 to 11..1)
    int max_flip_pos = source_vector.size();
    int correct_value;
    int flip;
    
    int total_iterations = (pow(2,(last_group+1))-1);
        
    //testing
    //if (total_iterations > 1000){
    //    total_iterations =1000000;
    //}
    
    if (target->get_child_ff_nodes().size() == 0){
        //no check necessary, not driven by any FFs
        return 1;
    }

    //assign all inputs to all possible fixed values
    vector<Node *> input_vector = target->get_child_input_nodes();
    
    ///set up all possible input combinations
    unsigned int maxconfig = (1<<(input_vector.size()));
    for(unsigned int config=0; config<maxconfig; config++){
        int ffsdone = 0;
    
        if ((config & 0x0FFF) == 0){
            printf("                    \r> %d INPUT and %d FF nodes to Test = %.2fM configurations to check  [ %5.2f%% ]", target->get_child_input_nodes().size(),target->get_child_ff_nodes().size(), (1L<<target->get_child_input_nodes().size())*(1L<<target->get_child_ff_nodes().size())/1000000.0,config*100.0/maxconfig);
                cout.flush();
        }
//         printf("\r> testing input vector [");
        for(int n=0;n<input_vector.size(); n++){
            Node *in = input_vector.at(n);
            if (config & (1<<n)){
//                 printf("1");
                in->set_value(1);
            }else{
//                 printf("0");
                in->set_value(0);
            }
        }
    //         printf("]\n");

            //do seu test
    
        int seu_found = 0;
        int sensitive[source_vector.size()];;
        for(int m=0; m<source_vector.size(); m++){
            sensitive[m] = 0;
        }
        
        for(int valid_in=0; valid_in <= total_iterations; valid_in++){
    
            if ((maxconfig == 1) &&((valid_in & 0xFFF) == 10)){
                time_t elapsed =  time(NULL) - start_time;
                
                //for valid_in we needed elapsed time.
                float minutes_done = ((float)elapsed)/60.0;
                float minutes_todo = ((((float)elapsed)/60.0)/((float)valid_in) * total_iterations) - minutes_done;
        
                printf("            \r> %d input FFs = %.2fM configurations to check [ %5.2f%% / ERT %.2f min ] done @%.2fk eval/s ",last_group,total_iterations/1000000.0,((float)valid_in*100.0)/((float)total_iterations),minutes_todo,(((max_flip_pos+1)*valid_in)/60)/minutes_done/1000.0);
                cout.flush();
            }
            
    #if SHOW_BITVECTORS
            printf("\rtesting [");
    #endif
            
            for(i=0; i<source_vector.size(); i++){
                if (valid_in & groupid[i]){
    #if SHOW_BITVECTORS
                    debug_printf("1");
    #endif
                    source_vector.at(i)->set_value(1);
                }else{
    #if SHOW_BITVECTORS
                    debug_printf("0");
    #endif
                    source_vector.at(i)->set_value(0);
                }
            }
            
            //evaluate correct value
            correct_value = target->evaluate();
            
    #if SHOW_BITVECTORS
            debug_printf("] is %d, injecting... \n", correct_value );
    #endif
            
            //ok, now do injection in a intelligentway: we now from "valid calc"
            //before that all FFs are now in a correct value.
            //-> only flip the bit we actually want to:
            for(flip=0; flip<max_flip_pos; flip++){
                //do the flip
                source_vector.at(flip)->flip();
                
                //evaluate target
                int seu_value = target->evaluate();
                
    #if SHOW_BITVECTORS
                printf("    inj [");
                for(int p=0; p<max_flip_pos; p++){
                    printf("%d",source_vector.at(p)->get_value(0));
                }
                printf("] is %d\n",seu_value);
    #endif
                
                if (seu_value != correct_value){
                    //print out which ff caused the problem:
                    #if !RADIATOR_QUIET2
                    printf("\r> ALERT: SEU sensitive FF found: %s for config %d!=%d [", source_vector.at(flip)->get_name(),correct_value,seu_value);
                    #endif
                    
                    for(int i=0; i<max_flip_pos; i++){
                        if (i==flip){
                            #if !RADIATOR_QUIET2
                            printf("#");
                            #endif
                            sensitive[i] = 1;
                        }else{
                            #if !RADIATOR_QUIET2
                            printf("%d",source_vector.at(i)->get_value(0));
                            #endif
                        }
                    }
                    #if !RADIATOR_QUIET2
                    printf("]\n");
                    #endif
                    seu_found++;
                }
                
                //flip back
                source_vector.at(flip)->flip();
            }
            ///every now & then test if we can abort because all sensitive FFs were already found
            if (seu_found && ((valid_in & 0xFFF) == 10)){
                //check if all FFs are already sensitive:
                int done = 1;
                for(i=0; i<source_vector.size(); i++){
                    if (sensitive[i] == 0){
                        done = 0;
                        i=source_vector.size()+1;
                    }
                }
                if (done){
                    valid_in = total_iterations+1; ///aborthack for test
                }
            }
        }
        
        if (seu_found){
            
            seu_sensitive_ff_count++;
            
            //printf("digraph a{\n");
            //target->print_graphviz(0);
            //printf("}\n");
            
            //push sensitive ffs to logger
            for(i=0; i<source_vector.size(); i++){
                if (sensitive[i]){
                    Node *sn = source_vector.at(i);
                    log_sensitive_ff(sn);
                }
            }
            
            printf("\r> found %d SEU sensitive configurations for %s (see above). Total FFs sensitive so far: %d\n\n",seu_found,target->get_name(),stat_seu_sensitive_ffs);
            
            #if !RADIATOR_QUIET
            printf("  NOTE: keep in mind that if you find an SEU sensitive part during LOGIC\n");
            printf("        recursion because of subgraph dividing this might be no real problem!\n");
            printf("        (for example if we divided through a voter)\n\n");
            printf("        On the other hand if you do NOT find any SEU sensitive parts you can be\n");
            printf("        sure there exists no problem.\n\n");
            
            printf("        The following FFs caused bitflips in tested Node (%s):\n",target->get_name());
            
            
            
            for(i=0; i<source_vector.size(); i++){
                if (sensitive[i]){
                    Node *sn = source_vector.at(i);
                    printf("        - %d/g%d: %s (@0x%X)\n",i,groupid[i],sn->get_name(),sn);
                }
            }
            printf("\n\n");
            
            printf("        The following FFs are ok:\n");
            for(i=0; i<source_vector.size(); i++){
                if (sensitive[i] == 0){
                    Node *sn = source_vector.at(i);
                    printf("        - %d/g%d: %s (@0x%X)\n",i,groupid[i],sn->get_name(),sn);
                }
            }
            
            printf("\n\n");
            
            printf("        The following Nodes were handeled as if they are SAFE inputs (no SEU possible):\n");
            vector <Node*> input_vector = target->get_child_input_nodes();
            for(i=0; i<input_vector.size(); i++){
                Node *sn = input_vector.at(i);
                printf("        - %d: %s (@0x%X) := %d \n",i,sn->get_name(),sn,sn->get_value(0));
            }
            
            printf("\n\n");
            ///target->print_graphviz(0);
            
            
            printf("  this node will be skipped now for all other fixed input values!\n\n");
                    
            printf(" > press ENTER to continue or CTRL+C to exit\n");
            cin.sync();  // purge any \n
            cin.get();   // console wait
            #endif
            
            config=maxconfig+1;
            
        } 
    }
    //printf("done\n");
    
    //find all nodes that share the same input:
    //(will include this node <node> also
    for(int j=0; j<g->get_node_count(); j++){
        Node *dup = g->get_node_by_id(j);
        if ((dup != NULL) && (dup->is_ff()) && (target->is_ff())){
            if (dup->get_ff_input_node() == target->get_ff_input_node()){
                if (!dup->get_seu_check_done()){
                    //we do not have to check this FF again
                    dup->set_seu_check_done(1);
                    ffs_done++;
                }
            }
        }
    }
    
    //count node also if input
    if ((!target->get_seu_check_done()) && (target->is_ff() || target->is_output())){ ffs_done++; }
    target->set_seu_check_done(1);
    
    return ffs_done;
}




