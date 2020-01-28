#! /bin/awk -f
# usage: print_speed.awk -v MACRO_FILE=xxx
# i.e.: print_speed.awk -v MACRO_FILE=speed.sym
BEGIN {
printf("#include \"../apue.h\" \n")
printf("#include <termios.h> \n")
printf("\n")
printf("void print_speed (char const* prompt, speed_t s)\n")
printf("{\n")
printf("    printf (\"%%s %%d\\n\", prompt, s); \n")
printf("    switch (s)\n")
printf("    {\n")
FS=":"
while (getline < MACRO_FILE > 0) {
printf("#ifdef %s\n", $1)
printf("    case %s:\n", $1)
printf("        printf (\"%%s bits/s    %%.2f kb/s\\n\", \"%s\"+1, atoi(\"%s\"+1)/8.0/1000); \n", $1, $1)
printf("        break; \n")
printf("#endif\n")
}
close (MACRO_FILE)
exit
}
END {
printf("    default:\n")
printf("        printf (\"unknown enum %%d\\n\", s); \n")
printf("        break; \n")
printf("    }\n")
printf("}\n")
}
