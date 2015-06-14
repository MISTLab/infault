#!/usr/bin/perl
#$formula = "(!((!!((!!(A || B)) && !((!C) && !(A && B)))) && !((!D) && !((!E) && F)) && !((!F) && E)))";
#$formula = "!((!C) && !(A && B))";
$formula = "(SAFE && SAFE && SAFE && !((DUT/pci_core/r_reg_0_2200__Q && DUT/pci_core/r_reg3_2_2200__Q) || (DUT/pci_core/r_reg2_1_2246__Q && DUT/pci_core/r_reg3_2_2246__Q)))"
#0) dissolve all !!a to a
#$formula =~ s/!!//gi;
print "$formula\n";

#1) remove all => / a?b:c / != / == tags:
#todo..

#2) shift negate inwards:
#while ($formula =~ s/!\(([\w\d|&\(\)!]+) && ([\w\d|&\(\)!]+)\)/\(!$1 || !$2\)/i){
#	print "$formula\n";
#}
#while ($formula =~ s/!!//gi){
#	print "$formula\n";
#}
#while ($formula =~ s/!\(!([\w\d]+)\)/$1/i){
	#print "$formula\n";
#}
$resolved = 1;
while ($resolved){
	$resolved = 0;
	while ($formula =~ s/!\(([\w\d|&\(\)!\s]+) \|\| ([\w\d|&\(\)!\s]+)\)/\(!$1 && !$2\)/i){
		print "$formula\n";
		$resolved = 1;
	}

	while ($formula =~ s/!\(([\w\d|&\(\)!\s]+) && ([\w\d|&\(\)!\s]+)\)/\(!$1 || !$2\)/i){
		print "$formula\n";
		$resolved = 1;
	}
}
while ($formula =~ s/!\(([\w\d|&\(\)!\s]+) && ([\w\d|&\(\)!\s]+)\)/\(!$1 || !$2\)/g){
	print "$formula\n";
}
while ($formula =~ s/!!//gi){
	print "$formula\n";
}

while ($formula =~ s/\((!*[\w\d]+)\)/$1/i){
	print "$formula\n";
}


#while ($formula =~ s/([\w\d|&\(\)!\s]+) && \(([\w\d|&\(\)!\s]+) \|\| ([\w\d|&\(\)!\s]+)\)/(($1 && $2) || ($1 && $3))/g){
	#print "$formula\n";
#}
#
#while ($formula =~ s/\(([\w\d|&\(\)!\s]+) \|\| ([\w\d|&\(\)!\s]+)\) && ([\w\d|&\(\)!\s]+)/(($1 && $3) || ($2 && $3))/g){
	#print "$formula\n";
#sleep(1);
#}

