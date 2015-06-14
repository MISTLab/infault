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
#include "analyzer.h"
#include "radiator.h"
#include "debug.h"
#include <math.h>
#define max(a,b) ((a)>(b)?(a):(b))
using namespace std;


Analyzer::Analyzer()
{
}


Analyzer::~Analyzer()
{
}


int Analyzer::closer_match(string target, string a, string b)
{
    const char *target_buf = target.c_str();
    const char *a_buf = a.c_str();
    const char *b_buf = b.c_str();
    
    int maxi = max(max(target.length(), a.length()), b.length());
    for(int i=0; i<maxi; i++){
        int dist_a = abs(target_buf[i] - a_buf[i]);
        int dist_b = abs(target_buf[i] - b_buf[i]);
        if (dist_a > dist_b){
            return 0;
        }else if (dist_a < dist_b){
            return 1;
        }else{
            //equal, continue
        }
    }
    printf("ERRRRRRRRR\n");
    exit(-1);

}
/*int Analyzer::id_distance(Node *a, Node *b)
{
    const char *a_buf = a->get_node_id().c_str();
    const char *b_buf = b->get_node_id().c_str();
    
    int maxi = max(a->get_node_id().length(), b->get_node_id().length());
    int dist = maxi;
    for(int i=0; i<maxi; i++){
        if (a_buf[i] == b_buf[i]){
            dist--;
        }else{
            printf("dist(%s, %s) = %d\n",a->get_name(),b->get_name(),dist);
            return dist;
        }
    }
    printf("dist(%s, %s) = %d\n",a->get_name(),b->get_name(),dist);
    return dist;
}*/


int Analyzer::analyze_node(Graph *g, Node *target, int ffs_done)
{
    if (target == NULL){
        printf("ERROR: %s called with NULL node. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
    if (g == NULL){
        printf("ERROR: %s called with NULL graph. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
    //not driven by ffs ?
    if (target->get_child_ff_nodes().size() == 0){
        //no check necessary, not driven by any FFs, return OK
        return 1;
    }
    
    //step1: retrieve the List of FFs feeding this target FF
    vector <Node*> source_vector = target->get_child_ff_nodes();
    
    //step2: find triplets the easy way: same inputnode -> same group
    sort(source_vector.begin(),source_vector.end(),Node::compare_by_child_address);
    
    int groupid[source_vector.size()];
    int groupid2addr[source_vector.size()];
    int last = 0;
    int group_counter = 0;
    
    vector<int> no_triplet_ids_vector;
    vector<int> triplet_vector;
    
    //find groups
    for(int i=0; i<source_vector.size(); i++){
        if ((i!=0) && (source_vector.at(i-1)->get_ff_input_node() != source_vector.at(i)->get_ff_input_node())){
            last++;
            if ((group_counter%3) != 0){
//                 printf("group not valid\n");
                no_triplet_ids_vector.insert(no_triplet_ids_vector.end(), triplet_vector.begin(), triplet_vector.end());
            }
            group_counter = 1;
            triplet_vector.clear();
        }else{
            group_counter++;
        }
//         printf("GROUP %d: %s\n",1<<last,source_vector.at(i)->get_name());
        groupid[i] = last;
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
                    compare_nodes[m] = Radiator::compare_nodes_for_equality(node_a, node_b);
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
    
    
    //build mapping groupid to first triplet ff addr:
    for(int i=0; i<source_vector.size(); i++){
        groupid2addr[i]=0;
    }
    for(int i=0; i<source_vector.size(); i++){
        if (groupid2addr[groupid[i]]==0){
            //update mapping: store address of first FF
            groupid2addr[groupid[i]]=(int)source_vector.at(i);
        }
    }
    
    
    //setup triplets
    for(int i=0; i<source_vector.size(); i++){
        //look for all FFs with groupid
        vector<Node *> groupffs_vector;
        for(int j=0; j<source_vector.size(); j++){
            if (groupid[j] == i){
                //group found, push!
                groupffs_vector.push_back(source_vector.at(j));
            }
        }
        if (groupffs_vector.size() == 0){
            //ok, no ffs in this group
        }else if (groupffs_vector.size() > 3){
            //groups should have 0 or 3 FFs !
            printf("ERROR: ABORT! group %d has %d elements\n",i,groupffs_vector.size());
            exit(0);
        }else if(groupffs_vector.size() == 3){
            //perfect! setup group
            for(int x=0; x<groupffs_vector.size(); x++){
                groupffs_vector.at(x)->set_triplet(0, groupffs_vector.at(0));
                groupffs_vector.at(x)->set_triplet(1, groupffs_vector.at(1));
                groupffs_vector.at(x)->set_triplet(2, groupffs_vector.at(2));
                ///triplet_vector.at(t)->set_is_safe(1);
                //printf("%d %s\n",t,triplet_vector.at(t)->get_name());
            }
        }else{
            printf("WARNING: %d elements in triplet ?!?!\n",groupffs_vector.size());
            exit(0);
        }
            
    }
            
     
}


#if 0
int Analyzer::analyze_node(Graph *g, Node *target, int ffs_done)
{
    int i,j;
   
    if (target == NULL){
        printf("ERROR: %s called with NULL node. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
    if (g == NULL){
        printf("ERROR: %s called with NULL graph. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
    
    debug_analyze_verbose_printf("analyze_node %s called\n",target->get_name());
    cout.flush();
    
    if (target->get_is_safe()){
        //its ok, dont check again
        debug_analyze_verbose_printf("safe, ret 1\n");
        return 1;
    }
    

    if (target->get_is_input()){
        //inputs are safe
        debug_analyze_verbose_printf("input, ret 1\n");
        target->set_is_safe(1);
        return 1;
    }
    
    if (target->is_ff()){
        //dont check directly, descend to data input
        //printf("rekursion for FF %s\n",target->get_name());
        /*int res = analyze_node(g, target->get_ff_input_node(), 0);
        if(res){
            target->set_is_safe(1);
            //printf("----  %s.\n",target->get_name());
            debug_analyze_verbose_printf("is ff, ret 1\n");
            return 1;
        }else{
            debug_analyze_verbose_printf("is ff, ret 0\n");
        return 0;
        }*/
        for(int i=0; i<target->get_input_node_count(); i++){
            int res = analyze_node(g, target->get_input(i), 0);
            if (res == 0){
                printf("FF0: %s:%s\n",target->get_name(),target->get_input_name(i).c_str());
                return 0;
            }
        }
        //finished without err -> return 1
        return 1;
    }
    
    //check count of FFs for this node:
    vector <Node*> source_vector = target->get_child_ff_nodes();
    int childffcount = source_vector.size(); //unsafe_child_ff_nodes().size();
    
    if (childffcount == 0) {
        //be quiet because there are no unsafe FFs
        debug_analyze_verbose_printf("quiet %s(%d)\n",target->get_name(),target);
        return 1;
    }else if (childffcount < 3) {
        //printf("ERROR %d in FFs only for %s\n",childffcount,target->get_name());
        debug_analyze_verbose_printf("<3 childs, ret 0\n");
        for(int i=0; i<childffcount; i++){
            debug_analyze_verbose_printf("%d - %s\n",i,source_vector.at(i)->get_name());
        }
        for(int i=0; i<target->get_child_ff_nodes().size(); i++){
            debug_analyze_verbose_printf("%d: %s - %d\n",i,target->get_child_ff_nodes().at(i)->get_name(), target->get_child_ff_nodes().at(i)->get_is_safe());
        }
        //target->print_graphviz(0);
        cout.flush();
        return 0;
    }else if(childffcount == 3) {
        //perfect triplet
        //printf("MARK SAFE: %s\n",target->get_name());
        //vector <Node*> source_vector = target->get_child_ff_nodes();
        //for(int i=0; i<source_vector.size(); i++){
        //    printf("CHILD %d = %s\n",i,source_vector.at(i)->get_name());
        //}
        //exit(0);
        //printf("formula '%s'\n",target->get_formula().c_str());
        
        //vector <Node*> source_vector = target->get_unsafe_child_ff_nodes();
        int res = 0;
        for(int i=0; i<8; i++){
            for(int j=0; j<3; j++){
                if (i & (1<<j)){
                    source_vector.at(j)->set_value(1);
                }else{
                    source_vector.at(j)->set_value(0);
                }
            }
            res = (res<<1) |  (target->evaluate() & 0x01);
        }
        if ((res == 0xE8) || (res = 0x17)){
            //this is a voter!
            target->set_is_safe(1);
            
            //set triplet info:
            for(int t=0; t<3; t++){
                debug_analyze_verbose_printf("set triplet %d - %s(%d) = {",t,source_vector.at(t)->get_name(),source_vector.at(t));
                debug_analyze_verbose_printf("%s, ",source_vector.at(0)->get_name());
                debug_analyze_verbose_printf("%s, ",source_vector.at(1)->get_name());
                debug_analyze_verbose_printf("%s}\n",source_vector.at(2)->get_name());
                source_vector.at(t)->set_triplet(0, source_vector.at(0));
                source_vector.at(t)->set_triplet(1, source_vector.at(1));
                source_vector.at(t)->set_triplet(2, source_vector.at(2));
                ///source_vector.at(t)->set_is_safe(1);
            }
            debug_analyze_verbose_printf("3in triplet detected.\n");
            return 1;
        }else{
            //oops, not a voter
            debug_analyze_printf("nonvoter (%02X)\n",res);
            return 0;
        }
    }else{
        //hmmm still to big, try to descend
        //check all children
        for (i=0; i<target->get_input_node_count(); i++){
            Node *node = target->get_input(i);
            if (node == NULL){
                debug_analyze_printf("NULL node .... \n");
            }else{
                if (node->is_ff()){
                    //do not follow FFs !!
                    //do nothing
                    debug_analyze_verbose_printf("no rekursion for %s.\n",node->get_name());
                }else{
                    //printf("rekursion for %s.\n",node->get_name());
                    int res = analyze_node(g,node,0);
                    if (!res){
                        //no, we cant descend more, we need to analyse it now!
                        ///get all FFs connected! ALL! not only the unsafe ones!
                        vector<Node *> chnodes_in = target->get_child_ff_nodes();
                        childffcount = chnodes_in.size();
                        
                        if (childffcount > 31){
                            printf("ERROR: maxchildsize supported is 32! aborting. \n");
                            exit(-1);
                        }
                        debug_analyze_verbose_printf("INFO  %d in FFs for %s=%s::%s\n",childffcount,node->get_name(),target->get_name(),target->get_input_name(i).c_str());
                        //target->print_graphviz(0);
                        
                        
                        //remove duplicates
                        vector<Node *> chnodes;
                        for(int i=0; i<chnodes_in.size(); i++){
                            int known = 0;
                            for(int j=0; j<chnodes.size(); j++){
                                if (chnodes_in.at(i) == chnodes.at(j)){
                                    known = 1;
                                }
                            }
                            if (!known){
                                chnodes.push_back(chnodes_in.at(i));
                            }
                        }
                        
                        
                        for(int i=0; i<chnodes.size(); i++){
                            debug_analyze_verbose_printf("%d = %s\n",i,chnodes.at(i)->get_name());
                        }
                        
                        //generate logic table 
                        int num_max = chnodes.size();
                        long l = 0;
                        long max = (1<<num_max);
                        for(l=0; l<max; l++){
                            for(int n=0; n<num_max; n++){
                                if ((1<<n) & l){
                                    //printf("1");
                                    chnodes.at(n)->set_value(1);
                                }else{
                                    //printf("0");
                                    chnodes.at(n)->set_value(0);
                                }
                            }
                            //printf (" = %d\n", target->evaluate());
                            
                        }
                        
                        
                        vector<int> voter_ids;
                        int quick = 1;
                        while(quick){
                            for(int a=0; a<num_max; a++){
                                for(int b=0; b<num_max; b++){
                                    for(int c=0; c<num_max; c++){
                                        if ((a!=b) && (b!=c) && (c!=a)){
                                            //unique tripel
                                            //NOTE: we check way too many triplets!
                                            //      permutation of the voter inputs do not matter, 
                                            //      but we check them anyway here (just for now, FIXME for later)
                                            
                                            //build inputs wihtout those 3 ffs:
                                            vector<Node *> nodes_ut;
                                            for(int i=0; i<num_max; i++){
                                                if ((i!=a) && (i!=b) && (i!=c)){
                                                    //add
                                                    nodes_ut.push_back(chnodes_in.at(i));
                                                }
                                            }
                                            
                                            //setup all permutations of nodes_ut
                                            long val_ut = 0;
                                            long max_ut = (1<<nodes_ut.size());
                                            int res_abc = -1;
                                            int voter = 1;
                                            for(val_ut=0; (val_ut<max_ut) && (voter); val_ut++){
                                                for(int n=0; n<nodes_ut.size(); n++){
                                                    if ((1<<n) & val_ut){
                                                        //printf("1");
                                                        nodes_ut.at(n)->set_value(1);
                                                    }else{
                                                        //printf("0");
                                                        nodes_ut.at(n)->set_value(0);
                                                    }
                                                }
                                                //
                                                //printf("\n");
                                
                                                //now setup all a,b,c values
                                                res_abc = 0;
                                                for(int val=0; val<(1<<3); val++){
                                                    //setup those FFs:
                                                    chnodes_in.at(a)->set_value( (val&(1<<0))?1:0 );
                                                    chnodes_in.at(b)->set_value( (val&(1<<1))?1:0 );
                                                    chnodes_in.at(c)->set_value( (val&(1<<2))?1:0 );
                                                    //printf (" = %d\n", target->evaluate());
                                                    res_abc = (res_abc<<1) |  (target->evaluate() & 0x01);
                                                    //ok
                                                }
                                                if ( ! ((res_abc == 0x00) || (res_abc == 0xFF) || (res_abc == 0xE8) || (res_abc ==0x17))){
                                                    //printf("\nERROR, abc %d%d%d not voter! reason: 0x%0X as res\n",a,b,c,res_abc);
                                                    //break
                                                    voter=0;
                                                }
                                            }
                                            if (voter){
                                                //printf("possible voter for %d%d%d -> 0x%02X\n",a,b,c,res_abc);
                                                //push voter id to array
                                                int voter_id = (1<<a) | (1<<b) | (1<<c);
                                                //but only if unknown
                                                int id_known = 0;
                                                for (int x=0; x<voter_ids.size(); x++){
                                                    if (voter_ids.at(x) == voter_id){
                                                        id_known = 1;
                                                    }
                                                }
                                                if (!id_known){
                                                    voter_ids.push_back(voter_id);
                                                    debug_analyze_verbose_printf("detected possible voter for %d%d%d (voterid %d)-> 0x%02X\n",a,b,c,voter_id,res_abc);
                                                }
                                                //quick hack to speedup:
                                                a = max(max(a,b),c);
                                                b = max(max(a,b),c);
                                                c = max(max(a,b),c);
                                            }
                                            //printf("---------------------------\n");
                                        }
                                    }
                                }
                            }
                        
                            //ok we have now our voterids
                            //we need to find a combination of those so that
                            //all inputs are cover
                            //FIXME: to test ALL cases we need to test all permutations instead of this q&d hack!!!
                            int covered = 0;
                            debug_analyze_verbose_printf("%d voter ids to check\n",voter_ids.size());
                            for (int x=0; x<voter_ids.size(); x++){
                                if ((voter_ids.at(x) & covered) == 0){
                                    //AND combination of both do not give any hits
                                    //-> those outputs are not covered yet, so try to cover
                                    covered = covered | voter_ids.at(x);
                                    //set triplet info:
                                    vector<Node*> triplet_vector;
                                    for(int t=0; t<num_max; t++){
                                        if (voter_ids.at(x) & (1<<t)){
                                            triplet_vector.push_back(chnodes_in.at(t));
                                        }
                                    }
                                    
                                    if (triplet_vector.size() != 3){
                                        //whooops should not happen!
                                        printf("strange, expected 3 nodes for triplet. exiting\n");
                                        exit(-1);
                                    }
                                    
                                    for(int t=0; t<3; t++){
                                        triplet_vector.at(t)->set_triplet(0, triplet_vector.at(0));
                                        triplet_vector.at(t)->set_triplet(1, triplet_vector.at(1));
                                        triplet_vector.at(t)->set_triplet(2, triplet_vector.at(2));
                                        ///triplet_vector.at(t)->set_is_safe(1);
                                    //printf("%d %s\n",t,triplet_vector.at(t)->get_name());
                                    }
                                    if (target->is_ff()){
                                        target->set_is_safe(1);
                                    }
                                }
                            }
                            //covered should now be 111....11
                            if (covered == (max-1)){
                                //fine!
                                debug_analyze_verbose_printf("covered, ret 1\n");
                                if (node->is_ff()){
                                    node->set_is_safe(1);
                                }
                                return 1;
                            }else{
                                //ooops not covered -> unsafe
                                //maybe this was the quick run! retry harder
                                if (quick){
                                    quick = 0;
                                }else{
                                    //no, really not covered :(
                                    printf("NOT covered, ret 0\n");
                                    return 0;
                                }
                            }
                        }
                    }else{
                        //has been divided and checked -> nothing todo
                    }
                }
            }
        }
        //done without err -> ok
        ///target->set_is_safe(1);
        debug_analyze_verbose_printf("no error found, ret 1\n");
        return 1;
    }
    
    printf("FINISHED.exiting\n");
    exit(0);
    
    //return ffs_done;
}

#endif
                       
                       
void Analyzer::show_activity_icon(int count)
{
    //backspace
    printf("%c",8);
    switch(count%4){ 
        default:
            case 0: printf("|"); break;
            case 1: printf("/"); break;
            case 2: printf("-"); break;
            case 3: printf("\\"); break;
    }
}

void Analyzer::analyze(Graph *g)
{
    if (g == NULL){
        printf("ERROR: %s called with NULL graph. exiting",__PRETTY_FUNCTION__);
        exit(EXIT_FAILURE);
    }
    
//     Node *dbg = g->get_node_by_name("DUT/pci_core/r_reg_0_2113__Q");
//     analyze_node(g,dbg,0);
//     exit(-1);
    
    printf("> running full analysis...\n");
    
    int ffs_done = 0;
    int total_ffs = g->get_node_vector_ff_count();
    
    printf("> doing triplet check...\n");
    
    int count=0;
    for(int i=0; i<g->get_node_count(); i++){
        Node *node = g->get_node_by_id(i);
        //////////////////////////////
        if (count++ >= 50){
            printf("\r                                                                                            \r> checking triplication  %3d%% done : checking %s /",i*100/g->get_node_count(),node->get_name());
            cout.flush();
            count=0;
        }
        show_activity_icon(count);
        //////////////////////////
        
        
        if ((node != NULL) && (node->is_output() || node->is_ff())){
            //printf("%s(%d)\n",node->get_name(),node);
            int res = analyze_node(g,node,ffs_done);
            if (!res){
                printf("\r> WARNING: triplication violation for %s!                          \n",node->get_name());
                //sleep(1);
                //exit(-1);
            }
        }
        
        //check all inputs of this node:
        /*int ffs = 0;
        for (int j=0; j<node->get_input_node_count(); j++){
            Node *node2 = node->get_input(j);
            if (node2->is_ff()){
                ffs++;
            }
        }
        
        if ((node != NULL) && (ffs > 0)){
                int res = analyze_node(g,node,ffs_done);
                if (!res){
                       printf("analyze failed for %s\n",node->get_name());
                       sleep(1);
                       //exit(-1);
                }
        }*/
        
    }
    printf("> done.\n");
    printf("> checking reset/set triplication...\n");
    cout.flush();
    
    for(int i=0; i<g->get_node_count(); i++){
        Node *node = g->get_node_by_id(i);
        ////////////////
        if (count++ >= 50){
            printf("\r                                                                                             \r> checking triplication  %3d%% done : checking %s /",i*100/g->get_node_count(),node->get_name());
            cout.flush();
            count=0;
        }
        show_activity_icon(count);
        /////////////////////////
        
        
        //printf("\nNODE %s(%d)\n",node->get_name(),node);
        if ((node != NULL) && (node->is_ff())){
            
            if (!node->get_triplet_ok()){
                printf("\nno triplet for %s\n",node->get_name());
            }else{
                printf(".");
                
                if (!node->get_is_processed()){
                    
                    Node *t0 = node->get_triplet(0);
                    Node *t1 = node->get_triplet(1);
                    Node *t2 = node->get_triplet(2);
                    
                    for(int n=0; n<t0->get_input_node_count(); n++){
                        Node *i0 = t0->get_input(n);
                        Node *i1 = t1->get_input(n);
                        Node *i2 = t2->get_input(n);
                        
                        if (t0->get_input_name(n) != "D"){
                            if ((i0==i1) || (i1==i2) || (i2==i0)){
                                if(!(i0->is_constant() || i1->is_constant() || i2->is_constant())){
                                    printf("\rWARNING: shared input %s (net: %s) for: %s, %s, %s\n",t0->get_input_name(n).c_str(),t0->get_input(n)->get_name(),t0->get_name(),t1->get_name(),t2->get_name());
                                    count = 50; //refresh status
                                }
                            }
                        }
                    }
                    
                    node->set_is_processed(1);
                    t0->set_is_processed(1);
                    t1->set_is_processed(1);
                    t2->set_is_processed(1);
                }
            }
        }
    }
    
    
    printf("\r>done.\n");
}



