/*
Types.hpp
这里放各模块共用的简单类型
注意字符下标和显示列不是一回事,一个Tab在Buffer中只有一个字符,在屏幕上可能占多列.
*/
#ifndef MINIVIM_TYPES_HPP
#define MINIVIM_TYPES_HPP

#include <cstddef>

namespace sjtu {

struct Position {
    //Buffer中的行号和字符下标;插入模式的column_可以等于该行长度
    size_t row_{0};
    size_t column_{0};
};

struct ScreenSize {
    //终端的总行数和总列数,包括底部用于显示命令或提示的那一行
    size_t rows_{0};
    size_t columns_{0};
};

struct Viewport {
    //top_是顶部可见的Buffer行号,left_是左侧可见的显示列
    //rows_和columns_是正文区域的高度和宽度,不包含底部提示行
    size_t top_{0};
    size_t left_{0};
    size_t rows_{1};
    size_t columns_{1};
};

enum class CharTypeForWord {
    Char,//字母数字下划线
    Empty,//空格
    Punc,//标点
};

//分别表示普通模式、插入模式和冒号命令行模式
enum class Mode {
    Normal,
    Insert,
    CommandLine,
};

//Basic中支持的四个移动方向,可以由hjkl等按键生成
enum class Operator {
    Delete,
    Replace,
    Change,
    Yank,
    Visual_Select,
};
enum class Motion {
    Left,
    Down,
    Up,
    Right,
    Next_Word_Start,
    Pre_Word_Start,
    Next_Word_End,
    Pre_Word_End,
    Next_WORD_Start,
    Pre_WORD_Start,
    Next_WORD_End,
    Pre_WORD_End,
    Line_Start,
    Line_First_None_Empty,
    Line_End,
    First_Line,
    Last_line,
};
enum class Modifier{
    Inside,
    Around,
};

} // namespace sjtu

#endif // MINIVIM_TYPES_HPP;
