
#include <string>
#include <array>

#include "ClockDisplay.h"

ClockDisplay::ClockDisplay(const std::string& pFontPath):
    mTimeFont( pFontPath + "LiberationSerif-Bold.ttf",160),
    mDateFont( pFontPath + "LiberationSerif-Regular.ttf",60),
    mTemperatureFont( pFontPath + "LiberationSerif-Regular.ttf",60)
{
    mTemperatureFont.SetPenColour(140,140,200);
}

ClockDisplay::~ClockDisplay()
{

}

void ClockDisplay::SetForground(uint8_t pR,uint8_t pG,uint8_t pB)
{
    mFG.r = pR;
    mFG.g = pG;
    mFG.b = pB;
    mTimeFont.SetPenColour(pR,pG,pB);
    mDateFont.SetPenColour(pR,pG,pB);
}

void ClockDisplay::SetBackground(uint8_t pR,uint8_t pG,uint8_t pB)
{
    mBG.r = pR;
    mBG.g = pG;
    mBG.b = pB;
    mTimeFont.SetBackgroundColour(pR,pG,pB);
    mDateFont.SetBackgroundColour(pR,pG,pB);
    mTemperatureFont.SetBackgroundColour(pR,pG,pB);
}

void ClockDisplay::Update(tiny2d::FrameBuffer* pFB,int pX,int pY,const tm& pCurrentTime,const std::string& pCurrentTemperature)
{
    pFB->DrawRoundedRectangle(pX-8,pY-8,pX + 400,pY + 300,35,255,255,255,true);
    pFB->DrawRoundedRectangle(pX-4,pY-4,pX + 400-4,pY + 300-4,32,0,0,0,true);

    DrawTime(pFB,pX,pY,pCurrentTime.tm_hour,pCurrentTime.tm_min);
    DrawDay(pFB,pX + 8,pY + 140,pCurrentTime.tm_wday,pCurrentTime.tm_mday);
    // Draw temperature, if we have one.
    if( pCurrentTemperature.size() > 0 )
    {
        mTemperatureFont.Print(pFB,pX + 200,pY + 260,pCurrentTemperature.c_str());
    }
}

void ClockDisplay::DrawTime(tiny2d::FrameBuffer* pFB,int pX,int pY,int pHour,int pMinute)
{
    mTimeFont.Printf(pFB,pX + 4,pY + 120,"%02d:%02d",pHour,pMinute);
}

void ClockDisplay::DrawDay(tiny2d::FrameBuffer* pFB,int pX,int pY,int pWeekDay,int pMonthDay)
{
    static const std::array<std::string,7> Days = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

    mDateFont.Print(pFB,pX + 4,pY + 44,Days[pWeekDay].c_str());

    // Do month day.
    std::string monthDayTag = "th";
    if( pMonthDay == 1 || pMonthDay == 21 || pMonthDay == 31 )
    {
        monthDayTag = "st";
    }
    else if( pMonthDay == 2 || pMonthDay == 22 )
    {
        monthDayTag = "nd";
    }
    else if( pMonthDay == 3 || pMonthDay == 23 )
    {
        monthDayTag = "rd";
    }

    mDateFont.Printf(pFB,pX + 4,pY + 120,"%d%s",pMonthDay,monthDayTag.c_str());
}
