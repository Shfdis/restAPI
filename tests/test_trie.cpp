#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../network_handler/path_trie.h"
TEST_CASE("Trie", "[first]") {
    PathTrie<int> trie;
    trie.Add("/home/one", 1);
    trie.Add("/home/two", 2);
    REQUIRE(*trie["/home/one"] == 1);
    REQUIRE(*trie["/home/two"] == 2);
    REQUIRE(trie["dhhdhf"] == nullptr);
    REQUIRE_THROWS(trie.Add("/home/one", 2));
    REQUIRE_THROWS_AS(trie.Add("/home/one", 1), std::logic_error);
}
