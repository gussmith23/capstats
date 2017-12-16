#ifndef CAPSTATS_CONFIG_H
#define CAPSTATS_CONFIG_H

#include <string>
#include <vector>

class CapstatsConfig {
 public:
  CapstatsConfig(const std::vector<std::string>&);
  static CapstatsConfig loadConfig(const std::string& filename);
  const std::vector<std::string>& getDefaultUUIDs() { return defaultUUIDs; }
 private:
  std::vector<std::string> defaultUUIDs;
};

#endif // CAPSTATS_CONFIG_H
