# sshd
#
# VERSION               0.0.2

FROM ubuntu:14.04
MAINTAINER Sven Dowideit <SvenDowideit@docker.com>

RUN apt-get update
RUN apt-get install -y curl
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install -y build-essential
RUN apt-get install -y automake
RUN apt-get install -y bison flex
RUN apt-get install -y libcurl4-gnutls-dev
RUN apt-get install -y autotools-dev
RUN apt-get install -y libboost-dev
RUN apt-get install -y openssh-server
RUN apt-get install -y git

RUN git clone https://github.com/miloyip/rapidjson /tmp/rapidjson
RUN sudo cp -r /tmp/rapidjson/include/rapidjson /usr/include/
RUN git clone https://github.com/JinpengLI/gsoap.git /tmp/gsoap
RUN cd /tmp/gsoap
RUN sed -i '1i#ifndef WITH_COOKIES\n#define WITH_COOKIES\n#endif\n' ./gsoap/stdsoap2.h
RUN aclocal
RUN autoheader
RUN automake --add-missing
RUN autoconf
RUN automake
RUN ./configure
RUN make
RUN sudo make install




RUN mkdir /var/run/sshd
RUN echo 'root:screencast' | chpasswd
RUN sed -i 's/PermitRootLogin without-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
