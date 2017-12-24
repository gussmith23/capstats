#ifndef CAPSTATS_CONFIG_H
#define CAPSTATS_CONFIG_H

#include <string>
#include <vector>

struct DefaultUUID {
  std::string uuid;
  std::string description;
};

class CapstatsConfig {
 public:
  CapstatsConfig(const std::vector<DefaultUUID>&);

  /**
   * Construct a config from a given filename.
   */
  CapstatsConfig(const std::string&);

  // TODO(gus) add default constructor which initializes to default config values.

  const std::vector<DefaultUUID>& getDefaultUUIDs() 
      { return defaultUUIDs; }
 private:
  std::vector<DefaultUUID> defaultUUIDs;
};

#endif // CAPSTATS_CONFIG_H
