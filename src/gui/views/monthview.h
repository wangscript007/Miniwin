#ifndef __MONTH_VIEW_H__
#define __MONTH_VIEW_H__

#include <view.h>
#include <string>
#include <vector>

namespace nglui{

class MonthView:public View{
private :
    int mNormalDayColor;
    int mSelectDayColor;
    int mSelectBGColor;
    int mSelectBGTodayColor;
    int mCurrentDayColor;
    int mHintCircleColor;
    int mLunarTextColor;
    int mHolidayTextColor;
    int mLastOrNextMonthTextColor;
    int mCurrYear, mCurrMonth, mCurrDay;
    int mSelYear, mSelMonth, mSelDay;
    int mColumnSize, mRowSize, mSelectCircleSize;
    int mDaySize;
    int mLunarTextSize;
    int mWeekRow; // 当前月份第几周
    int mCircleRadius = 6;
    std::vector<std::string>mDaysText;
    std::vector<std::string>mHolidays;
    std::vector<std::string>mHolidayOrLunarText;
    bool mIsShowLunar;
    bool mIsShowHint;
    bool mIsShowHolidayHint;
private:
    void initSize();
    void clearData();
    void drawLastMonth(GraphContext&canvas);
    void drawThisMonth(GraphContext&canvas);
    void drawNextMonth(GraphContext&canvas);
    void drawLunarText(GraphContext&canvas, int*selected);
    void drawHoliday(GraphContext&canvas);
    void drawHintCircle(GraphContext&canvas);
    bool addTaskHint(int day);
    bool removeTaskHint(int day);
public:
    MonthView(int w,int h);
    MonthView(int w,int h,int year,int month);
    void onDraw(GraphContext&canvas);
    void setSelectYearMonth(int year, int month, int day);
    int getSelectYear() {
        return mSelYear;
    }
    int getSelectMonth() {
        return mSelMonth;
    }
    int getSelectDay() {
        return mSelDay;
    }
    int getRowSize() {
        return mRowSize;
    }
    int getWeekRow() {
        return mWeekRow;
    }
};

}
#endif
