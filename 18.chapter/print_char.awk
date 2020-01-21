#! /bin/awk -f
# usage: print_char.awk -v FUNC_NAME=xxx -v MACRO_FILE=xxx
# i.e.: print_char.awk -v FUNC_NAME=control -v MACRO_FILE=cchar.sym
BEGIN {
printf("#include \"../apue.h\"\n")
printf("#include <termios.h>\n") 
printf("\n")
printf("void print_%s_char (cc_t *cc, int len)\n", FUNC_NAME)
printf("{\n")
printf("    printf (\"input %s char array size %%d\\n\", len); \n", FUNC_NAME)
FS=":"
while (getline < MACRO_FILE > 0) {
printf("#ifdef %s\n", $1)
printf("    if (%s >= 0 && %s < len)\n", $1, $1)
printf("        printf (\"    cc[%s=%%d] = %%d (%%c)\\n\", %s, cc[%s], cc[%s]); \n", $1, $1, $1, $1)
printf("    else \n")
printf("        printf (\"    %s out of index\\n\"); \n", $1)
printf("#else\n")
printf("    printf (\"    %s not defined\\n\"); \n", $1)
printf("#endif\n")
}
close (MACRO_FILE)
exit
}
END {
printf("}\n")
}
