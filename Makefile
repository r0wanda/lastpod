main: main.cpp Cache.hpp Lastfm.hpp
	g++ -o lastpod main.cpp -Wall -Wextra -std=c++20 -g `pkg-config libgpod-1.0 --cflags --libs` -lglib-2.0 `pkg-config curlpp --cflags --libs` -lcrypto

rpi: main.cpp Cache.hpp Lastfm.hpp
	g++ -o lastpod main.cpp -Wall -Wextra -DRPI -std=c++20 -g `pkg-config libgpod-1.0 --cflags --libs` -lglib-2.0 `pkg-config curlpp --cflags --libs` -lcrypto
