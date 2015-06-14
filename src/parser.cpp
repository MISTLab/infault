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
#include "parser.h"

Parser::Parser()
{
}

Parser::Parser(const char *filename)
{
}


Parser::~Parser()
{
}


//for demo/test reasons this Parser returns 
//a static constructed Graph
Graph *Parser::get_graph()
{
    Graph *g = new Graph();
    
    //build test tree with triplication
    //function OUT = (A & B) | (!A & C) | ( C & D & E & F & G & H & I & J)
    Node *inA = new Node_INPUT("input A");
    Node *inB = new Node_INPUT("input B");
    Node *inC = new Node_INPUT("input C");
    
    g->add_node(inA);
    g->add_node(inB);
    g->add_node(inC);
    
    Node *inD = new Node_INPUT("input D");
    Node *inE = new Node_INPUT("input E");
    Node *inF = new Node_INPUT("input F");
    Node *inG = new Node_INPUT("input G");
    Node *inH = new Node_INPUT("input H");
    Node *inI = new Node_INPUT("input I");
    Node *inJ = new Node_INPUT("input J");
    
    g->add_node(inD);
    g->add_node(inE);
    g->add_node(inF);
    g->add_node(inG);
    g->add_node(inH);
    g->add_node(inI);
    g->add_node(inJ);
    
    Node *D0 = new Node_UNISIM_FF("D0");
    Node *D1 = new Node_UNISIM_FF("D1");
    Node *D2 = new Node_UNISIM_FF("D2");
    Node *E0 = new Node_UNISIM_FF("E0");
    Node *E1 = new Node_UNISIM_FF("E1");
    Node *E2 = new Node_UNISIM_FF("E2");
    Node *F0 = new Node_UNISIM_FF("F0");
    Node *F1 = new Node_UNISIM_FF("F1");
    Node *F2 = new Node_UNISIM_FF("F2");
    Node *G0 = new Node_UNISIM_FF("G0");
    Node *G1 = new Node_UNISIM_FF("G1");
    Node *G2 = new Node_UNISIM_FF("G2");
    Node *H0 = new Node_UNISIM_FF("H0");
    Node *H1 = new Node_UNISIM_FF("H1");
    Node *H2 = new Node_UNISIM_FF("H2");
    Node *I0 = new Node_UNISIM_FF("I0");
    Node *I1 = new Node_UNISIM_FF("I1");
    Node *I2 = new Node_UNISIM_FF("I2");
    Node *J0 = new Node_UNISIM_FF("J0");
    Node *J1 = new Node_UNISIM_FF("J1");
    Node *J2 = new Node_UNISIM_FF("J2");
    
    
    D0->set_input(inD, 0);
    D1->set_input(inD, 0);
    D2->set_input(inD, 0);
    E0->set_input(inE, 0);
    E1->set_input(inE, 0);
    E2->set_input(inE, 0);
    F0->set_input(inF, 0);
    F1->set_input(inF, 0);
    F2->set_input(inF, 0);
    G0->set_input(inG, 0);
    G1->set_input(inG, 0);
    G2->set_input(inG, 0);
    H0->set_input(inH, 0);
    H1->set_input(inH, 0);
    H2->set_input(inH, 0);
    I0->set_input(inI, 0);
    I1->set_input(inI, 0);
    I2->set_input(inI, 0);
    J0->set_input(inJ, 0);
    J1->set_input(inJ, 0);
    J2->set_input(inJ, 0);
    
    
    //gnd node
    Node *GND0 = new Node_UNISIM_GND("GND0");
    Node *GND1 = new Node_UNISIM_GND("GND1");
    Node *GND2 = new Node_UNISIM_GND("GND2");
    Node *GND3 = new Node_UNISIM_GND("GND3");
    Node *GND4 = new Node_UNISIM_GND("GND4");
    Node *GND5 = new Node_UNISIM_GND("GND5");
    Node *GND6 = new Node_UNISIM_GND("GND6");
    Node *GND7 = new Node_UNISIM_GND("GND7");
    Node *GND8 = new Node_UNISIM_GND("GND8");
    Node *GND9 = new Node_UNISIM_GND("GND9");
    
    //input nodes
    Node *A0 = new Node_UNISIM_FF("A0");
    Node *A1 = new Node_UNISIM_FF("A1");
    Node *A2 = new Node_UNISIM_FF("A2");
    Node *B0 = new Node_UNISIM_FF("B0");
    Node *B1 = new Node_UNISIM_FF("B1");
    Node *B2 = new Node_UNISIM_FF("B2");
    Node *C0 = new Node_UNISIM_FF("C0");
    Node *C1 = new Node_UNISIM_FF("C1");
    Node *C2 = new Node_UNISIM_FF("C2");
    
    
    A0->set_input(inA, 0);
    A1->set_input(inA, 0);
    A2->set_input(inA, 0);
    B0->set_input(inB, 0);
    B1->set_input(inB, 0);
    B2->set_input(inB, 0);
    C0->set_input(inC, 0);
    C1->set_input(inC, 0);
    C2->set_input(inC, 0);
    
    //add nodes in random order
    g->add_node(A0);
    
    g->add_node(B0);
    g->add_node(B1);
    
    g->add_node(A1);
    g->add_node(A2);
    
    g->add_node(B2);
    
    g->add_node(C0);
    g->add_node(C1);
    g->add_node(C2);

    
    
    //voter for A
    Node *VOTE_A = new Node_UNISIM_LUT4("VOTE_A");
    VOTE_A->set_init(0xE8E8); //voter
    VOTE_A->set_input(A0,  0);
    VOTE_A->set_input(A1,  1);
    VOTE_A->set_input(A2,  2);
    VOTE_A->set_input(GND0, 3);
    g->add_node(VOTE_A);
    
    //voter for B
    Node *VOTE_B = new Node_UNISIM_LUT4("VOTE_B");
    VOTE_B->set_init(0xE8E8); //voter
    VOTE_B->set_input(B0,  0);
    VOTE_B->set_input(B1,  1);
    VOTE_B->set_input(B2,  2);
    VOTE_B->set_input(GND1, 3);
    g->add_node(VOTE_B);
    
    //voter for C
    Node *VOTE_C = new Node_UNISIM_LUT4("VOTE_C");
    VOTE_C->set_init(0xE8E8); //voter
    VOTE_C->set_input(C0,  0);
    VOTE_C->set_input(C1,  1);
    VOTE_C->set_input(C2,  2);
    VOTE_C->set_input(GND2, 3);
    g->add_node(VOTE_C);
    
    //voter for D
    Node *VOTE_D = new Node_UNISIM_LUT4("VOTE_D");
    VOTE_D->set_init(0xE8E8); //voter
    VOTE_D->set_input(D0,  0);
    VOTE_D->set_input(D1,  1);
    VOTE_D->set_input(D2,  2);
    VOTE_D->set_input(GND3, 3);
    g->add_node(VOTE_D);
    
    //voter for E
    Node *VOTE_E = new Node_UNISIM_LUT4("VOTE_E");
    VOTE_E->set_init(0xE8E8); //voter
    VOTE_E->set_input(E0,  0);
    VOTE_E->set_input(E1,  1);
    VOTE_E->set_input(E2,  2);
    VOTE_E->set_input(GND4, 3);
    g->add_node(VOTE_E);
    
    //voter for F
    Node *VOTE_F = new Node_UNISIM_LUT4("VOTE_F");
    VOTE_F->set_init(0xE8E8); //voter
    VOTE_F->set_input(F0,  0);
    VOTE_F->set_input(F1,  1);
    VOTE_F->set_input(F2,  2);
    VOTE_F->set_input(GND5, 3);
    g->add_node(VOTE_F);
    
    //voter for G
    Node *VOTE_G = new Node_UNISIM_LUT4("VOTE_G");
    VOTE_G->set_init(0xE8E8); //voter
    VOTE_G->set_input(G0,  0);
    VOTE_G->set_input(G1,  1);
    VOTE_G->set_input(G2,  2);
    VOTE_G->set_input(GND6, 3);
    g->add_node(VOTE_G);
    
    //voter for H
    Node *VOTE_H = new Node_UNISIM_LUT4("VOTE_H");
    VOTE_H->set_init(0xE8E8); //voter
    VOTE_H->set_input(H0,  0);
    VOTE_H->set_input(H1,  1);
    VOTE_H->set_input(H2,  2);
    VOTE_H->set_input(GND7, 3);
    g->add_node(VOTE_H);
    
    //voter for I
    Node *VOTE_I = new Node_UNISIM_LUT4("VOTE_I");
    VOTE_I->set_init(0xE8E4); //voter
    VOTE_I->set_input(I0,  0);
    VOTE_I->set_input(I1,  1);
    VOTE_I->set_input(I2,  2);
    VOTE_I->set_input(GND8, 3);
    g->add_node(VOTE_I);
    
    //voter for J
    Node *VOTE_J = new Node_UNISIM_LUT4("VOTE_J");
    VOTE_J->set_init(0xE8E8); //voter
    VOTE_J->set_input(J0,  0);
    VOTE_J->set_input(J1,  1);
    VOTE_J->set_input(J2,  2);
    VOTE_J->set_input(GND9, 3);
    g->add_node(VOTE_J);
    
    //inverter for A
    Node *NOT_A = new Node_UNISIM_NOT("NOT");
    NOT_A->set_input(VOTE_A, 0);
    g->add_node(NOT_A);
    
    //first AND gate for (A & B)
    Node *AND_AB = new Node_UNISIM_AND2("AND_AB");
    AND_AB->set_input(VOTE_A, 0);
    AND_AB->set_input(VOTE_B, 1);
    g->add_node(AND_AB);
    
    //second AND gate for (!A & C)
    Node *AND_nAC = new Node_UNISIM_AND2("AND_nAC");
    AND_nAC->set_input(NOT_A,  0);
    AND_nAC->set_input(VOTE_C, 1);
    g->add_node(AND_nAC);
    
    //C & D & E & F & G & H Tree
    Node *AND_CD= new Node_UNISIM_AND2("AND_CD");
    Node *AND_EF= new Node_UNISIM_AND2("AND_EF");
    Node *AND_GH= new Node_UNISIM_AND2("AND_GH");
    Node *AND_IJ= new Node_UNISIM_AND2("AND_IJ");
    Node *AND_CDEF = new Node_UNISIM_AND2("AND_CDEF");
    Node *AND_GHIJ = new Node_UNISIM_AND2("AND_GHIJ");
    Node *AND_CDEFGHIJ = new Node_UNISIM_AND2("AND_CDEFGHIJ");
    
    
    
    AND_CDEFGHIJ->set_input(AND_CDEF,  0);
    AND_CDEFGHIJ->set_input(AND_GHIJ,  1);
    AND_CDEF->set_input(AND_CD,  0);
    AND_CDEF->set_input(AND_EF,  1);
    AND_GHIJ->set_input(AND_GH,  0);
    AND_GHIJ->set_input(AND_IJ,  1);
    AND_CD->set_input(VOTE_C,  0);
    AND_CD->set_input(VOTE_D,  1);
    AND_EF->set_input(VOTE_E,  0);
    AND_EF->set_input(VOTE_F,  1);
    AND_GH->set_input(VOTE_G,  0);
    AND_GH->set_input(VOTE_H,  1);
    AND_IJ->set_input(VOTE_I,  0);
    AND_IJ->set_input(VOTE_J,  1);
    
    g->add_node(AND_CD);
    g->add_node(AND_EF);
    g->add_node(AND_GH);
    g->add_node(AND_IJ);
    
    g->add_node(AND_CDEF);
    g->add_node(AND_GHIJ);
    g->add_node(AND_CDEFGHIJ);
    
    //final result
    Node *OR1 = new Node_UNISIM_OR2("OR_1");
    OR1->set_input(AND_AB,  0);
    OR1->set_input(AND_nAC, 1);
    g->add_node(OR1);
    
    Node *OR2 = new Node_UNISIM_OR2("OR_1");
    OR2->set_input(OR1,  0);
    OR2->set_input(AND_CDEFGHIJ, 1);
    g->add_node(OR2);
    
    Node *OUT_OR = new Node_UNISIM_OR2("OUT_OR");
    OUT_OR->set_input(OR1,  0);
    OUT_OR->set_input(OR2, 1);
    g->add_node(OUT_OR);
    
    Node *OUT = new Node_UNISIM_FF("OUT");
    OUT->set_input(OUT_OR,  0);
    g->add_node(OUT);
    
    //do consistency check (rucursive from output node)
    if (!OUT->check_consistency()){
        printf("consistency check failed... returning NULL!\n");;
        return NULL;
    }
    
    return g;
}


