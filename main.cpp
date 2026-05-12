#include <iostream>
#include <gpod/itdb.h>
#include <curlpp/cURLpp.hpp>
#include "Lastfm.hpp"
#include "laserpants/dotenv.h"
#define GERROR(e, fatal) if (e) { if (e->message) std::cerr << e->message; g_error_free(e); e = NULL; if (fatal) return 1; }

int main(int argc, char **argv) {
	cURLpp::initialize();
	dotenv::init();
	Lastfm lfm(std::getenv("LASTFM_API_KEY"));
	return 0;

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
	for (it = itdb->tracks; it != NULL; it = it->next) {
		Itdb_Track *tr = (Itdb_Track*)it->data;
		if (tr->playcount < 1) continue;
		std::cout << tr->title << ": " << tr->playcount << " @ " << tr->time_played << std::endl;
	}

	itdb_free(itdb);
	cURLpp::terminate();
	return 0;
}
