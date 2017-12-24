#include "JsonBox.h"

#include "capstats_config.h"

CapstatsConfig::CapstatsConfig(const std::vector<DefaultUUID>& _defaultUUIDs)
    : defaultUUIDs(_defaultUUIDs) {
    
}

CapstatsConfig::CapstatsConfig(const std::string& filename) {
  JsonBox::Value config;
  config.loadFromFile(filename);

  // TODO(gus): make a constant
  // TODO(gus): bad variable naming
  // TODO(gus): add checks
  JsonBox::Array default_uuids = config["default_uuids"].getArray();
  for (auto v = default_uuids.begin(); v != default_uuids.end(); ++v) {
    DefaultUUID defaultUUID = {
        (*v)["uuid"].getString(),
        // TODO(gus): this implicitly requires us to have a description.
        (*v)["description"].getString()
    };
    defaultUUIDs.push_back(std::move(defaultUUID));
  }
}
