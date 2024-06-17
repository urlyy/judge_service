#pragma once
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdbool.h>
#define BUFFER_SIZE 1024

char* concat(char *str, const char *buffer){
    // 计算拼接后的字符串长度
    size_t resultLen = strlen(str) + strlen(buffer) + 1;
    // 分配足够大的空间来存储拼接后的结果
    char *result = (char *)malloc(resultLen * sizeof(char));
    // 将两个缓冲区的内容拼接到一起
    strcpy(result, str);
    strcat(result, buffer);
    return result;
}

// 去除行末空格
void clearLastWhiteSpace(char *line){
    bool endWithLineBreak = line[strlen(line)-1] == '\n';
    int endIdx = strlen(line)-1;
    if(endWithLineBreak){
        if(line[endIdx-1] == ' '){
            line[endIdx-1]='\n';
            line[endIdx]='\0';
        }
    }else{
         if(line[endIdx] == ' '){
            line[endIdx]='\0';
         }
    } 
}

void clearLastLineBreak(char *str){
    int lastIdx = strlen(str)-1;
    if(str[lastIdx] == '\n'){
        str[lastIdx]='\0';
    }
}
  
bool readWithoutWhiteSpace(char *filename,char **value) {
    FILE *file;  
    char buffer[BUFFER_SIZE];  
    // 打开文件  
    file = fopen(filename, "r");  
    if (file == NULL) {  
        perror("Error opening file");  
        return false;  
    }  
    // 逐行读取文件  
    char *tmp="";
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {  
        // // 在这里处理每一行，例如打印它
        clearLastWhiteSpace(buffer);
        tmp = concat(tmp,buffer);
    }  
    clearLastLineBreak(tmp);
    *value = tmp;
    // 关闭文件  
    fclose(file);
    return true;  
}

int readFile(const char *filename, char **content) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("文件 %s 不存在\n", filename);
        return 0;
    }
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    // 分配内存保存文件内容
    *content = (char *)malloc(size + 1); // +1 用于存放字符串结尾的 '\0'
    if (*content == NULL) {
        fclose(file);
        return 0;
    }
    // 读取文件内容到字符串
    size_t result = fread(*content, 1, size, file);
    if (result != size) {
        free(*content);
        fclose(file);
        return 0;
    }
    (*content)[size] = '\0'; // 添加字符串结尾的 '\0'
    fclose(file);
    return 1;
}


bool contentEqual(char *filea,char *fileb){
    char *contentA,*contentB;
    bool resa = readWithoutWhiteSpace(filea,&contentA);
    bool resb = readWithoutWhiteSpace(fileb,&contentB);
    if(!resa || !resb){
        return false;
    }
    int result = strcmp(contentA, contentB);
    if (result == 0) {
        return true;
    }else{
        return false;
    }
}



bool judgeSpecial(const char *specialFile,const char *outputFile,const char *specialOutFile,const int timeLimit){
    char cmd[60];
    sprintf(cmd,"python %s %s %d %s",specialFile,outputFile,timeLimit,specialOutFile);
    printf("%s\n",cmd);
    int result = system(cmd);
    if (result == -1) {  
        // system()调用失败  
        // perror("system() call failed");  
        return false;  
    }  
    // 根据Python脚本的返回值，可以检查是否成功执行  
    if (result == 0) {  
        printf("Python script executed successfully.\n");  
        // 打开文件  
        FILE *file = fopen(specialOutFile, "r");  
        if (file == NULL) {  
            perror("Error opening file");  
            return false;  
        }  
        char buffer[10]; // 假设我们要读取10个字节
        // 读取一行  
        if (fgets(buffer, sizeof(buffer), file) == NULL) {  
            return false;
        } 
        fclose(file);  
        return strcmp(buffer,"True")==0;
    } else {  
        return false;
    }  
}