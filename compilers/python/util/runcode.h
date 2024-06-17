#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <pthread.h>
#include <sys/resource.h>
#include "diff.h"
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>

#define ACCECPT 0
#define COMPILE_PASS 0

#define COMPILE_ERROR 1
#define WRONG_ANSWER 2
#define RUNTIME_ERROR 3
#define TIME_LIMIT_EXCEED 4
#define MEMORY_LIMIT_EXCEED 5

// #define JUDGE_ERROR 6

//#define COMPILE_CMD "gcc -x c %s -o %s 2>&1"
// run的参数在_runExe那里

typedef struct  {
    int status;
    int timeUsed;
    int memoryUsed;
    char *log;
}Result;


void executeCommand(const char *cmd,int *retCode,char **output){
    FILE *fp;
    // 执行命令并读取输出
    fp = popen(cmd, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return;
    }
    char *tmp="";
    char buffer[128];
    // 读取命令输出内容
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        tmp = concat(tmp,buffer);
    }
    // 关闭文件指针
    int status = pclose(fp);
    // 获取命令的返回值
    if (WIFEXITED(status)) {
        *retCode = WEXITSTATUS(status);
        *output = tmp;
    }else{
        printf("执行有误\n");
    }
}

// void compile(const char* codeFile,const char *exeFile,Result *result){
//     char command[50];
//     sprintf(command, COMPILE_CMD, codeFile,exeFile);
//     // 只返回0和1
//     executeCommand(command,&(result->status),&(result->log));
// }

// ms kb
void setProcessLimit(const int timelimit, const int memory_limit) {
    struct rlimit rl;
    /* set the time_limit (second)*/
    rl.rlim_cur = timelimit / 1000;
    rl.rlim_max = rl.rlim_cur + 1;
    //限制秒
    setrlimit(RLIMIT_CPU, &rl);
    // printf("时间限制:%ld ",rl.rlim_cur);
    /* set the memory_limit (b)*/
    // 不知道这个是干吗的
    rl.rlim_cur = memory_limit * 1024;
    rl.rlim_max = rl.rlim_cur;
    // printf("内存限制%ld\n",rl.rlim_cur);
    setrlimit(RLIMIT_DATA, &rl);
     // 限制字节
    rl.rlim_cur = memory_limit * 1024;
    rl.rlim_max = rl.rlim_cur;
    setrlimit(RLIMIT_AS, &rl);
}

void _runExe(char *exeFile,long timeLimit, long memoryLimit, char *in, char *out) {
    int newstdin = open(in,O_RDWR|O_CREAT,0644);
    int newstdout = open(out,O_RDWR|O_CREAT|O_TRUNC,0644);
    if (newstdout != -1 && newstdin != -1){
        dup2(newstdout,fileno(stdout));
        dup2(newstdin,fileno(stdin));
        char *cmd[] = {"python", exeFile, NULL};
        if (execvp(cmd[0], cmd) == -1){
            printf("====== Failed to start the process! =====\n");
        }
    } else {
        printf("====== Failed to open file! =====\n");
    }
    close(newstdin);
    close(newstdout);
}



void monitor(pid_t pid, int timeLimit, int memoryLimit, Result *rest) {
    int status;
    struct rusage ru;
    // 等待进程结束
    if (wait4(pid, &status, 0, &ru) == -1)printf("wait4 failure");
    rest->timeUsed = ru.ru_utime.tv_sec * 1000
            + ru.ru_utime.tv_usec / 1000
            + ru.ru_stime.tv_sec * 1000
            + ru.ru_stime.tv_usec / 1000;
    // 另一个可能可行的方案：缺页错误就是使用内存的次数，乘页面大小就是内存占用，java可能用：`ru.ru_minflt * (sysconf(_SC_PAGESIZE) / 1024))` ;
    rest->memoryUsed = ru.ru_maxrss;
    // 程序异常中断
    if(WIFSIGNALED(status)){
        int sig = WTERMSIG(status);
        switch (WTERMSIG(status)) {
            case SIGSEGV:
                if (rest->memoryUsed > memoryLimit)
                    rest->status = MEMORY_LIMIT_EXCEED;
                else
                    rest->status = RUNTIME_ERROR;
                break;
            case SIGALRM:
            case SIGXCPU:
                rest->status = TIME_LIMIT_EXCEED;
                break;
            default:
                rest->status = RUNTIME_ERROR;
                break;
        }
    } else {
        // 注意语法错误和运行错误都会进这里
        int sig = WEXITSTATUS(status);
        if (sig==0){
            rest->status = ACCECPT;
        }else{
            rest->status = RUNTIME_ERROR;
        }
    }
}

void runExe( char* exeFile, char* inputFile, char* outputFile,int timeLimit,int memoryLimit,Result* result){
    pid_t pid = fork();
    if(pid<0){
        printf("error in fork!\n");
        result->status = WRONG_ANSWER;
        result->log = "无法创建新进程";
        return;
    }
    if(pid>0){
        monitor(pid, timeLimit, memoryLimit, result);
        // printf("{\"status\":\"%d\",\"timeUsed:\":\"%d\",\"memoryUsed:\":\"%d\"}", result->status, result->timeUsed, result->memoryUsed);
    }else{
        setProcessLimit(timeLimit,memoryLimit);
        // 这里执行运行操作
        // 不能用system()，那样拿不到运行程序的返回值，而是shell的
        _runExe(exeFile,timeLimit,memoryLimit,inputFile,outputFile);
    }
}
