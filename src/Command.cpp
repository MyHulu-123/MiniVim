#include "Command.hpp"

namespace sjtu {

EditorAction NormalModeParser::Feed(KeyEvent key) {
    if (key.code_ == KeyCode::Escape) {
        return {};
    }
    if (key.code_ == KeyCode::Character) {
        auto value = key.value_;
        if(value >= '1' && value <= '9'){
            size_t digit = value - '0';
            if(count_[cnt_] > (COUNT_MAX - digit) / 10)count_[cnt_] = COUNT_MAX;
            else count_[cnt_] = count_[cnt_] * 10 + digit;
            pre_ = value;
        }
        else if(value == '0'){
            if(pre_.has_value() && pre_ >= '0' && pre_ <= '9'){
                if(count_[cnt_] > (COUNT_MAX) / 10)count_[cnt_] = COUNT_MAX;
                else count_[cnt_] = count_[cnt_] * 10;
            }
            else{
                return GenerateMotion(Motion::Line_Start);
            }
        }
        else{
            if(pre_.has_value() && pre_ >= '0' && pre_ <= '9')cnt_++;
            switch (value) {
            case 'h':
                return GenerateMotion(Motion::Left);
                break;
            case 'j':
                return GenerateMotion(Motion::Down);
                break;
            case 'k':
                return GenerateMotion(Motion::Up);
            case 'l':
                return GenerateMotion(Motion::Right);
            case 'w':
                return GenerateMotion(Motion::Next_Word_Start);
            case 'b':
                return GenerateMotion(Motion::Pre_Word_Start);
            case 'e':
                if(pre_ == 'g')return GenerateMotion(Motion::Pre_Word_End);
                return GenerateMotion(Motion::Next_Word_End);
            case 'g':
                if(pre_ == 'g')return GenerateMotion(Motion::First_Line);
                pre_ = 'g';
                return {};
            case 'W':
                return GenerateMotion(Motion::Next_WORD_Start);
            case 'B':
                return GenerateMotion(Motion::Pre_WORD_Start);
            case 'E':
                if(pre_ == 'g')return GenerateMotion(Motion::Pre_WORD_End);
                return GenerateMotion(Motion::Next_WORD_End);
            case '^':
                return GenerateMotion(Motion::Line_First_None_Empty);
            case '$':
                return GenerateMotion(Motion::Line_End);
            case 'G':
                return GenerateMotion(Motion::Last_line);
            case 'i':
                return GenerateCommand(ActionKind::InsertBefore);
                break;
            case 'a':
                return GenerateCommand(ActionKind::InsertAfter);
                break;
            case ':':
                return GenerateCommand(ActionKind::EnterCommandLine);
                break;
            default:
                reset();
                return {};
                break;
            }
        }
        return {};
    }


    //switch (key.code_) {
    //case KeyCode::ArrowLeft:
        //return GenerateMotion(Motion::Left);
    //case KeyCode::ArrowRight:
        //return GenerateMotion(Motion::Right);
    //case KeyCode::ArrowUp:
        //return GenerateMotion(Motion::Up);
    //case KeyCode::ArrowDown:
        //return GenerateMotion(Motion::Down);
    //default:
        //return {};
    //}
    //虽然不要求这些按键,但是我们给你的Terminal.hpp可以处理这些你键盘上的特殊按键并把他们放在了keycode里
    //在Vim中,它们对应着hjkl.

    return {};
}

void NormalModeParser::reset(){
    kind_ = ActionKind::None;
    for(int i = 0; i < 5; i++)count_[i] = 0;
    cnt_ = 0;
    operator_ = std::nullopt;
    motion_ = std::nullopt;
    pre_ = std::nullopt;

}

EditorAction NormalModeParser::Generate(){
    size_t final_count = 1;
    if(cnt_ == 0)final_count = 0;
    else{
        for(int i = 0; i < cnt_; i++){
            final_count = (final_count * count_[i] >= COUNT_MAX ? COUNT_MAX : final_count * count_[i]);
        }
    }
    EditorAction temp_{kind_, operator_, motion_, final_count};
    reset();
    return temp_;

}

EditorAction NormalModeParser::GenerateMotion(Motion motion) {
    if(kind_ == ActionKind::None)kind_ = ActionKind::Move;
    motion_ = motion;
    return Generate();
}

EditorAction NormalModeParser::GenerateCommand(ActionKind kind) {
    return {kind};
}

} // namespace sjtu
