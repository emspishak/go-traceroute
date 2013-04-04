CC=gcc
JAVA=java
JAVAC=javac
C_OPTS+=-Wall

all: ip server udp

ip: ip.c
	$(CC) $(C_OPTS) -o ip ip.c

udp: udp.c
	$(CC) $(C_OPTS) -o udp udp.c

server:	SimpleUDPServer.class

SimpleUDPServer.class: SimpleUDPServer.java
	$(JAVAC) SimpleUDPServer.java

clean:
	rm -f ip udp *.o *.class

run-server: server
	$(JAVA) SimpleUDPServer
