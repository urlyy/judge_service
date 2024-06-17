# 使用基础镜像， Ubuntu
FROM ubuntu:latest


ENV DEBIAN_FRONTEND=noninteractive

# 安装所需的编译器和其他依赖项
RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    zlib1g-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    llvm \
    libncurses5-dev \
    libncursesw5-dev \
    xz-utils \
    tk-dev \
    libffi-dev \
    liblzma-dev \
    python3-openssl \
    python3-pip \
    wget

# 将本地的 Python 压缩包复制到容器中
COPY Python-3.8.12.tar.xz .


# 解压 Python 压缩包并进行安装
# RUN wget https://www.python.org/ftp/python/3.8.12/Python-3.8.12.tar.xz &&
RUN tar -xf Python-3.8.12.tar.xz && \
	cd Python-3.8.12 && \
    ./configure --enable-optimizations && \
    make -j$(nproc) && \
    make altinstall

# 删除临时文件
RUN rm -f Python-3.8.12.tar.xz
# 方便直接执行python
RUN ln Python-3.8.12/python /usr/bin/python
# 设置容器启动时的默认命令
# CMD ["python", "--version"]
CMD ["bash"]


# 记得打包打成judger:base

