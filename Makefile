all:

server:
	g++ Server.cpp -o Server
	./Server 20000

client:
	g++ Client.cpp -o Client
	./Client bob@student11-H97M-D3H:20000 "First conversation with Bob via mail" mail.txt

clean:
	rm -rf *o Server
	rm -rf *o Client