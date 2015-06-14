module mysub1(clk[2] , clk[1] , clk[0], inputA , inputB, inputC, outputA, scin, sdin);
	input [2:0] clk;
	input inputA;
	input inputB;
	input inputC;
	input scin;
	input sdin;
	output outputA;

	buffd7 mybuf_inA(.i(inputA), .z(inputA_buffered));
	buffd7 mybuf_inB(.i(inputB), .z(inputB_buffered));
	buffd7 mybuf_inC(.i(inputC), .z(inputC_buffered));

	an02d2 myand_AB(.a1(inputA_buffered), .a2(inputB_buffered), .z(AandB));
	an02d2 myand_ABC(.a1(AandB), .a2(inputC_buffered), .z(AandBandC));
	sdnrq1 reg1(.cp(clk[1]), .d(AandBandC), .q(outputA),.sc(scin), .sd(sdin));
endmodule
module main(Mclk[2] , Mclk[1] , Mclk[0], MinputA , MinputB, MinputC, MoutputA);
	input [2:0] Mclk;
	input MinputA;
	input MinputB;
	input MinputC;
	output MoutputA;

	inv0d1 invA(.i(MinputA), .zn(MinputA_inv));
	inv0d1 invOUT(.i(subOUT), .zn(MoutputA));      
	mysub1 mysub1inst( .clk[2](Mclk[2]), .clk[1](Mclk[1]), .clk[0](Mclk[0]), .inputA(MinputA_inv), .inputB(MinputB),.inputC(MinputC),.outputA(subOUT),.scin(),.sdin());
endmodule
