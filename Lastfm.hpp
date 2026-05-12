#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <gpod/itdb.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>

class Lastfm {
public:
	Lastfm(const char *key):
		apikey(key),
		userAgent("lastpod (https://github.com/r0wanda/lastpod)") {
		std::ostringstream sstr;
		sstr << curlpp::options::Url("http://ws.audioscrobbler.com/2.0");
		std::cout << sstr.str() << std::endl;
	}
	nlohmann::json fetch() {
		std::list<std::string> headers;
		headers.push_back("User-Agent: " + userAgent);
		using namespace curlpp::Options;
		curlpp::Easy req;
		req.setOpt(new HttpHeader(headers));
		req.setOpt(new Url(url));
	}
	void scrobble(Itdb_Track *track) {
		
	}
private:
	const std::string apikey;
	const std::string userAgent;
};
