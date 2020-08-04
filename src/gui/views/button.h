#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__
#include<widget.h>

namespace nglui{
class Button : public Widget {
 public:

  enum State {
    STATE_NORMAL = 0,
    STATE_HOVERED,
    STATE_PRESSED,
    STATE_DISABLED,
    STATE_COUNT,
  };
  enum Style {
    STYLE_STANDARD = 0,
    STYLE_TEXT,
  };

 public:
  explicit Button(const std::string& text);
  explicit Button(const std::string& text, int w, int h);
  virtual ~Button();

  virtual State getState() const;
  virtual Style getStyle() const;

  virtual void onDraw(GraphContext& canvas) override;

  virtual const SIZE& getPreferSize() override;
 protected:
  State state_;
  Style style_;
 private:
  typedef Widget INHERITED;
  DISALLOW_COPY_AND_ASSIGN(Button);
};
}
#endif
