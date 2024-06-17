#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "util/diff.h"
#include "util/runcode.h"
#include "cjson/cJSON.h"

#define EXE "Main"

#define OUT "data/out.out"
#define SPECIAL_OUT "data/sout.out"

#define CODE_FILE "data/code"
#define JAVA_CODE_FILE "data/Main.java"

#define INPUT_FILE_SUFFIX "in"
#define EXPECT_FILE_SUFFIX "expect"
#define SPECIAL_FILE "data/special.py"
#define RES_FILE "data/res.json"

void res2json(Result *compileResult,Result *runResults,int testCaseNum,char *lastOuput){
    // 创建 JSON 对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        fprintf(stderr, "Failed to create JSON object.\n");
        return;
    }
    //编译结果
    cJSON *compileNode = cJSON_CreateObject();
    cJSON_AddNumberToObject(compileNode, "status", compileResult->status);
    cJSON_AddStringToObject(compileNode, "log", compileResult->log);
    cJSON_AddItemToObject(root, "compile", compileNode);
    // 运行结果
    cJSON * runNodes = cJSON_CreateArray();
    for(int i=0; i<testCaseNum;i++){
        cJSON *runNode = cJSON_CreateObject();
        cJSON_AddNumberToObject(runNode, "status", runResults[i].status);
        cJSON_AddStringToObject(runNode, "log", runResults[i].log);
        cJSON_AddNumberToObject(runNode, "time", runResults[i].timeUsed);
        cJSON_AddNumberToObject(runNode, "memory", runResults[i].memoryUsed);
        cJSON_AddItemToArray(runNodes, runNode);
    }
    cJSON_AddItemToObject(root, "run", runNodes);
    //最后一次输出
    cJSON *lastOutputNode = cJSON_CreateString(lastOuput);
    cJSON_AddItemToObject(root, "lastOutput", lastOutputNode);
    // // 将 JSON 对象转换为 JSON 字符串
    char *jsonStr = cJSON_Print(root);
    if (jsonStr == NULL) {
        fprintf(stderr, "Failed to convert JSON object to string.\n");
        cJSON_Delete(root);
        return;
    }
    cJSON_Delete(root);
    // 打开文件，如果不存在则创建，准备写入  
    FILE *file = fopen(RES_FILE, "w");  
    if (file == NULL) {  
        perror("Error opening file");  
        return;  
    }  
    // 写入字符串到文件
    fputs(jsonStr, file);
    fclose(file);
    printf("%s\n",jsonStr);
    free(jsonStr);
}


int judge(const int testCaseNum,const int isSpecial,const int timeLimit,const int memoryLimit,Result *compileRes,Result *runResArr,int *passNum){
    // 注意这里多了一次重命名
    rename(CODE_FILE,JAVA_CODE_FILE);
    compile(JAVA_CODE_FILE,compileRes);
    if(compileRes->status==COMPILE_ERROR){
        return COMPILE_ERROR;
    }
    for(int i=0;i<testCaseNum;i++){
        char inputFilename[25];
        sprintf(inputFilename, "data/%d.%s", i,INPUT_FILE_SUFFIX);
        runExe(EXE,inputFilename,OUT,timeLimit,memoryLimit,&runResArr[i]);
        // // 看是否运行通过
        if(runResArr[i].status!=ACCECPT){
            return runResArr[i].status;
        } 
        // printf("是special%d\n",isSpecial);
        // //看是否答案正确
        if(isSpecial){
            int res=judgeSpecial(SPECIAL_FILE,OUT,SPECIAL_OUT,timeLimit);
            if(res){
                runResArr[i].status=ACCECPT;
                *passNum+=1;
            }else{
                runResArr[i].status=WRONG_ANSWER;
                return WRONG_ANSWER;
            }
        }else{
            char expectFilename[25];
            sprintf(expectFilename, "data/%d.%s",i,EXPECT_FILE_SUFFIX);
            if(contentEqual(OUT,expectFilename)){
                runResArr[i].status=ACCECPT;
                *passNum+=1;
            }else{
                runResArr[i].status=WRONG_ANSWER;
                return WRONG_ANSWER;
            }
        }
    }
    return ACCECPT;
}

int main(int argc,char **argv) {
    int isSpecial = atoi(getenv("special"));
    int testCaseNum = atoi(getenv("casenum"));
    int timeLimit = atoi(getenv("timelimit"));
    int memoryLimit = atoi(getenv("memorylimit"));
    Result runResArr[testCaseNum];
    Result compileRes;
    int passNum=0;
    for(int i=0;i<testCaseNum;i++){
        runResArr[i].status = -1;
        runResArr[i].log = "";
        runResArr[i].timeUsed = 0;
        runResArr[i].memoryUsed = 0;
    }
    int status = judge(testCaseNum,isSpecial,timeLimit,memoryLimit,&compileRes,runResArr,&passNum);
    char *lastOutput;
    if (readFile(OUT, &lastOutput)==0) {
        lastOutput="";
    }
    res2json(&compileRes,runResArr,testCaseNum,lastOutput);
    free(lastOutput);
    remove(EXE);
    // 特地不删
    // remove(OUT);
    // remove(SPECIAL_EXE);
    // remove(SPECIAL_OUT);
    // 把自己的exe删掉
    // remove(argv[0]);
    return 0;
}
