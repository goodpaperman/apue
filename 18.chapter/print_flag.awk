#! /bin/awk -f
# usage: print_flag.awk -v FUNC_NAME=xxx -v MACRO_FILE=xxx
# i.e.: print_flag.awk -v FUNC_NAME=output -v MACRO_FILE=oflag.sym
BEGIN {
printf("#include \"../apue.h\"\n")
printf("#include <termios.h>\n")
printf("\n")
printf("void print_%s_flag (tcflag_t flag)\n", FUNC_NAME)
printf("{\n")
printf("    printf (\"%s flag 0x%%08x\\n\", flag); \n", FUNC_NAME)
FS=":"
while (getline < MACRO_FILE > 0) {
printf("#ifdef %s\n", $1)
printf("    if (flag & %s)\n", $1)
printf("        printf (\"    %s\\n\"); \n", $1)
printf("//    else\n")
printf("//        printf (\"    %s not set\\n\"); \n", $1)
printf("#else\n")
printf("//    printf (\"    %s not defined\\n\"); \n", $1)
printf("#endif\n")
}
close (MACRO_FILE)
exit
}
END {
printf("}")
}
