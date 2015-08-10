print  "const char *stringImage[] = {\n";
while(<>) {
	chop;
	print "\"" . $_ . "\",\n" ;
}
print "};\n";
print "#define LINE_CNT " . $. . "\n";

