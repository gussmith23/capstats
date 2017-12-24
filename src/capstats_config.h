#ifndef CAPSTATS_CONFIG_H
#define CAPSTATS_CONFIG_H

#include <string>
#include <vector>

struct DefaultUUID {
  std::string uuid;
  std::string description;
};

/**
 * This class is not currently used, but may become useful in the future if we
 * ever want the server to be aware of a unified config. Wrapping it up in a
 * class might make it easier to enable the server to change config settings
 * and write them back to a file, for example.
 */
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
