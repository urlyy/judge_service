package util

import (
	"fmt"
	"os"
)

const (
	SPECIAL_TEMPLATE = `import signal  
import sys  
from contextlib import contextmanager  
  
@contextmanager  
def time_limit(seconds):  
    def signal_handler(signum, frame):  
        raise Exception()  
    signal.signal(signal.SIGALRM, signal_handler)  
    signal.alarm(seconds)  
    try:  
        yield  
    finally:  
        signal.alarm(0)
        

try:  
    with open(sys.argv[1], 'r') as file:  
        lines = file.readlines()  
    with time_limit(int(sys.argv[2])):
        res = judge(lines)  
except Exception as e:  
        res = False
with open(sys.argv[3], 'w') as f:  
    f.write(str(res))`
)

func init(){
	os.MkdirAll("data", os.ModePerm)
}

func RemoveAll(path string) error {
    // 获取文件/文件夹信息
    fileInfo, err := os.Stat(path)
    if err != nil {
        return err
    }

    if fileInfo.IsDir() {
        // 获取文件夹下的所有文件和子文件夹
        files, err := os.ReadDir(path)
        if err != nil {
            return err
        }
        // 递归删除文件夹下的所有内容
        for _, file := range files {
            err := RemoveAll(path + "/" + file.Name())
            if err != nil {
                return err
            }
        }
        // 删除空文件夹
        err = os.Remove(path)
        if err != nil {
            return err
        }
    } else {
        // 如果是文件，则直接删除
        err := os.Remove(path)
        if err != nil {
            return err
        }
    }

    return nil
}

func writeFile(path string,data []byte){
	file, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		fmt.Println("无法打开文件:", err)
		return
	}
	defer file.Close()

	// 写入文件
	if _, err := file.Write(data); err != nil {
		fmt.Println("无法写入文件:", err)
		return
	}
}

func ReadFile(path string,content *string)error{
	file, err := os.Open(path)
	if err != nil {
		fmt.Println("无法打开文件:", err)
		return err
	}
	defer file.Close()
	// 读取文件内容
	val, err := os.ReadFile(path)
	if err != nil {
		fmt.Println("读取文件时发生错误:", err)
		return err
	}
	*content = string(val)
	return  nil
}

func PrepareFiles(submissionID string,code string,input []string,expected []string,isSpecial bool,specialCode string)(string,error){
	dir, err := os.Getwd()
    if err != nil {
        fmt.Println("无法获取当前工作目录:", err)
        return "",err
    }
	trueDir := fmt.Sprintf("%s/data/%s", dir,submissionID)
	os.Mkdir(trueDir,0755)
	codePath := fmt.Sprintf("%s/code", trueDir)
	writeFile(codePath,[]byte(code))
	if isSpecial{
		specialPath := fmt.Sprintf("%s/special.py",trueDir)
		writeFile(specialPath,[]byte(specialCode+"\n"+SPECIAL_TEMPLATE))
	}else{
		for idx,exp := range expected{
			exPath :=  fmt.Sprintf("%s/%d.expect", trueDir,idx)
			writeFile(exPath,[]byte(exp))
		}
	}
	for idx,inp := range input{
		inPath :=  fmt.Sprintf("%s/%d.in", trueDir,idx)
		writeFile(inPath,[]byte(inp))
	}
	return trueDir,nil
}