#include "Searcher.hpp"
namespace sjtu {

Position Searcher::Find(const Buffer& buffer, SearchDirection dir, const Position& cursor, std::string_view str){
    lastDir = dir;
    lastStr = str;
    FindAllStringMatches(buffer, str);
    if(matchPos.empty())return cursor;
    if(dir == SearchDirection::Forward){
        for(const auto& pos : matchPos){
            if(pos.row_ > cursor.row_ || pos.row_ == cursor.row_ && pos.column_ > cursor.column_){
                return pos;
            }
        }
        return matchPos[0];
    }
    if(dir == SearchDirection::Backward){
        size_t i = 0;
        for(const auto& pos : matchPos){
            if(pos.row_ > cursor.row_ || pos.row_ == cursor.row_ && pos.column_ > cursor.column_){
                break;
            }
            i++;
        }
        if(i == 0){
            return matchPos[matchPos.size() - 1];
        }
        return matchPos[i - 1];
    }
    return cursor;
}

void Searcher::FindAllStringMatches(const Buffer& buffer, std::string_view str){
    matchPos.clear();
    for(size_t i = 0; i < buffer.GetLineCount(); i++){
        std::string_view line = buffer.GetLineAt(i);
        for(size_t j = 0; j < line.length(); j++){
            if(str[0] == line[j] && j + str.length() - 1 < line.length()){
                bool matched = true;
                for(size_t k = 1; k < str.length(); k++){
                    if(str[k] != line[j + k]){
                        matched = false;
                        break;
                    }
                }
                if(matched)matchPos.push_back({i,j});
            }
        }
    }
}

}