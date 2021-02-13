#include <ctime>
#include <string>
#include <array>

#include "framebuffer.h"
#include "ClockDisplay.h"

ClockDisplay::ClockDisplay(const std::string& pFontPath):
    mTimeFont( pFontPath + "LiberationSerif-Bold.ttf",160,true),
    mDateFont( pFontPath + "LiberationSerif-Regular.ttf",60,true)
{
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
    pFB->DrawRoundedRectangle(pX-8,pY-8,pX + 400,pY + 300,35,255,255,255,true);
    pFB->DrawRoundedRectangle(pX-4,pY-4,pX + 400-4,pY + 300-4,32,0,0,0,true);

    DrawTime(pFB,pX,pY);
    DrawDay(pFB,pX + 8,pY + 140);
}

void ClockDisplay::DrawTime(FBIO::FrameBuffer* pFB,int pX,int pY)
{
    std::time_t result = std::time(nullptr);
    tm local_tm = *localtime(&result);

    mTimeFont.Printf(pFB,pX + 4,pY + 120,"%02d:%02d",local_tm.tm_hour,local_tm.tm_min);
}

void ClockDisplay::DrawDay(FBIO::FrameBuffer* pFB,int pX,int pY)
{
    static const std::array<std::string,7> Days = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

    std::time_t result = std::time(nullptr);
    tm local_tm = *localtime(&result);

    mDateFont.Print(pFB,pX + 4,pY + 44,Days[local_tm.tm_wday].c_str());

    // Do month day.
    std::string monthDayTag = "th";
    if( local_tm.tm_mday == 1 || local_tm.tm_mday == 21 || local_tm.tm_mday == 31 )
    {
        monthDayTag = "st";
    }
    else if( local_tm.tm_mday == 2 || local_tm.tm_mday == 22 )
    {
        monthDayTag = "nd";
    }

    mDateFont.Printf(pFB,pX + 4,pY + 100,"%d%s",local_tm.tm_mday,monthDayTag.c_str());
}
