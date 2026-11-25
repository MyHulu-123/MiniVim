#ifndef MINIVIM_SEARCHER_HPP
#define MINIVIM_SEARCHER_HPP

#include <optional>

#include "Buffer.hpp"
#include "Types.hpp"

namespace sjtu {

class Searcher{
public:
    Position Find(const Buffer& buffer, SearchDirection dir, const Position& cursor, std::string_view str);
private:
    std::optional<SearchDirection> lastDir;
    std::optional<std::string> lastStr;
    std::vector<Position> matchPos;

    void FindAllStringMatches(const Buffer& buffer, std::string_view str);
};

}

#endif // MINIVIM_SEARCHER_HPP;