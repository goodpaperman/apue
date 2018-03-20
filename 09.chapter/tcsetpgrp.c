#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

static void judge(void){
        pid_t pid;
        pid = tcgetpgrp(STDIN_FILENO);
        if(pid == -1){
                perror("tcgetpgrp");
                return;
        }else if(pid == getpgrp()){
                printf("foreground\n");
        }else{
                printf("background\n");
        }
}

int main(void){
        signal(SIGTTOU,SIG_IGN);
        printf("tcgetsid:%d,pgrp=%d,sid=%d\n",tcgetsid(STDIN_FILENO),getpgrp(),getsid(getpid()));
        pid_t spid = tcgetsid(STDIN_FILENO);  
        judge();
        int result;
        result = tcsetpgrp(STDIN_FILENO,getpgrp());
        if(result == -1){
                perror("tcsetpgrp");
                return -1;
        }
        judge();
        result = tcsetpgrp(STDIN_FILENO,spid);  
        return 0;
}
