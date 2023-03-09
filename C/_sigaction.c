#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define TEST_SIG_DEL        //测试恢复信号默认处理函数
#define TEST_SA_RESETHAND   //测试恢复信号默认处理函数
#define TEST_RECOVER_SIG    //测试信号处理后恢复信号
#define TEST_SA_MASK        //测试信号处理时，屏蔽其他信号，SIGSTOP和SIGKILL不可屏蔽

void signal_hook(int signo, siginfo_t* info, void* ct)
{
    printf("got signal %d\n", signo);
    int i;
    for (i = 0; i < 3; ++i) {
        printf("signal_hook\n");
        sleep(1);
    }

#ifdef TEST_SIG_DEL
    struct sigaction act, oact;
    memset(&act, 0, sizeof(act));
    memset(&oact, 0, sizeof(oact));

    act.sa_handler = SIG_DFL;
    sigfillset(&act.sa_mask);
    if (sigaction(signo, &act, &oact) < 0) {
        perror("restore handler for signal failed\n");
        return;
    }
#endif
#ifdef TEST_RECOVER_SIG
    kill(getpid(), signo);
#endif    
}

int signal_init()
{
    struct sigaction act, oact;
    memset(&act, 0, sizeof(act));
    memset(&oact, 0, sizeof(oact));

    act.sa_sigaction = signal_hook;
    act.sa_flags = SA_SIGINFO;
#ifdef TEST_SA_RESETHAND
    act.sa_flags = act.sa_flags | SA_RESETHAND;
#endif
#ifdef TEST_SA_MASK
    sigfillset(&act.sa_mask);
#endif
    if (0 != sigaction(SIGINT, &act, &oact)) {
        perror("sigaction");
        return -1;
    }
    return 0;
}

int main(void)
{
    signal_init();
    for (;;) {
        printf("main...\n");
        sleep(1);
    }
}