#include "Command.hpp"

namespace sjtu {

EditorAction NormalModeParser::Feed(KeyEvent key) {
    if (key.code_ == KeyCode::Escape) {
        reset();
        return {};
    }
    if (key.code_ == KeyCode::Character) {
        auto value = key.value_;
        if(value >= '1' && value <= '9'){
            size_t digit = value - '0';
            if (count_[cnt_] > COUNT_MAX / 10) {
                count_[cnt_] = COUNT_MAX;
            } else {
                count_[cnt_] = count_[cnt_] * 10 + digit;
            }
            last_digit = value;
        }
        else if(value == '0'){
            if(last_digit.has_value()){
                if (count_[cnt_] > COUNT_MAX / 10) {
                    count_[cnt_] = COUNT_MAX;
                } else {
                    count_[cnt_] = count_[cnt_] * 10;
                }
                last_digit = '0';
            }
            else{
                return GenerateMotion(Motion::Line_Start);
            }
        }
        else{
            if(last_digit.has_value()){
                cnt_++;
                last_digit.reset();
            }
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
                if(pending_seq == 'g'){
                    pending_seq.reset();
                    return GenerateMotion(Motion::Pre_Word_End);
                }
                return GenerateMotion(Motion::Next_Word_End);
            case 'g':
                if(pending_seq == 'g'){
                    pending_seq.reset();
                    return GenerateMotion(Motion::First_Line);
                }
                else{
                    pending_seq = 'g';
                    return {};
                }
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
            case 'a':
                return GenerateCommand(ActionKind::InsertAfter);
            case ':':
                return GenerateCommand(ActionKind::EnterCommandLine);
            case '/':
                return GenerateSearch(Search::SearchForward);
            default:
                reset();
                return {};
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
    operator_.reset();
    motion_.reset();
    last_digit.reset();
    pending_seq.reset();
    search_.reset();

}

EditorAction NormalModeParser::Generate(){
    size_t final_count = 1;
    for (size_t i = 0; i < cnt_; ++i) {
        if (final_count > COUNT_MAX / count_[i]){
            final_count = COUNT_MAX;
            break;
        }
        final_count *= count_[i];
    }
    EditorAction temp_;
    std::optional<size_t> res = {};
    if(cnt_ != 0){
        res = final_count;
    }
    temp_ = {kind_, operator_, motion_, res, {}, search_};
    reset();
    return temp_;

}

EditorAction NormalModeParser::GenerateMotion(Motion motion) {
    if(kind_ == ActionKind::None)kind_ = ActionKind::Move;
    motion_ = motion;
    return Generate();
}

EditorAction NormalModeParser::GenerateCommand(ActionKind kind) {
    EditorAction act{kind};
    reset();
    return act;
}

EditorAction NormalModeParser::GenerateSearch(Search search) {
    if(kind_ == ActionKind::None)kind_ = ActionKind::Search;
    search_ = search;
    return Generate();
}

} // namespace sjtu
