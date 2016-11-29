#include "capstats_server.h"

using namespace std;

int main() {
	CapstatsServer server(23232, ":memory:");
	server.init();
	return server.run();
}