#include "Window.hpp"
#include "TextLayout.hpp"

#include <algorithm>
#include <cmath>
#include <cctype>

namespace sjtu {


void Window::Resize(ScreenSize terminal_size) {
    //底部留一行给命令或提示,其余作为正文区域;正文行数和列数都至少取1
    //修改视口即可
    viewport_.rows_ = std::max((size_t)1, terminal_size.rows_ - 1);
    viewport_.columns_ = std::max((size_t)1, terminal_size.columns_);
}

void Window::ApplyMotion(const Buffer& buffer, Motion motion, size_t count) {
    //1. 根据方向调用对应的移动函数,Basic中每次移动一步,在Advanced中你可以改变count/添加别的case
    //2. 将行列限制在Normal模式的合法范围内(Buffer应始终至少有一行)
    //3. 调整视口,让移动后的光标可见(EnsureCursorVisible)
    switch (motion)
    {
    case Motion::Left:
        MoveLeft(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Right:
        MoveRight(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Up:
        MoveUp(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Down:
        MoveDown(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Next_Word_Start:
        MoveNextWordStart(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Pre_Word_End:
        MovePreWordEnd(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Next_Word_End:
        MoveNextWordEnd(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Pre_Word_Start:
        MovePreWordStart(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Line_Start:
        MoveLineStart();
        break;
    case Motion::Line_First_None_Empty:
        MoveLineFirstNoneEmpty(buffer);
        break;
    case Motion::Line_End:
        MoveLineEnd(buffer, count);
        break;
    case Motion::Next_WORD_Start:
        MoveNextWORDStart(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Pre_WORD_End:
        MovePreWORDEnd(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Next_WORD_End:
        MoveNextWORDEnd(buffer, count == 0 ? 1 : count);
        break;
    case Motion::Pre_WORD_Start:
        MovePreWORDStart(buffer, count == 0 ? 1 : count);
        break;
    case Motion::First_Line:
        MoveFirstLine(buffer, count);
        break;
    case Motion::Last_line:
        MoveLastLine(buffer, count);
        break;
    default:
        break;
    }
    EnsureCursorVisible(buffer);
}

void Window::EnsureCursorVisible(const Buffer& buffer) {
    //1. 光标高于或低于可见区域时,调整top_,使光标刚好进入区域
    //2. 把光标的字符下标换算成显示列,再用相同思路调整left_
    if(cursor_.row_ < viewport_.top_)viewport_.top_ = cursor_.row_;
    else if(cursor_.row_ > viewport_.top_ + viewport_.rows_ - 1)viewport_.top_ = cursor_.row_ - viewport_.rows_ + 1;
    size_t _col = BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_), cursor_.column_);
    if(_col < viewport_.left_)viewport_.left_ = _col;
    else if(_col > viewport_.left_ + viewport_.columns_ - 1)viewport_.left_ = _col - viewport_.columns_ + 1;
}

const Position& Window::GetCursor() const {
    //返回当前光标位置的只读引用
    return cursor_;
}

const Viewport& Window::GetViewport() const {
    //返回当前可见区域的只读引用,供Renderer绘制
    return viewport_;
}

void Window::SetCursor(const Buffer& buffer, Position position, bool allow_line_end) {
    //1. 先限制行号,再根据该行长度和allow_line_end限制列号
    //2. 用新位置更新上下移动时的目标显示列
    //3. 调整视口,保证光标可见
    cursor_.row_ = std::clamp(position.row_, (size_t)0, buffer.GetLineCount() - 1);
    if(buffer.GetLineAt(cursor_.row_).length() == 0)cursor_.column_ = 0;
    else{
        if(allow_line_end)cursor_.column_ = std::clamp(position.column_, (size_t)0, buffer.GetLineAt(cursor_.row_).length());
        else cursor_.column_ = std::clamp(position.column_, (size_t)0, buffer.GetLineAt(cursor_.row_).length() - 1);
    }
    desired_column_ = cursor_.column_;
    EnsureCursorVisible(buffer);
}

void Window::MoveLeft(const Buffer& buffer, std::size_t count) {
    //向左移动count个字符,最多到行首,并更新目标显示列
    if(buffer.GetLineAt(cursor_.row_).empty())cursor_.column_ = 0;
    else if(cursor_.column_ < count)cursor_.column_ = 0;
    else cursor_.column_ = cursor_.column_ - count;
    desired_column_ = cursor_.column_;
}

void Window::MoveRight(const Buffer& buffer, std::size_t count) {
    //向右移动count个字符,最多到最后一个字符,并更新目标显示列
    if(buffer.GetLineAt(cursor_.row_).empty())cursor_.column_ = 0;
    else if(cursor_.column_ + count >= buffer.GetLineAt(cursor_.row_).length())cursor_.column_ = buffer.GetLineAt(cursor_.row_).length() - 1;
    else cursor_.column_ += count;
    desired_column_ = cursor_.column_;
}

void Window::MoveUp(const Buffer& buffer, std::size_t count) {
    //先算目标行,最多到第一行,再将期望的显示列换算成目标行的字符下标
    //经过短行时不要更新desired_screen_column_,这样继续移动到长行时能回到原来的列
    if(cursor_.row_ < count)cursor_.row_ = 0;
    else cursor_.row_ = cursor_.row_ - count;
    if(buffer.GetLineAt(cursor_.row_).length() == 0){
        cursor_.column_ = 0;
    }
    else cursor_.column_ = std::min(desired_column_, buffer.GetLineAt(cursor_.row_).length() - 1);
}

void Window::MoveDown(const Buffer& buffer, std::size_t count) {
    if(cursor_.row_ + count >= buffer.GetLineCount())cursor_.row_ = buffer.GetLineCount() - 1;
    else cursor_.row_ = cursor_.row_ + count;
    if(buffer.GetLineAt(cursor_.row_).length() == 0){
        cursor_.column_ = 0;
    }
    else cursor_.column_ = std::min(desired_column_, buffer.GetLineAt(cursor_.row_).length() - 1);
}

void Window::MoveNextWordStart(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    while (count > 0)
    {
        if(!Advance(buffer, p, 0, 0)){
            break;
        }
        count--;
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

void Window::MoveNextWORDStart(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    while (count > 0)
    {
        if(!Advance(buffer, p, 0, 1)){
            break;
        }
        count--;
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

void Window::MovePreWordStart(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    if(GetPosCharType(buffer, p) != CharTypeForWord::Empty){
        std::string_view str = buffer.GetLineAt(p.row_);
        if(p.column_ > 0 && GetPosCharType(buffer, p) == GetPosCharType(buffer, {p.row_, p.column_ - 1})){
            count--;
        }
    }
    while (count > 0)
    {
        if(!Former(buffer, p, 0, 0)){
            break;
        }
        count--;
    }
    std::string_view str = buffer.GetLineAt(p.row_);
    if(str.empty()){
        cursor_ = p;
        desired_column_ = cursor_.column_;
        return;
    }
    for(;p.column_ > 0; p.column_--){
        if(GetPosCharType(buffer, p) != GetPosCharType(buffer, {p.row_, p.column_ - 1})){
            cursor_ = p;
            desired_column_ = cursor_.column_;
            return;
        }
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

void Window::MovePreWORDStart(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    if(GetPosCharType(buffer, p) != CharTypeForWord::Empty){
        std::string_view str = buffer.GetLineAt(p.row_);
        if(p.column_ > 0 && CharTypeForWord::Empty != GetPosCharType(buffer, {p.row_, p.column_ - 1})){
            count--;
        }
    }
    while (count > 0)
    {
        if(!Former(buffer, p, 0, 1)){
            break;
        }
        count--;
    }
    std::string_view str = buffer.GetLineAt(p.row_);
    if(str.empty()){
        cursor_ = p;
        desired_column_ = cursor_.column_;
        return;
    }
    for(;p.column_ > 0; p.column_--){
        if(CharTypeForWord::Empty == GetPosCharType(buffer, {p.row_, p.column_ - 1})){
            cursor_ = p;
            desired_column_ = cursor_.column_;
            return;
        }
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

void Window::MoveNextWordEnd(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    if(GetPosCharType(buffer, p) != CharTypeForWord::Empty){
        std::string_view str = buffer.GetLineAt(p.row_);
        if(p.column_ + 1 < str.length() && GetPosCharType(buffer, p) == GetPosCharType(buffer, {p.row_, p.column_ + 1})){
            count--;
        }
    }
    while (count > 0)
    {
        if(!Advance(buffer, p, 1, 0)){
            break;
        }
        count--;
    }
    std::string_view str = buffer.GetLineAt(p.row_);
    if(str.empty()){
        cursor_ = p;
        desired_column_ = cursor_.column_;
        return;
    }
    for(;p.column_ + 1 < str.length(); p.column_++){
        if(GetPosCharType(buffer, p) != GetPosCharType(buffer, {p.row_, p.column_ + 1})){
            cursor_ = p;
            desired_column_ = cursor_.column_;
            return;
        }
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

void Window::MoveNextWORDEnd(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    if(GetPosCharType(buffer, p) != CharTypeForWord::Empty){
        std::string_view str = buffer.GetLineAt(p.row_);
        if(p.column_ + 1 < str.length() && CharTypeForWord::Empty != GetPosCharType(buffer, {p.row_, p.column_ + 1})){
            count--;
        }
    }
    while (count > 0)
    {
        if(!Advance(buffer, p, 1, 1)){
            break;
        }
        count--;
    }
    std::string_view str = buffer.GetLineAt(p.row_);
    if(str.empty()){
        cursor_ = p;
        desired_column_ = cursor_.column_;
        return;
    }
    for(;p.column_ + 1 < str.length(); p.column_++){
        if(CharTypeForWord::Empty == GetPosCharType(buffer, {p.row_, p.column_ + 1})){
            cursor_ = p;
            desired_column_ = cursor_.column_;
            return;
        }
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

void Window::MovePreWordEnd(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    while (count > 0)
    {
        if(!Former(buffer, p, 0, 0)){
            break;
        }
        count--;
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

void Window::MovePreWORDEnd(const Buffer& buffer, std::size_t count){
    Position p = cursor_;
    while (count > 0)
    {
        if(!Former(buffer, p, 0, 1)){
            break;
        }
        count--;
    }
    cursor_ = p;
    desired_column_ = cursor_.column_;
}

bool Window::Former(const Buffer& buffer, Position& pos, bool ignoreEmptyLine, bool isWORD){
    if(pos.row_ == 0 && pos.column_ == 0){
        return false;
    }
    std::string_view str = buffer.GetLineAt(pos.row_);
    CharTypeForWord charType = GetPosCharType(buffer, pos);
    if(pos.column_ == 0){
        pos.row_--;
        str = buffer.GetLineAt(pos.row_);
        if(str.empty())pos.column_ = 0;
        else pos.column_ = str.length() - 1;
        charType = CharTypeForWord::Empty;
    }
    else pos.column_--;

    while (true)
    {
        if(pos.row_ == 0 && pos.column_ == 0){
            return false;
        }
        str = buffer.GetLineAt(pos.row_);
        if(str.empty() && !ignoreEmptyLine)return true;
        CharTypeForWord newCharType = GetPosCharType(buffer, pos);
        if(!isWORD){
            if(newCharType != CharTypeForWord::Empty && newCharType != charType)return true;
        }
        else{
            if(newCharType != CharTypeForWord::Empty && charType == CharTypeForWord::Empty)return true;
        }
        
        charType = newCharType;

        if(pos.column_ == 0){
            pos.row_--;
            str = buffer.GetLineAt(pos.row_);
            if(str.empty())pos.column_ = 0;
            else pos.column_ = str.length() - 1;
            charType = CharTypeForWord::Empty;
        }
        else pos.column_--;
    }
    return false;
}

bool Window::Advance(const Buffer& buffer, Position& pos, bool ignoreEmptyLine, bool isWORD){
    if(buffer.GetLineCount() == pos.row_ + 1 && buffer.GetLastLine().length() <= pos.column_ + 1){
        return false;
    }
    std::string_view str = buffer.GetLineAt(pos.row_);
    CharTypeForWord charType = GetPosCharType(buffer, pos);
    if(buffer.GetLineAt(pos.row_).length() <= pos.column_ + 1){
        pos.row_++;
        pos.column_ = 0;
        charType = CharTypeForWord::Empty;
    }
    else pos.column_++;

    while (true)
    {
        if(buffer.GetLineCount() == pos.row_ + 1 && buffer.GetLastLine().length() <= pos.column_ + 1){
            return false;
        }
        str = buffer.GetLineAt(pos.row_);
        if(str.empty() && !ignoreEmptyLine)return true;
        CharTypeForWord newCharType = GetPosCharType(buffer, pos);
        if(!isWORD){
            if(newCharType != CharTypeForWord::Empty && newCharType != charType)return true;
        }
        else{
            if(newCharType != CharTypeForWord::Empty && charType == CharTypeForWord::Empty)return true;
        }
        
        charType = newCharType;

        if(str.length() <= pos.column_ + 1){
            pos.row_++;
            pos.column_ = 0;
            charType = CharTypeForWord::Empty;
        }
        else pos.column_++;
    }
    return false;
}

void Window::MoveLineStart(){
    cursor_.column_ = 0;
    desired_column_ = 0;
}

void Window::MoveLineFirstNoneEmpty(const Buffer& buffer){
    std::string_view str = buffer.GetLineAt(cursor_.row_);
    if(str.empty()){
        cursor_.column_ = desired_column_ = 0;
        return;
    }
    size_t p = 0;
    for(; p < str.length(); p++){
        if(str[p] != ' '){
            cursor_.column_ = desired_column_ = p;
            return;
        }
    }
    cursor_.column_ = desired_column_ = p - 1;
}

void Window::MoveLineEnd(const Buffer& buffer, std::size_t count){
    if(cursor_.row_ + 1 == buffer.GetLineCount() && count > 1){
        desired_column_ = curs_Max;
        return;
    }
    MoveDown(buffer, count - 1);
    std::string_view str = buffer.GetLineAt(cursor_.row_);
    if(str.empty())cursor_.column_ = 0;
    else cursor_.column_ = str.length() - 1;
    desired_column_ = curs_Max;
}

void Window::MoveFirstLine(const Buffer& buffer, std::size_t count){
    if(count == 0)cursor_.row_ = 0;
    else if(count > buffer.GetLineCount())cursor_.row_ = buffer.GetLineCount() - 1;
    else cursor_.row_ = count - 1;
    MoveLineFirstNoneEmpty(buffer);
}

void Window::MoveLastLine(const Buffer& buffer, std::size_t count){
    if(count == 0)cursor_.row_ = buffer.GetLineCount() - 1;
    else if(count > buffer.GetLineCount())cursor_.row_ = buffer.GetLineCount() - 1;
    else cursor_.row_ = count - 1;
    MoveLineFirstNoneEmpty(buffer);
}

CharTypeForWord Window::GetPosCharType(const Buffer& buffer, const Position& pos){
    std::string_view str = buffer.GetLineAt(pos.row_);
    if(pos.column_ == str.length())return CharTypeForWord::Empty;
    char c = str[pos.column_];
    if(c == ' ')return CharTypeForWord::Empty;
    if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))return CharTypeForWord::Char;
    return CharTypeForWord::Punc;
}   

} // namespace sjtu

/*
./MiniVim ./f.txt
*/