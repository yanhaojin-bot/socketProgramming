all: clean makeclientA makeclientB makeserver makeserverT makeserverS makeserverP
.PHONY : all

makeclientA: clientA.cpp tcp_client.cpp util.cpp
	g++ --std=c++11 -o clientA clientA.cpp

makeclientB: clientB.cpp tcp_client.cpp
	g++ --std=c++11 -o clientB clientB.cpp

makeserver: serverC.cpp udpClass.cpp tcp_server.cpp
	g++ --std=c++11 -o serverC serverC.cpp

makeserverT: serverT.cpp
	g++ --std=c++11 -o serverT serverT.cpp

makeserverS: serverS.cpp
	g++ --std=c++11 -o serverS serverS.cpp

makeserverP: serverP.cpp
	g++ --std=c++11 -o serverP serverP.cpp


.PHONY : clean
clean:
	rm -f clientB
	rm -f serverC
	rm -f serverT
	rm -f serverS
	rm -f serverP
	rm -f clientA
