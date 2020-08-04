#ifndef UI_LIBUI_LINEARLAYOUT_H_
#define UI_LIBUI_LINEARLAYOUT_H_

#include "layout.h"

namespace nglui {

class LinearLayout : public Layout {
private:
   int direct;
public:
   static constexpr int32_t DELIMITER = 5;

   LinearLayout();

   virtual ~LinearLayout();

   virtual void onLayout(View* view) override;
};

}  // namespace ui
#endif 

