# Ubuntu 20.04 LTS is the standard platform for development
FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    make \
    git \
    cmake \
    software-properties-common \
    python3 \
    python3-pip \
    doxygen \
    curl \
    wget \
    time \
    clang

RUN ln -sf /usr/bin/python3 /usr/bin/python
RUN ln -sf /usr/bin/pip3 /usr/bin/pip
# virtual env does not currently work past version 16.7.8.
RUN python -m pip install virtualenv==16.7.8

RUN ln -sf /usr/bin/llvm-profdata-10 /usr/bin/llvm-profdata
RUN ln -sf /usr/bin/llvm-cov-10 /usr/bin/llvm-cov

# Update gcc to newer version
RUN wget --output-document="/tmp/arm-gcc.tar.bz2" \
https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
RUN cd /usr/bin && tar xjf /tmp/arm-gcc.tar.bz2
ENV PATH "$PATH:/usr/bin/gcc-arm-none-eabi-10.3-2021.10/bin"

#testing sonarqube scanner and c files
RUN apt-get install -y zip
RUN wget --output-document="/tmp/sonar-scanner.zip" \
https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-4.3.0.2102-linux.zip
RUN cd /usr/bin && unzip /tmp/sonar-scanner.zip
ENV PATH "$PATH:/usr/bin/sonar-scanner-4.3.0.2102-linux/bin"
# get the wrapper
RUN wget --output-document="/tmp/sonar-wrapper.zip" \
https://sonar.myqops.com/static/cpp/build-wrapper-linux-x86.zip
RUN cd /usr/bin && unzip /tmp/sonar-wrapper.zip
ENV PATH "$PATH:/usr/bin/build-wrapper-linux-x86"

# This is in accordance to : https://www.digitalocean.com/community/tutorials/how-to-install-java-with-apt-get-on-ubuntu-16-04
RUN apt-get update && \
	apt-get install -y openjdk-8-jdk && \
	apt-get install -y ant && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/* && \
	rm -rf /var/cache/oracle-jdk8-installer;
	
# Fix certificate issues, found as of 
# https://bugs.launchpad.net/ubuntu/+source/ca-certificates-java/+bug/983302
RUN apt-get update && \
	apt-get install -y ca-certificates-java && \
	apt-get clean && \
	update-ca-certificates -f && \
	rm -rf /var/lib/apt/lists/* && \
	rm -rf /var/cache/oracle-jdk8-installer;

# Setup JAVA_HOME, this is useful for docker commandline
ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64/
RUN export JAVA_HOME

ENV PATH "/data/pclp:$PATH"

# Below are tested, but not verified speed increases with ccache and lld
# use at your own risk
#RUN apt-get install -y lld
#RUN apt-get install -y ccache
#RUN /usr/sbin/update-ccache-symlinks
#RUN ln -sf /usr/bin/ccache /usr/lib/ccache/arm-none-eabi-gcc
#RUN ln -sf /usr/bin/ccache /usr/lib/ccache/arm-none-eabi-gcc-7.2.1
#RUN ln -sf /usr/bin/ccache /usr/lib/ccache/arm-none-eabi-g++
#RUN ln -sf /usr/bin/lld /usr/bin/ld
#ENV PATH "/usr/lib/ccache:$PATH"

RUN python -m pip install compiledb
