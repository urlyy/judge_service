package util

import (
	"context"
	"fmt"

	"github.com/docker/docker/api/types/container"
	"github.com/docker/docker/api/types/image"
	"github.com/docker/docker/api/types/mount"
	"github.com/docker/docker/client"
)
func getClient() *client.Client{
	cli, err := client.NewClientWithOpts(client.WithHost("tcp://localhost:2375"), client.WithVersion("1.44"))
	if err != nil {
		panic(err)
	}
	return cli
}

func getImages(){
	cli := getClient()
	images,err := cli.ImageList(context.Background(), image.ListOptions{All: true})
	if err != nil {
		panic(err)
	}
	for _,img :=range images{
		fmt.Println(img.RepoTags)
	}
}

type JudgeParams struct{
	Special int
	TimeLimit uint64
	MemoryLimit uint64
	CaseNum int
}


func Run(params *JudgeParams,compiler string,dataDir string){
	cli := getClient()
	ctx :=context.Background()
	env := []string{
		fmt.Sprintf("special=%d", params.Special),
		fmt.Sprintf("timelimit=%d", params.TimeLimit),
		fmt.Sprintf("memorylimit=%d", params.MemoryLimit),
		fmt.Sprintf("casenum=%d", params.CaseNum),
	}
	// 准备配置,单位是毫秒->秒，再两倍
	timeout := int(params.TimeLimit)/500
	config := &container.Config{
		Image: fmt.Sprintf("judger:%s",compiler), 
		Env:   env,
		StopTimeout: &timeout,
	}
	// 准备 HostConfig，设置挂载点
	hostConfig := &container.HostConfig{
		Mounts: []mount.Mount{
			{
				Type:   mount.TypeBind,
				Source: dataDir,
				Target: "/app/data",
			},
		},
	}

	// 创建容器
	cont, err := cli.ContainerCreate(ctx, config, hostConfig, nil, nil, "")
	if err != nil {
		panic(err)
	}

	// 启动容器
	if err := cli.ContainerStart(ctx, cont.ID, container.StartOptions{}); err != nil {
		panic(err)
	}

	fmt.Printf("Container %s started.\n", cont.ID)
    // // 创建容器
	statusCh, errCh := cli.ContainerWait(ctx, cont.ID, container.WaitConditionNotRunning)
	select {
	case err := <-errCh:
	    if err != nil {
			cli.ContainerRemove(ctx,cont.ID,container.RemoveOptions{
				// Force: true,
			})
	        fmt.Println(err)
	    }
	case status := <-statusCh:
	    fmt.Println("Container exited with status:", status.StatusCode)
	}
	cli.ContainerRemove(ctx,cont.ID,container.RemoveOptions{
		// Force: true,
	})
}

