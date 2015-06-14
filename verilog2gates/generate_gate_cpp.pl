#!/usr/bin/perl
(@ARGV == 1) || die("usage: $0 input.v\n\n");
my $filename = $ARGV[0];

#parse file
open(IN, "<$filename") || die("cant open '$filename'\n");
my %io_ports;
my %resolve;
my $processing = 1;
my $is_ff = 0;
my $is_buffer = 0;
my $gatename;
while ((my $line = <IN>) && $processing){
	#process line
	if ($line =~ /\s*(in|out)put\s+([^\s;]+)\s*;/gi){
		#add in/out to portlist
		$io_ports{uc($2)} = uc($1);
	}elsif ($line =~ /=>/){
		printf "parsing done.\n";
		$processing = 0;
	}elsif ($line =~ /\s*([^\s]+)\s+([^\(]+)?\s*\(\s*([^,\s]+)\s*,\s*([^\)]+)\s*\)/gi){
		print "$1 - $2 - $3 - $4\n";
		my $tmp = $2;
		if ($1 =~ /module/){
			#this is the name of the module:
			$gatename = $tmp;
			$gatename =~ s/(.*)\d\s*/$1/i; #remove drive strength
			print "gatename $gatename.\n";
		}else{
			my $logic = uc($4);
			my $func  = uc($1);
			my $result= uc($3) ;
			#replace arguments like A1 with [A1]:
			$logic =~ s/([\w\d]+)/\[$1\]/gi;
			$resolve{$result} = resolve_logic($func, $logic);
			print "$result = " .resolve_logic($func, $logic)."\n";
		}
	}
}

#resolve outputs

my $inportmap;
my $inportsel = "    switch(i){\n";
my $indefines = "//define some helpers, makes it easier to write function\n";
my $indefine2 = "";
my $indefine3 = "//define some helpers, makes it easier to write function\n";


my $incount = 0;
my $func = "";
foreach $key (keys %io_ports){
	my $name = $key;
	my $dir  = $io_ports{$key};
	if ($dir eq "IN"){
		if ((uc($name) ne "SD") && (uc($name) ne "SC")){
			$inportmap .= '    port_map_vector.push_back("'.$name.'"); //'.$incount."\n";
			$inportsel .= '        case('.$incount.'): return("'.$name.'");'."\n";
			$indefines .= '    #define '.$name.' input_nodes['.$incount.']->get_value(run)'."\n";
			$indefine2 .= '    #define IN_'.$name.' input_nodes['.$incount.']'."\n";
			$indefine3 .= '    std::string formula_'.$name.' = input_nodes['.$incount.']->get_formula();'."\n";
			$incount++;
		}
	}
}
$indefines .= "    #define SC 0\n";
$indefines .= "    #define SD D\n";
$indefine2 .= "    return IN_D;\n";
$indefine3 .= '    std::string formula_0 = "FALSE";'."\n";
$indefine3 .= '    std::string formula_1 = "TRUE";'."\n";
$inportsel .= "        default: return \"<?>\";\n    }\n";

foreach $key (keys %io_ports){
	my $name = $key;
	my $dir  = $io_ports{$key};
	if ($dir eq "OUT"){
		my $fnamebase = lc("NODE_ATC18_".$gatename."_".$name);
		my $indefines_ = $indefines . "    #define ".uc($name)."  value\n";
		if ($name eq "QN"){
			$indefines_ .= "    #define Q !(QN)\n";
		}
		
		print "\n\n================ building $fnamebase  ==============\n";
		my $func2 = "";
		if (($name eq "QN") && (exists $io_ports{"Q"})){
			$func2 = "!(".resolve_function("Q",0).")";
		}else{
			$func2 = resolve_function($name,0);
		}
		$func = $func2;

		$func2 =~ s/([\w\d]+)/" + formula_$1 + "/gi;
		print_cpp($fnamebase .".cpp", $gatename, $name, $incount, $inportmap, $indefines_, $indefine2, $indefine3, $func, $func2);
		print_h  ($fnamebase .".h",   $gatename, $name, $incount, $inportmap, $indefines_, $indefine2, $indefine3, $func, $func2);
	}
}


exit 0;


############################
sub resolve_function()
{
	my $name = shift;
	my $rek = shift;
	my $res;
	print "resolve_function($name) called\n";

	if ($io_ports{$name} eq "IN"){
		#resolve inputs to inputs
		$res = $name;
	}elsif (($rek != 0) && ($io_ports{$name} eq "OUT")){
		#resolve inputs to inputs
		$res = $name;
	}elsif(!exists $resolve{$name}){
		#abort if unknown 
		die("do not know how to resolve '$name'\n");
	}else{
		#else: split & process
		$res = $resolve{$name};

		$res =~ s/\[0\]/0/gi;
		$res =~ s/\[1\]/1/gi;
		
		print "0-- $res --\n";
		while ($res =~ /\[([\d\w_]+)\]/){
			my $target = $1;
			my $rep = resolve_function($target, $rek+1);
			$res =~ s/\[$target\]/$rep/gi;
		}
		print "1-- $res --\n\n";
		
	}	
	print "resolve_function($name) returned $res\n";
	return $res;
}


sub resolve_logic()
{
	my $function = shift;
	my $operands = shift;
	$operands =~ s/\s*//gi;

	print "\nresolve_logic($function, '$operands') called\n";

	if($function =~ /^not$/i){
		$operands = "!".$operands;
	}elsif($function =~ /^notif0$/i){
		my @ops = split(",",$operands);
		$operands = $ops[1]."?".$ops[0].":0"; #this should be :Z
	}elsif ($function =~ /^n(.*)$/i){
		#resolve not
		return "!" . resolve_logic($1, $operands) . "";
	}elsif($function =~ /^buf$/i){
		$is_buffer = 1;
		$operands = $operands;
	}elsif($function =~ /^bufif0$/i){
		my @ops = split(",",$operands);
		$operands = $ops[1]."?".$ops[0].":0"; #this should be :Z
	}elsif($function =~ /^UDP_MUX41$/i){
		#UUDP_MUX41 (z, s0, s1, i0, i1, i2, i3);
		#               0   1    2  3   4    5 
		my @ops = split(",",$operands);
		$operands =  $ops[0]."?(".
			#s0 = 1 -> 1-i3 / 0-i1
			$ops[1]."?".$ops[5].":".$ops[3]."):(".
			#s0 = 0 -> 1-i2 / 0-i0
			$ops[1]."?".$ops[4].":".$ops[2].")";
	}elsif($function =~ /^UDP_MUX21$/i){
		#UDP_MUX21 (z, s, i0, i1);
		#              0  1   2 
		my @ops = split(",",$operands);
		$operands = $ops[0]."?".$ops[2].":".$ops[1];
	################ FFS ###########################
	}elsif($function =~ /^udp_dff$/i){
		#UDP_DFF ( q, muxed_d, cp,   1, cdn, notifier );
		#             DATA     clk  sdn cdn notify
		#              0        1    2   3     4
		my @ops = split(",",$operands);
		$operands = $ops[2]."?(".$ops[3]."?".$ops[0].":0):1";
		$is_ff = 1;
	}elsif($function =~ /^UDP_DFF_RS0_Q$/i){
		#UDP_DFF_RS0_Q  (q,  d, cp, sdn, cdn, notifier);
		#                    0  1   2     3      4
		my @ops = split(",",$operands);
		$operands = $ops[2]."?(".$ops[3]."?".$ops[0].":0):1";
		$is_ff = 1;		
	}elsif($function =~ /^UDP_DFF_RS0_QN$/i){
		#UDP_DFF_RS0_QN  (q,  d, cp, sdn, cdn, notifier);
		#                    0  1   2     3      4
		my @ops = split(",",$operands);
		$operands = "!(".$ops[2]."?(".$ops[3]."?".$ops[0].":0):1)";
		$is_ff = 1;
	###################### LATCHES ################
	}elsif($function =~ /^udp_latch$/i){
		#UDP_LATCH_  (q,  d, e, sdn, cdn, notifier);
		#                 0  1   2    3     4    //e is handled like clk here ... fixme: add support for real latch
		my @ops = split(",",$operands);
		$operands = $ops[2]."?(".$ops[3]."?".$ops[0].":0):1";
		$is_ff = 1;
	}elsif($function =~ /^udp_latch_rs1_q$/i){
		#UDP_LATCH_RS1_Q  (q,  d, e, sdn, cdn, notifier);
		#                      0  1   2    3     4    //e is handled like clk here ... fixme: add support for real latch
		my @ops = split(",",$operands);
		$operands = $ops[2]."?(".$ops[3]."?".$ops[0].":0):1";
		$is_ff = 1;
	}elsif($function =~ /^udp_latch_rs1_qn$/i){
		#UDP_LATCH_RS1_Qn  (q,  d, e, sdn, cdn, notifier);
		#                      0  1   2    3     4    //e is handled like clk here ... fixme: add support for real latch
		my @ops = split(",",$operands);
		$operands = "!(".$ops[2]."?(".$ops[3]."?".$ops[0].":0):1)";
		$is_ff = 1;
	##############
	}elsif($function =~ /^or$/i){
		$operands =~ s/,/ || /gi;
	}elsif($function =~ /^and$/i){
		$operands =~ s/,/ && /gi;
	}elsif($function =~ /^xor$/i){
		$operands =~ s/,/ != /gi;
	}elsif($function =~ /^xnor$/i){
		$operands =~ s/,/ == /gi;
	}else{
		die("unknown func $function\n");
	}
	return "(".$operands.")";
}



sub print_cpp()
{
my $filename = shift;
my $gate = shift;
my $outputname = shift;
my $incount = shift;
my $inportmap = shift;
my $indefines = shift;
my $indefine2 = shift;
my $indefine3 = shift;
my $func = shift;
my $func2 = shift;

my $gatename = uc($gate) . "_" . $outputname;
my $classname = "Node_ATC18_".$gatename;

open(OUT, ">".lc($filename)) or die ("cant write to $filename");

print OUT '/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by '.$1.' on '.localtime().'

#include "'.lc($classname).'.h"

//register to node factory
std::string '.$classname.'::node_factory_dummy = Node_factory<Node>::register_create_function("ATC18_'.uc($gatename).'",'.$classname.'::create_function);
std::string '.$classname.'::node_factory_dummy2 = Node_factory<Node>::register_output("ATC18_'.uc($gate).'","'.$outputname.'");

'.$classname.'::'.$classname.'(const char *name) : Node(name, '.$incount.'){
'.$inportmap.'
}

'.$classname.'::~'.$classname.'()
{
}

std::string '.$classname.'::get_input_name(int i)
{
'.$inportsel.'
}
';
if ($is_ff){
print OUT '
//NOTE: evaluate is different for FF elements!
//      -> for an evaluate call we actually calculate the values using the inputs
//         while get_value() just returns a value set before
//         we use this for example when having "loops" from the output FF to the 
//         input of this FF. (think about it ;) )
int '.$classname.'::evaluate()
{
'.$indefines.'

    int run = last_eval++;
    last_run_value = '.$func.';
    return last_run_value;
}

Node *'.$classname.'::get_ff_input_node()
{
'.$indefine2.'
}

std::string '.$classname.'::get_formula()
{
	return std::string(get_name());
}
';

if ($outputname eq "QN"){
print OUT '
int '.$classname.'::get_value(int run)
{
    return !value;
}
';
}

}else{
print OUT '
int '.$classname.'::get_value(int run)
{
'.$indefines.'

    if (last_run != run){
        //need to recalc
        last_run_value = '.$func.';
        last_run = run;
    }
    return last_run_value;
}

std::string '.$classname.'::get_formula()
{
'.$indefine3.'
    if (is_safe){
        return "SAFE";
    }else{
        return std::string("'.$func2.'");
    }
}
'
}

}


sub print_h()
{
my $filename = shift;
my $gate = shift;
my $outputname = shift;
my $incount = shift;
my $inportmap = shift;
my $indefines = shift;
my $indefine2 = shift;
my $indefine3 = shift;
my $func = shift;
my $func2 = shift;

my $gatename = uc($gate) . "_" . $outputname;
my $classname = "Node_ATC18_".$gatename;

open(OUT, ">".lc($filename)) or die ("cant write to $filename");

print OUT '/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 ***************************************************************************/
 ///this file was automatically created by '.$1.' on '.localtime().'

#ifndef '.uc($classname).'_H
#define '.uc($classname).'_H
#include "node.h"

class '.$classname.' : public Node
{
public:
    '.$classname.'(const char *name = "?");
    ~'.$classname.'();

    static Node* create_function(const char *name){ return new '.$classname.'(name); }
    static std::string node_factory_dummy;
    static std::string node_factory_dummy2;

    std::string get_input_name(int i);
    std::string get_formula();
';
if ($is_ff){
print OUT '    int is_ff(){return 1;};
    Node *get_ff_input_node();

protected:
        int evaluate();
';
if ($outputname eq "QN"){
print OUT "    int get_value(int run = 0);";
}
}else{
print OUT "    int get_value(int run = 0);";
}

if ($is_buffer){
print OUT "    int is_buffer(){return 1;};\n";
}

print OUT '
};

#endif

';
}
