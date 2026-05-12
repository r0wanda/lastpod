#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

class Lastfm {
public:
	Lastfm(const char *key): apikey(key) {
		std::ostringstream sstr;
		sstr << curlpp::options::Url("http://ws.audioscrobbler.com/2.0");
		std::cout << sstr.str() << std::endl;
	}
private:
	const std::string apikey;
};
