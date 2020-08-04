#include<button.h>

namespace nglui{
Button::Button(const std::string& text)
  : Button(text, text.size() * getFontSize(), (getFontSize() * 3)){
}

Button::Button(const std::string& text, int32_t w, int32_t h)
  : INHERITED(text, w, h), state_(Button::State::STATE_NORMAL), style_(Button::Style::STYLE_STANDARD){
    mTextAlignment=DT_CENTER|DT_VCENTER;
    setFlag(FOCUSABLE|ENABLED);
    if (w == 0 && h == 0) {
        setBound(0, 0, text.size() * getFontSize(), (getFontSize() * 3));
    } else if (w != 0 && h == 0) {
        setBound(0, 0, w, (getFontSize() * 3));
    } else if (w == 0 && h != 0) {
        setBound(0, 0, text.size() * getFontSize(), h);
    } else {
        setBound(0, 0, w, h);
    }
}

Button::~Button() {
}

Button::State Button::getState() const {
    return state_;
}

Button::Style Button::getStyle() const {
    return style_;
}

void Button::onDraw(GraphContext& canvas) {
    if ((getWidth() == 0) || (getHeight() == 0)) {
        return;
    }

    int x = 0;//getX();
    int y = 0;//getY();
    int w = getWidth();
    int h = getHeight();
    RECT rect;
    // Draw the shadow
    canvas.set_color(0xFFB0B0B0);
    canvas.rectangle(x+1,y,w,h);
    canvas.stroke();
    // Draw the button
    if (state_ == Button::STATE_PRESSED||isFocused()) {
        // Pressed
        canvas.set_color(0xFF909090);
    } else {
        // Normal
        canvas.set_color(0xFFE0E0E0);
    }
    canvas.rectangle(getClientRect());
    canvas.fill();

    // Text label
    //p.setFlags(SkPaint::kAntiAlias_Flag| SkPaint::kSubpixelText_Flag);
    if (state_ == Button::STATE_PRESSED) {
        // Pressed
        canvas.set_color(0xFFF0F0F0);
    } else {
        // Normal
        canvas.set_color(0xFFFFFFFF);
    }
    canvas.set_font_size(getFontSize());
    canvas.save();
    canvas.draw_text(getClientRect(),mText,getAlignment());
    canvas.restore();
}

#if 0
bool Button::onMousePress(MotionEvent& evt) {
    state_ = Button::STATE_PRESSED;
    // Notify the bound to redraw
    invalidate(&bound_);
    return true;
}

bool Button::onMouseRelease(MotionEvent& evt) {
    state_ = Button::STATE_NORMAL;
    // Notify the bound to redraw
    invalidate(&bound_);
    return true;
}

bool Button::onMouseMotion(MotionEvent& evt) {
    return true;
}
#endif

const SIZE& Button::getPreferSize() {
    mPreferSize.set(getWidth() + 3, getHeight() + 3);
    return mPreferSize;
}
 

}//endof namespace

