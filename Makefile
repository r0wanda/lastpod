GPOD_CFG=$(shell pkg-config libgpod-1.0 --cflags --libs)
CURLPP_CFG=$(shell pkg-config curlpp --cflags --libs)

main: main.cpp Cache.hpp Lastfm.hpp
	g++ -o lastpod main.cpp -Wall -Wextra -std=c++20 -g $(GPOD_CFG) -lglib-2.0 $(CURLPP_CFG) -lcrypto

rpi: main.cpp Cache.hpp Lastfm.hpp
	g++ -o lastpod main.cpp -Wall -Wextra -DRPI -std=c++20 -g $(GPOD_CFG) -lglib-2.0 $(CURLPP_CFG) -lcrypto
	g++ -o eject eject.cpp -Wall -Wextra -std=c++20
