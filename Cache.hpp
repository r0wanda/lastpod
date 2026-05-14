#pragma once
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

typedef std::unordered_map<std::string, std::pair<unsigned short, unsigned long>> ITdb;

class Cache {
public:
	unsigned long initdbts;
	std::string lfmsk;
	ITdb initdb;
	ITdb db;
    Cache(): initdbts(0) {
        const char *home = std::getenv("HOME");
        path = /*fs::path(home) /*/ ".lastpod";
		read();
	}
	unsigned long unixtime() {
		const auto t = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch()).count();
	}
	void read() {
		initdb = ITdb();
		db = ITdb();
		std::ifstream strm(path);
		if (!strm.is_open()) return;
		strm >> initdbts;
		strm >> lfmsk;
		std::string title;
		unsigned short plays;
		unsigned long ts;
		while (strm >> title >> plays >> ts) {
			dbset(&initdb, title, plays, ts);
		}
		strm.close();
	}
	void dbset(ITdb *db, std::string title, unsigned short plays, unsigned long ts) {
		(*db)[title] = std::pair<unsigned short, unsigned long>(plays, ts);
	}
	void write() {
		std::fstream strm(path, std::fstream::out | std::fstream::trunc);
		strm << unixtime() << "\n";
		strm << lfmsk << "\n";
		for (auto &it : db) {
			strm << it.first << "\n";
			strm << it.second.first << "\n";
			strm << it.second.second << "\n";
		}
		strm.close();
	}
private:
    fs::path path;
};
