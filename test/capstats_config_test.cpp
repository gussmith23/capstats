#include "catch.hpp"
#include "capstats_config.h"

TEST_CASE("config utility") {
  CapstatsConfig config("test_config_1.cfg");
  REQUIRE(config.getDefaultUUIDs().size() == 1);
  REQUIRE(config.getDefaultUUIDs()[0].uuid == "123e4567-e89b-12d3-a456-426655440000");
  REQUIRE(config.getDefaultUUIDs()[0].description == "test uuid 1");
}
