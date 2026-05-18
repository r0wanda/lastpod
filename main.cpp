#include <string>
#include <iostream>
#include <gpod/itdb.h>
#include <curlpp/cURLpp.hpp>
#include "Lastfm.hpp"
#include "Cache.hpp"
#include "eject.hpp"
#include "deps/dotenv.h"
#ifdef RPI
#include "rpi.hpp"
#endif
#define GERROR(e, fatal) if (e) { if (e->message) std::cerr << e->message; g_error_free(e); e = NULL; if (fatal) return 1; }

int main(int argc, char **argv) {
	cURLpp::initialize();
	dotenv::init();
#ifdef RPI
	Leds leds();
#endif

	Lastfm lfm(std::getenv("LASTFM_API_KEY"), std::getenv("LASTFM_API_SECRET"));
	Cache cache;
	lfm.load(&cache);

	GError *err = nullptr;
	Itdb_iTunesDB *itdb;
	gchar *inp = nullptr;
	if (argc < 2) {
		std::cerr << "usage: " << g_path_get_basename(argv[0]) << " <mountpoint>" << std::endl;
		return 1;
	}
	inp = argv[1];

	itdb = itdb_parse(inp, &err);
	GERROR(err, true);

	std::cout << "tracks: " << g_list_length(itdb->tracks) << std::endl;
	GList *it;
	Scrobble *first = nullptr;
	Scrobble *prev = nullptr;
	int n = 0;
	int total = 0;
	
	for (it = itdb->tracks; it != NULL; it = it->next) {
		Itdb_Track *tr = (Itdb_Track*)it->data;
		if (tr->playcount < 1) continue;
		std::string title(tr->title);
		unsigned short playcount = static_cast<unsigned short>(tr->playcount);
		unsigned long ts = static_cast<unsigned long>(tr->time_played);
		cache.dbset(&cache.db, title, playcount, ts);
		std::pair<unsigned short, unsigned long> cData;
		if (cache.initdb.contains(tr->title)) {
			cData = cache.initdb.at(tr->title);
		} else {
			cData = std::pair(0, 0);
		}

		if (ts <= cData.second) continue;
		int plays = playcount - cData.first;
		for (int i = 0; i < plays; i++) {
			if (n == 48) {
				int ign = lfm.scrobble(first, 48);
#ifdef RPI
				leds.blink(ign, 500, 100);
#endif
				first = nullptr;
				prev = nullptr;
				total += n;
				n = 0;
			}
			Scrobble *sc = (Scrobble*)malloc(sizeof(Scrobble));
			sc->track = tr;
			// add 60 seconds between repeated tracks bc itdb doesnt track all timestamps for plays
			sc->ts = tr->time_played + i * 60;
			if (first == nullptr) {
				first = sc;
				prev = sc;
			} else {
				prev->next = sc;
				prev = sc;
			}
			n++;
		}
		//std::cout << tr->title << ": " << tr->playcount << " @ " << tr->time_played << std::endl;
	}
	int ignf = lfm.scrobble(first, n);
#ifdef RPI
	leds.blink(ignf, 500, 100);
#endif
	std::cout << "scrobbled " << (total + n) << " songs" << std::endl;

	cache.write();
	itdb_free(itdb);
	cURLpp::terminate();
	eject(inp);
	return 0;
}
