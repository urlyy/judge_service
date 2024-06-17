# 使用基础镜像， Ubuntu
FROM judger:base

COPY . /app
# RUN wget https://repo.huaweicloud.com/openjdk/11.0.1/openjdk-11.0.1_linux-x64_bin.tar.gz &&

RUN mv /app/openjdk-11.0.1_linux-x64_bin.tar.gz .
RUN tar -zxvf openjdk-11.0.1_linux-x64_bin.tar.gz
RUN rm -f openjdk-11.0.1_linux-x64_bin.tar.gz

ENV JAVA_HOME=/jdk-11.0.1
ENV PATH $PATH:$JAVA_HOME/bin

VOLUME /app/data
RUN cd /app && gcc judge.c ./cjson/cJSON.c -o judge.exe

# 设置工作目录为 /app
WORKDIR /app

# 设置容器启动时的默认命令
# CMD ["bash"]
CMD ["./judge.exe"]
