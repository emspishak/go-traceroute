JAVA=java
JAVAC=javac

all: server

server:	SimpleUDPServer.class

SimpleUDPServer.class: SimpleUDPServer.java
	$(JAVAC) SimpleUDPServer.java

clean:
	rm -f *.class

run-server: server
	$(JAVA) SimpleUDPServer
