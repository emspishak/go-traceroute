CC=gcc
JAVA=java
JAVAC=javac
C_OPTS+=-Wall

all: ip server

ip: ip.c
	$(CC) $(C_OPTS) -o ip ip.c

server:	SimpleUDPServer.class

SimpleUDPServer.class: SimpleUDPServer.java
	$(JAVAC) SimpleUDPServer.java

clean:
	rm -f ip *.o *.class

run-server: server
	$(JAVA) SimpleUDPServer
