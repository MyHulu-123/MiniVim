/*
Window.hpp
Window保存光标位置和正文的可见区域,根据Buffer的行数和行长限制移动范围.
移动只改变光标和视口,文件内容的修改由Buffer完成.
*/
#ifndef MINIVIM_WINDOW_HPP
#define MINIVIM_WINDOW_HPP

#include "Buffer.hpp"
#include "Types.hpp"


namespace sjtu {

class Window {

public:
    void Resize(ScreenSize terminal_size);

    void ApplyMotion(const Buffer& buffer, Motion motion, size_t count = 0);

    void EnsureCursorVisible(const Buffer& buffer);

    //allow_line_end为true时允许停在最后一个字符之后,供插入模式使用
    void SetCursor(const Buffer& buffer, Position position, bool allow_line_end);
    const Position& GetCursor() const ;
    const Viewport& GetViewport() const ;

private:
    void MoveLeft(const Buffer& buffer, size_t count);
    void MoveRight(const Buffer& buffer, size_t count);
    void MoveUp(const Buffer& buffer, size_t count);
    void MoveDown(const Buffer& buffer, size_t count);
    void MoveNextWordStart(const Buffer& buffer, size_t count);
    void MovePreWordStart(const Buffer& buffer, size_t count);
    void MoveNextWordEnd(const Buffer& buffer, size_t count);
    void MovePreWordEnd(const Buffer& buffer, size_t count);
    void MoveNextWORDStart(const Buffer& buffer, size_t count);
    void MovePreWORDStart(const Buffer& buffer, size_t count);
    void MoveNextWORDEnd(const Buffer& buffer, size_t count);
    void MovePreWORDEnd(const Buffer& buffer, size_t count);
    void MoveLineStart();
    void MoveLineFirstNoneEmpty(const Buffer& buffer);
    void MoveLineEnd(const Buffer& buffer, size_t count);
    void MoveFirstLine(const Buffer& buffer, size_t count);
    void MoveLastLine(const Buffer& buffer, size_t count);


    bool Former(const Buffer& buffer, Position& pos, bool ignoreEmptyLine, bool isWORD);
    bool Advance(const Buffer& buffer, Position& pos, bool ignoreEmptyLine, bool isWORD);

    CharTypeForWord GetPosCharType(const Buffer& buffer, const Position& pos);

    Position cursor_{}; //Buffer中的光标位置
    Viewport viewport_{}; //正文可见区域及其滚动偏移
    size_t desired_column_{0}; //上下移动时希望保持的显示列,经过短行时也保留这个目标
    const size_t curs_Max = 1073741823;
};

} // namespace sjtu

#endif // MINIVIM_WINDOW_HPP
