#include "JsonBox.h"

#include "capstats_config.h"

CapstatsConfig::CapstatsConfig(const std::vector<std::string>& _defaultUUIDs)
  : defaultUUIDs(_defaultUUIDs) {}

CapstatsConfig CapstatsConfig::loadConfig(const std::string& filename) {
  JsonBox::Value config;
  config.loadFromFile(filename);

  // TODO(gus): make a constant
  // TODO(gus): add checks
  JsonBox::Array default_uuids = config["default_uuids"].getArray();
  std::vector<std::string> defaultUUIDs;
  for (auto v = default_uuids.begin(); v != default_uuids.end(); ++v) {
    defaultUUIDs.push_back(v->getString());
  }

  return CapstatsConfig(defaultUUIDs);
}
