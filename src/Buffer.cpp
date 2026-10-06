#include <fstream>

#include "Buffer.hpp"

namespace sjtu {

Buffer::Buffer(const std::filesystem::path& path){
    //从path指向的文件构造Buffer,你需要打开文件并且把文件内容填充进Buffer,并正确初始化一些状态.
    //注意path可能为空的边界情况
    if(path.empty() || !std::filesystem::exists(path)){
        lines_ = {""};
    }
    else{
        std::ifstream file(path);
        std::string line;
        while (getline(file,line))
        {
            lines_.push_back(line);
        }
        if(lines_.empty())lines_.push_back("");
    }
    saved_ = lines_;
    path_ = path;
}

Buffer::Buffer(std::vector<std::string> lines, std::filesystem::path path) {
    lines_ = lines;
    path_ = path;
}

std::size_t Buffer::GetLineCount() const {
    //返回文件行数
    return lines_.size();
}

const std::string& Buffer::GetLineAt(std::size_t row) const {
    //返回第row行的内容
    return lines_[row];
}


std::string Buffer::GetDisplayName() const {
    //返回文件名,若是新文件,返回"[No Name]"
    if(path_.empty()){
        return "[No Name]";
    }
    return path_.filename().string();
}

bool Buffer::IsModified() const {
    //返回文件和上次保存比起来是否被修改过
    return lines_ == saved_;
}

void Buffer::InsertCharacter(std::size_t row, std::size_t column, char value) {
    //在第row行第col列插入一个value, 注意越界检查
    if(row >= lines_.size() || row < 0)return;
    if(column > lines_[row].length())return;
    lines_[row].insert(column,1,value);
}

void Buffer::EraseCharacter(std::size_t row, std::size_t column) {
    //在第row行第col列删除一个value
    if(row >= lines_.size())return;
    if(column >= lines_[row].length())return;
    lines_[row].erase(column, 1);
}

void Buffer::SplitLine(std::size_t row, std::size_t column) {
    //在第row行第col列分割,即在此处敲了回车键
    if(row >= lines_.size())return;
    if(column > lines_[row].length())return;
    std::string first,second;
    first = lines_[row].substr(0,column);
    second = lines_[row].substr(column);
    lines_[row] = first;
    lines_.insert(lines_.begin() + row + 1, second);
}

void Buffer::JoinLine(std::size_t row) {
    //把第row + 1行合并进第row行
    if(row + 1 >= lines_.size())return;
    lines_[row] += lines_[row + 1];
    lines_.erase(lines_.begin() + row + 1);
}

void Buffer::Save() {
    //把文件内容保存, 直接调用WriteTo方法
    Buffer::WriteTo(path_);
    saved_ = lines_;
}

void Buffer::SaveAs(const std::filesystem::path& path) {
    Buffer::WriteTo(path);
    path_ = path;
    saved_ = lines_;
}


void Buffer::WriteTo(const std::filesystem::path& path) const {
    //实际将缓冲区中的内容写入path指向的文件中
    if(path.empty())return;
    std::ofstream out(path);
    for (std::size_t i = 0; i < lines_.size(); i++){
        out << lines_[i] << '\n';
    }
}

} // namespace sjtu
