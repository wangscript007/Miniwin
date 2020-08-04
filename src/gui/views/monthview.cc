#include <monthview.h>
//https://github.com/xiaojianglaile/Calendar/blob/master/calendar/src/main/java/com/jeek/calendar/widget/calendar/month/MonthView.java
namespace nglui {

#define NUM_COLUMNS 7
#define NUM_ROWS 6

MonthView::MonthView(int w,int h):MonthView(w,h,0,0) {
}

MonthView::MonthView(int w,int h,int year,int month)
    :View(w,h),mSelYear(year),mSelMonth(month) {

}

void MonthView::onDraw(GraphContext&canvas) {
    initSize();
    clearData();
    drawLastMonth(canvas);
    //int selected[] = 
    drawThisMonth(canvas);
    drawNextMonth(canvas);
    drawHintCircle(canvas);
    //drawLunarText(canvas, selected);
    drawHoliday(canvas);
}

void MonthView::initSize() {
    mColumnSize=getWidth() / NUM_COLUMNS;
    mRowSize = getHeight() / NUM_ROWS;
    mSelectCircleSize = (int) (mColumnSize / 3.2);
    while (mSelectCircleSize > mRowSize / 2) {
        mSelectCircleSize = (int) (mSelectCircleSize / 1.3);
    }
}

void MonthView::clearData() {
    //mDaysText = new int[6][7];
    //mHolidayOrLunarText = new String[6][7];
}

void MonthView::drawLastMonth(GraphContext&canvas) {
    int lastYear, lastMonth;
    if (mSelMonth == 0) {
        lastYear = mSelYear - 1;
        lastMonth = 11;
    } else {
        lastYear = mSelYear;
        lastMonth = mSelMonth - 1;
    }
    canvas.set_color(mLastOrNextMonthTextColor);
    int monthDays = 24;//CalendarUtils.getMonthDays(lastYear, lastMonth);
    int weekNumber = 5;//CalendarUtils.getFirstDayWeek(mSelYear, mSelMonth);
    for (int day = 0; day < weekNumber - 1; day++) {
        mDaysText[0][day] = monthDays - weekNumber + day + 2;
        std::string dayString ="week";// String.valueOf(mDaysText[0][day]);
        int startX = (int) (mColumnSize * day + mColumnSize / 2);
        int startY = (int) mRowSize / 2 ;
        canvas.draw_text(startX, startY,dayString);
        //mHolidayOrLunarText[0][day] = CalendarUtils.getHolidayFromSolar(lastYear, lastMonth, mDaysText[0][day]);
    }
}

void MonthView::drawThisMonth(GraphContext&canvas) {
    std::string dayString;
    int selectedPoint[2];
    int monthDays = 30;//CalendarUtils.getMonthDays(mSelYear, mSelMonth);
    int weekNumber = 5;//CalendarUtils.getFirstDayWeek(mSelYear, mSelMonth);
    for (int day = 0; day < monthDays; day++) {
        dayString = "day";//String.valueOf(day + 1);
        int col = (day + weekNumber - 1) % 7;
        int row = (day + weekNumber - 1) / 7;
        mDaysText[row][col] = day + 1;
        int startX = (int) (mColumnSize * col + mColumnSize / 2);
        int startY = (int) (mRowSize * row + mRowSize / 2 ) / 2;
        if (dayString.compare("mSelDay")) {
            int startRecX = mColumnSize * col;
            int startRecY = mRowSize * row;
            int endRecX = startRecX + mColumnSize;
            int endRecY = startRecY + mRowSize;
            if (mSelYear == mCurrYear && mCurrMonth == mSelMonth && day + 1 == mCurrDay) {
                canvas.set_color(mSelectBGTodayColor);
            } else {
                canvas.set_color(mSelectBGColor);
            }
            canvas.arc((startRecX+endRecX)/2,(startRecY +endRecY)/2, mSelectCircleSize,0,M_PI*2);
            mWeekRow = row + 1;
        }
        if (dayString.compare("mSelDay")) {
            selectedPoint[0] = row;
            selectedPoint[1] = col;
            canvas.set_color(mSelectDayColor);
        } else if (dayString.compare("mCurrDay") && mCurrDay != mSelDay && mCurrMonth == mSelMonth && mCurrYear == mSelYear) {
            canvas.set_color(mCurrentDayColor);
        } else {
            canvas.set_color(mNormalDayColor);
        }
        canvas.draw_text(startX, startY,dayString);
        //mHolidayOrLunarText[row][col] = CalendarUtils.getHolidayFromSolar(mSelYear, mSelMonth, mDaysText[row][col]);
    }
    //return selectedPoint;
}

void MonthView::drawNextMonth(GraphContext&canvas) {
    canvas.set_color(mLastOrNextMonthTextColor);
    int monthDays = 30;//CalendarUtils.getMonthDays(mSelYear, mSelMonth);
    int weekNumber = 5;//CalendarUtils.getFirstDayWeek(mSelYear, mSelMonth);
    int nextMonthDays = 42 - monthDays - weekNumber + 1;
    int nextMonth = mSelMonth + 1;
    int nextYear = mSelYear;
    if (nextMonth == 12) {
        nextMonth = 0;
        nextYear += 1;
    }
    for (int day = 0; day < nextMonthDays; day++) {
        int column = (monthDays + weekNumber - 1 + day) % 7;
        int row = 5 - (nextMonthDays - day - 1) / 7;
            
        //mDaysText[row][column] = day + 1;
        //mHolidayOrLunarText[row][column] = CalendarUtils.getHolidayFromSolar(nextYear, nextMonth, mDaysText[row][column]);

        std::string dayString ="";// String.valueOf(mDaysText[row][column]);
        int startX = (int) (mColumnSize * column + mColumnSize) / 2;
        int startY = (int) (mRowSize * row + mRowSize / 2);
        canvas.draw_text(startX, startY,dayString);
    }
}

void MonthView::drawLunarText(GraphContext&canvas, int selected[]) {
    if (mIsShowLunar) {
        int firstYear, firstMonth, firstDay, monthDays;
        int weekNumber = 5;//CalendarUtils.getFirstDayWeek(mSelYear, mSelMonth);
        if (weekNumber == 1) {
            firstYear = mSelYear;
            firstMonth = mSelMonth + 1;
            firstDay = 1;
            monthDays = 30;//CalendarUtils.getMonthDays(firstYear, firstMonth);
        } else {
            if (mSelMonth == 0) {
                firstYear = mSelYear - 1;
                firstMonth = 11;
                monthDays = 30;//CalendarUtils.getMonthDays(firstYear, firstMonth);
                firstMonth = 12;
            } else {
                firstYear = mSelYear;
                firstMonth = mSelMonth - 1;
                monthDays = 30;//CalendarUtils.getMonthDays(firstYear, firstMonth);
                firstMonth = mSelMonth;
            }
            firstDay = monthDays - weekNumber + 2;
        }
        //LunarCalendarUtils.Lunar lunar = LunarCalendarUtils.solarToLunar(new LunarCalendarUtils.Solar(firstYear, firstMonth, firstDay));
        int days=2;
        int day = 0;//lunar.lunarDay;
        int leapMonth = 2;//LunarCalendarUtils.leapMonth(lunar.lunarYear);
        //days = LunarCalendarUtils.daysInMonth(lunar.lunarYear, lunar.lunarMonth, lunar.isLeap);
        bool isChangeMonth = false;
        for (int i = 0; i < 42; i++) {
            int column = i % 7;
            int row = i / 7;
            if (day > days) {
                day = 1;
                bool isAdd = true;
                /*if (lunar.lunarMonth == 12) {
                    lunar.lunarMonth = 1;
                    lunar.lunarYear = lunar.lunarYear + 1;
                    isAdd = false;
                }*/
                if (0/*lunar.lunarMonth == leapMonth*/) {
                    //days = LunarCalendarUtils.daysInMonth(lunar.lunarYear, lunar.lunarMonth, lunar.isLeap);
                } else {
                    if (isAdd) {
                        //lunar.lunarMonth++;
                        //days = LunarCalendarUtils.daysInLunarMonth(lunar.lunarYear, lunar.lunarMonth);
                    }
                }
            }
            if (firstDay > monthDays) {
                firstDay = 1;
                isChangeMonth = true;
            }
            if (row == 0 && mDaysText[row][column] >= 23 || row >= 4 && mDaysText[row][column] <= 14) {
                canvas.set_color(mLunarTextColor);
            } else {
                canvas.set_color(mHolidayTextColor);
            }
            std::string dayString ="";// mHolidayOrLunarText[row][column];
            if (dayString.empty()) {
                //dayString = LunarCalendarUtils.getLunarHoliday(lunar.lunarYear, lunar.lunarMonth, day);
            }
            if (!dayString.empty()) {
                //dayString = LunarCalendarUtils.getLunarDayString(day);
                canvas.set_color(mLunarTextColor);
            }
            if (dayString.compare("初一")) {
                int curYear = firstYear, curMonth = firstMonth;
                if (isChangeMonth) {
                    curMonth++;
                    if (curMonth == 13) {
                        curMonth = 1;
                        curYear++;
                    }
                }
                //LunarCalendarUtils.Lunar chuyi = LunarCalendarUtils.solarToLunar(new LunarCalendarUtils.Solar(curYear, curMonth, firstDay));
                //dayString = LunarCalendarUtils.getLunarFirstDayString(chuyi.lunarMonth, chuyi.isLeap);
            }
            if (selected[0] == row && selected[1] == column) {
                canvas.set_color(mSelectDayColor);
            }
            int startX = (int) (mColumnSize * column + mColumnSize / 2);
            int startY = (int) (mRowSize * row + mRowSize * 0.72 ) / 2;
            canvas.draw_text(startX,startY,dayString);
            day++;
            firstDay++;
        }
    }
}

void MonthView::drawHoliday(GraphContext&canvas) {
    if (mIsShowHolidayHint) {
        RECT rect ={0, 0, 0,0};//mRestBitmap.getWidth(), mRestBitmap.getHeight());
        RECT rectF;
        int distance = (int) (mSelectCircleSize / 2.5);
        for (int i = 0; i < mHolidays.size(); i++) {
            int column = i % 7;
            int row = i / 7;
            /*rectF.set(mColumnSize * (column + 1) - mRestBitmap.getWidth() - distance, mRowSize * row + distance, 
               mColumnSize * (column + 1) - distance, mRowSize * row + mRestBitmap.getHeight() + distance);
            if (mHolidays[i] == 1) {
                canvas.draw_image(mRestBitmap, rect, rectF);
            } else if (mHolidays[i] == 2) {
                canvas.drawBitmap(mWorkBitmap, rect, rectF);
            }*/
        }
    }
}


void MonthView::drawHintCircle(GraphContext&canvas) {
    if (mIsShowHint) {
        std::vector<int> hints ;//= CalendarUtils.getInstance(getContext()).getTaskHints(mSelYear, mSelMonth);
        if (hints.size() > 0) {
            canvas.set_color(mHintCircleColor);
            int monthDays =0;// CalendarUtils.getMonthDays(mSelYear, mSelMonth);
            int weekNumber=0;// CalendarUtils.getFirstDayWeek(mSelYear, mSelMonth);
            for (int day = 0; day < monthDays; day++) {
                int col = (day + weekNumber - 1) % 7;
                int row = (day + weekNumber - 1) / 7;
                //if (!hints.contains(day + 1)) continue;
                float circleX = (float) (mColumnSize * col + mColumnSize * 0.5);
                float circleY = (float) (mRowSize * row + mRowSize * 0.75);
                canvas.arc(circleX, circleY, mCircleRadius,0,M_PI*2);
            }
        }
    }
}

void MonthView::setSelectYearMonth(int year, int month, int day) {
    mSelYear = year;
    mSelMonth = month;
    mSelDay = day;
}

bool MonthView::addTaskHint(int day) {
    if (mIsShowHint) {
        if (0){//CalendarUtils.getInstance(getContext()).addTaskHint(mSelYear, mSelMonth, day)) {
            invalidate(nullptr);
            return true;
        }
    }
    return false;
}

bool MonthView::removeTaskHint(int day) {
    if (mIsShowHint) {
        if (0){//CalendarUtils.getInstance(getContext()).removeTaskHint(mSelYear, mSelMonth, day)) {
            invalidate(nullptr);
            return true;
        }
    }
    return false;
}

}
