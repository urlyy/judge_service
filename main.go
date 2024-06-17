package main

import (
	"encoding/json"
	"fmt"
	pb "judge/proto/judge"
	"log"
	"net"

	"judge/util"

	"golang.org/x/net/context"
	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"
)

type server struct{
	pb.UnimplementedJudgeServer
}


func (s *server) SubmitJudge(ctx context.Context, req *pb.JudgeRequest) (*pb.JudgeReply, error) {
	dataDir,err :=util.PrepareFiles(req.SubmissionID,req.Code,req.InputList,req.ExpectList,req.Special,req.SpecialCode)
	if err!=nil{
		fmt.Println(err)
		util.RemoveAll(dataDir)
		return &pb.JudgeReply{},err
	}
	special := 0
	if req.Special{
		special=1
	}
	judgeParams := util.JudgeParams{
		Special: special,
		TimeLimit: req.TimeLimit,
		MemoryLimit: req.MemoryLimit,
		CaseNum: len(req.InputList),
	}
	util.Run(&judgeParams,req.Compiler,dataDir)
	var resJson string
	resPath := fmt.Sprintf("%s/res.json",dataDir)
	err=util.ReadFile(resPath,&resJson)
	if err!=nil{
		fmt.Println(err)
		util.RemoveAll(dataDir)
		return &pb.JudgeReply{},err
	}
	var reply pb.JudgeReply
	err = json.Unmarshal([]byte(resJson), &reply)
	if err!=nil{
		fmt.Println("报错了",err)
		util.RemoveAll(dataDir)
		return &pb.JudgeReply{},err 
	}
	util.RemoveAll(dataDir)
	return &reply,nil
}

func main() {
	lis, err := net.Listen("tcp", "0.0.0.0:8800")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	// 实例化grpc服务端
	s := grpc.NewServer()
	// 注册Greeter服务
	pb.RegisterJudgeServer(s, &server{})
	// 往grpc服务端注册反射服务
	reflection.Register(s)
	// 启动grpc服务
	if err := s.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}