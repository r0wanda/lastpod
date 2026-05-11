#pragma once
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

typedef std::unordered_map<std::string, unsigned short> ITdb;

class Cache {
public:
	ITdb initdb;
	ITdb db;
    Cache() {
        const char *home = std::getenv("HOME");
        path = fs::path(home) / ".lastpod.csv";
	}
	void write() {
		std::fstream strm(path, std::fstream::out | std::fstream::trunc);
		for (auto &it : db) {
			strm << it.first << "\n";
			strm << it.second << "\n";
		}
		strm.close();
	}
private:
    fs::path path;
};
