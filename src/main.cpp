#include <string>
#include <vector>

#include "capstats_server.h"

int main(int argc, char** argv) {

  // Default server settings.
  // TODO(gus): these should be defined within CapstatsServer itself. However,
  // I'm not sure on the best way to do this. I was thinking of using default
  // arguments in the constructor, but that becomes a pain in the ass to set
  // only a subset of the settings when constructing the server. Why doesn't
  // C++ have named args??
  std::vector<DefaultUUID> defaultUUIDs;
  uint16_t port = 23232;
  std::string databasePath = ":memory:";

  // If a config file is specified, load it and get its data.
  if (argc >= 2) { 
    JsonBox::Value config; config.loadFromFile(argv[1]);

    // Get the port value from the config if it exists; otherwise, use the
    // existing port setting.
    port = config["port"].tryGetInteger(port);

    databasePath = config["databasePath"].tryGetString(databasePath);

    JsonBox::Array defaultUUIDsJSON = config["default_uuids"].getArray();
    for (auto v = defaultUUIDsJSON.begin(); v != defaultUUIDsJSON.end(); ++v) {
      defaultUUIDs.push_back({
          (*v)["uuid"].getString(),
          // TODO(gus): this implicitly requires us to have a description.
          (*v)["description"].getString()
      });
    }
  }

  CapstatsServer server = CapstatsServer(port, databasePath, defaultUUIDs);

  server.init();
  return server.run();
}
