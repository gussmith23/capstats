#include <argp.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "capstats_server.h"

static char doc[] =
  "capstats -- an HTTP server exposing an API for tracking caps statistics.";

/* A description of the arguments we accept. */
static char args_doc[] = "";

static struct argp_option options[] = {
  {"database", 'd', "databasePath", 0, "Path to database file" },
  {"disableAPIKeys", 'r', 0, 0, "Disables API key checking for PUT/POST requests; for testing purposes only" },
  {"port", 'p', "port", 0, "Port number"},
  {"config", 'c', "configPath", 0, "Path to config file"},
  {"key", 'k', "key", 0, "Add the given API key to the keys database"},
  { 0 }
};

struct arguments
{
  uint16_t port;
  std::string databasePath;
  std::string configPath;
  bool checkAPIKeys;
  std::vector<DefaultUUID> defaultUUIDs;
};

  static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = (struct arguments*) state->input;

  int port;

  switch (key)
  {
    case 'd':
      arguments->databasePath = arg;
      break;

    case 'r':
      arguments->checkAPIKeys = false;
      break;

    case 'p':
      port = atoi(arg);
      if (port != 0) arguments->port = port;
      else argp_usage(state);
      break;

    case 'c':
      arguments->configPath = arg;
      break;

    case 'k':
      arguments->defaultUUIDs.push_back({ arg, "" });
      break;
    
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char** argv) {

  // Default server settings.
  // TODO(gus): these should be defined within CapstatsServer itself. However,
  // I'm not sure on the best way to do this. I was thinking of using default
  // arguments in the constructor, but that becomes a pain in the ass to set
  // only a subset of the settings when constructing the server. Why doesn't
  // C++ have named args??
  
  struct arguments arguments;
  arguments.port = 23232;
  arguments.databasePath = ":memory:";
  arguments.checkAPIKeys = true;
  arguments.configPath = "";

  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  // TODO(gus): preferably, we would load the config and then overwrite config
  // settings with command line settings. currently we're doing the opposite.
  
  // If a config file is specified, load it and get its data.
  if (!arguments.configPath.empty()) { 
    JsonBox::Value config; config.loadFromFile(arguments.configPath);

    // Get the port value from the config if it exists; otherwise, use the
    // existing port setting.
    arguments.port = config["port"].tryGetInteger(arguments.port);

    arguments.databasePath = config["databasePath"].tryGetString(arguments.databasePath);

    JsonBox::Array defaultUUIDsJSON = config["default_uuids"].getArray();
    for (auto v = defaultUUIDsJSON.begin(); v != defaultUUIDsJSON.end(); ++v) {
      arguments.defaultUUIDs.push_back({
          (*v)["uuid"].getString(),
          // TODO(gus): this implicitly requires us to have a description.
          (*v)["description"].getString()
      });
    }
  }

  CapstatsServer server = CapstatsServer(arguments.port, arguments.databasePath, arguments.defaultUUIDs, arguments.checkAPIKeys);

  server.init();
  return server.run();
}
