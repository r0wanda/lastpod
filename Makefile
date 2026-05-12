main: main.cpp
	g++ -o lastpod main.cpp -Wall -Wextra `pkg-config libgpod-1.0 --cflags --libs` -lglib-2.0 `pkg-config curlpp --cflags --libs`
