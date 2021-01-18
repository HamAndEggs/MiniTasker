#include <ctime>
#include <string>
#include <array>

#include "framebuffer.h"
#include "ClockDisplay.h"

ClockDisplay::ClockDisplay(const std::string& pFontPath):
    mTimeFont( pFontPath + "LiberationSerif-Bold.ttf",160,true),
    mDateFont( pFontPath + "LiberationSerif-Regular.ttf",60,true)
{
    mLastHour = -1;
    mLastMinute = -1;
    mLastWeekDay = -1;
    mLastMonthDay = -1;
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
}

void ClockDisplay::Update(FBIO::FrameBuffer* pFB,int pX,int pY)
{
    DrawTime(pFB,pX,pY);
    DrawDay(pFB,pX + 8,pY + 140);
}

void ClockDisplay::DrawTime(FBIO::FrameBuffer* pFB,int pX,int pY)
{
    std::time_t result = std::time(nullptr);
    tm local_tm = *localtime(&result);

    if( mLastHour != local_tm.tm_hour || mLastMinute != local_tm.tm_min )
    {
        mLastHour = local_tm.tm_hour;
        mLastMinute = local_tm.tm_min;

        pFB->DrawRectangle(pX,pY,pX+380,pY+140,mBG.r,mBG.g,mBG.b,true);

        mTimeFont.Printf(pFB,pX + 4,pY + 120,"%02d:%02d",mLastHour,mLastMinute);
//        pFB->DrawRectangle(pX,pY,pX+380,pY+140,255,0,0,false);
    }
}

void ClockDisplay::DrawDay(FBIO::FrameBuffer* pFB,int pX,int pY)
{
    static const std::array<std::string,7> Days = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

    std::time_t result = std::time(nullptr);
    tm local_tm = *localtime(&result);

    if( mLastWeekDay != local_tm.tm_wday || mLastMonthDay != local_tm.tm_mday )
    {
        mLastWeekDay = local_tm.tm_wday;
        mLastMonthDay = local_tm.tm_mday;

        pFB->DrawRectangle(pX,pY,pX+420,pY+60,mBG.r,mBG.g,mBG.b,true);

        mDateFont.Print(pFB,pX + 4,pY + 44,Days[mLastWeekDay].c_str());
//        pFB->DrawRectangle(pX,pY,pX+420,pY+60,255,0,0,false);

        // Do month day.
        std::string monthDayTag = "th";
        if( mLastMonthDay == 1 || mLastMonthDay == 21 || mLastMonthDay == 31 )
        {
            monthDayTag = "st";
        }
        else if( mLastMonthDay == 2 || mLastMonthDay == 22 )
        {
            monthDayTag = "nd";
        }

        mDateFont.Printf(pFB,pX + 4,pY + 100,"%d%s",mLastMonthDay,monthDayTag.c_str());
    }
}
