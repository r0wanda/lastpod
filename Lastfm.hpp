#pragma once
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <gpod/itdb.h>
#include <openssl/md5.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>
#include "Cache.hpp"

struct Scrobble {
	Itdb_Track *track;
	Scrobble *next = nullptr;
	unsigned long ts;
};

class Lastfm {
public:
	Lastfm(const char *key, const char *sec):
		apikey(key), secret(sec),
		userAgent("lastpod (https://github.com/r0wanda/lastpod)") {
	}
	nlohmann::json fetch(std::string method, std::map<std::string, std::string> opts) {
		std::list<std::string> headers;
		headers.push_back("User-Agent: " + userAgent);
		using namespace curlpp::Options;
		curlpp::Easy req;
		req.setOpt(new HttpHeader(headers));

		std::string url = "https://ws.audioscrobbler.com/2.0/";
		std::string sig = "";
		opts["api_key"] = apikey;
		opts["method"] = method;
		bool first = true;
		for (auto &opt : opts) {
			if (first) {
				url += "?";
				first = false;
			} else url += "&";
			url += opt.first + "=" + opt.second;
			sig += opt.first + opt.second;
		}
		url += "&format=json";
		sig += secret;
		url += "&api_sig=" + md5(sig);

		std::cout << url << std::endl;
		req.setOpt(new Url(url));
		std::ostringstream sstrm;
		sstrm << req;

		nlohmann::json res = nlohmann::json::parse(sstrm.str());
		if (res.contains("error")) std::cerr << "error " << res["error"] << ": " << res["message"] << std::endl;
		return res;
	}
	nlohmann::json post(std::string method, std::map<std::string, std::string> opts) {
		std::string url = "https://ws.audioscrobbler.com/2.0/";
		std::string sig = "";
		opts["api_key"] = apikey;
		opts["method"] = method;
		opts["sk"] = sk;
		std::string body = "";
		bool first = true;
		for (auto &opt : opts) {
			std::cout << opt.first << ":" << opt.second << std::endl;
			if (first) {
				body += "?";
				first = false;
			} else body += "&";
			body += opt.first + "=" + opt.second;
			sig += opt.first + opt.second;
		}
		body += "&format=json";
		sig += secret;
		body += "&api_sig=" + md5(sig);

		std::cout << body << std::endl;

		std::list<std::string> headers;
		headers.push_back("User-Agent: " + userAgent);
		headers.push_back("Content-Type: application/x-www-form-urlencoded");
		headers.push_back("Content-Length: " + std::to_string(body.length()));
		using namespace curlpp::Options;
		curlpp::Easy req;
		req.setOpt(new HttpHeader(headers));
		req.setOpt(new PostFields(body));
		req.setOpt(new PostFieldSize(body.length()));
		req.setOpt(new Url(url));
		std::ostringstream sstrm;
		sstrm << req;

		std::cout << sstrm.str();
		nlohmann::json res = nlohmann::json::parse(sstrm.str());
		if (res.contains("error")) std::cerr << "error " << res["error"] << ": " << res["message"] << std::endl;
		return res;
	}
	nlohmann::json authfetch(std::string method, std::map<std::string, std::string> opts) {
		opts["sk"] = sk;
		return fetch(method, opts);
	}
	void load(Cache *cache) {
		if (!cache->lfmsk.empty()) {
			sk = cache->lfmsk;
			auto res = authfetch("user.getInfo", {});
			if (!res.contains("error")) {
				std::cout << "logged in as " << res["user"]["name"] << std::endl;
				return;
			}
		}
		std::cout << "logging in" << std::endl;
		login();
		cache->lfmsk = sk;
		return;
	}
	void login() {
		auto tok = fetch("auth.getToken", {});
		std::string token = tok["token"];

		std::string cmd = "xdg-open \"https://www.last.fm/api/auth/?api_key=" + apikey + "&token=" + token + "\"";
		std::system(cmd.c_str());
		std::cout << "press enter when authentication is complete";
		std::cin.ignore();

		auto session = fetch("auth.getSession", {{"token", token}});
		std::cout << session;
		sk = session["session"]["key"];
		std::cout << "logged in as " << session["session"]["name"] << std::endl;
	}
	void scrobble(Scrobble *tracks) {
		std::map<std::string, std::string> body;
		int i = 0;
		Scrobble *prev = nullptr;
		for (Scrobble *sc = tracks; sc->next != nullptr; sc = sc->next) {
			free(prev);
			Itdb_Track *tr = sc->track;
			if (!tr) continue;
			scrobbleI(&body, i, "artist", tr->artist);
			scrobbleI(&body, i, "track", tr->title);
			scrobbleI(&body, i, "timestamp", sc->ts);
			scrobbleI(&body, i, "album", tr->album);
			scrobbleI(&body, i, "duration", tr->tracklen / 1000);
			prev = sc;
		}
		free(prev);
		post("track.scrobble", body);
	}
private:
	template <typename T>
	void scrobbleI(std::map<std::string, std::string> *body, int i, std::string key, T val) {
		std::ostringstream sstrm;
		std::ostringstream vstrm;
		sstrm << key << "[" << i << "]";
		vstrm << val;
		(*body)[sstrm.str()] = vstrm.str();
	}
	std::string md5(std::string sig) {
		std::ostringstream ssig;
		unsigned char digest[MD5_DIGEST_LENGTH];
		MD5_CTX ctx;
		MD5_Init(&ctx);
		MD5_Update(&ctx, sig.c_str(), sig.length());
		MD5_Final(digest, &ctx);
		for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
			ssig << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
		}
		return ssig.str();
	}
	std::string sk;
	const std::string apikey;
	const std::string secret;
	const std::string userAgent;
};
