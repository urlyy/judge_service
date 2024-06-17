# 使用基础镜像， Ubuntu
FROM judger:base

RUN apt-get install -y build-essential

COPY . /app

VOLUME /app/data

RUN cd /app && gcc judge.c ./cjson/cJSON.c -o judge.exe

# 设置工作目录为 /app
WORKDIR /app

# 设置容器启动时的默认命令
# CMD ["bash"]
CMD ["./judge.exe"]
