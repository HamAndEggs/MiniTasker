
#include "DisplayBitcoinPrice.h"
#include "TinyJson.h"
#include "FileDownload.h"
#include "style.h"


DisplayBitcoinPrice::DisplayBitcoinPrice(int pBitcoinFont)
{
    this->SetID("bitcoin");
    this->GetStyle().mFont = pBitcoinFont;
    this->SetGrid(1,2);

    UpStyle.mBackground = eui::MakeColour(100,255,100);
    UpStyle.mThickness = BORDER_SIZE;
    UpStyle.mBorder = eui::COLOUR_WHITE;
    UpStyle.mRadius = RECT_RADIUS;
    UpStyle.mForeground = eui::COLOUR_BLACK;

    DownStyle.mBackground = eui::MakeColour(255,100,100);
    DownStyle.mThickness = BORDER_SIZE;
    DownStyle.mBorder = eui::COLOUR_WHITE;
    DownStyle.mRadius = RECT_RADIUS;
    DownStyle.mForeground = eui::COLOUR_BLACK;

    mControls.LastPriceUK = new eui::Element;
        mControls.LastPriceUK->SetPadding(0.05f);
        mControls.LastPriceUK->SetText("£XXXXXX");
        mControls.LastPriceUK->SetPadding(CELL_PADDING);
        mControls.LastPriceUK->SetPos(0,1);
    this->Attach(mControls.LastPriceUK);
}

DisplayBitcoinPrice::~DisplayBitcoinPrice()
{

}

bool DisplayBitcoinPrice::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( dayDisplay )
    {
        UpStyle.mBackground = eui::MakeColour(100,255,100);
        UpStyle.mBorder = eui::COLOUR_WHITE;
        UpStyle.mForeground = eui::COLOUR_BLACK;

        DownStyle.mBackground = eui::MakeColour(255,100,100);
        DownStyle.mBorder = eui::COLOUR_WHITE;
        DownStyle.mForeground = eui::COLOUR_GREY;
    }
    else
    {
        UpStyle.mBackground = eui::COLOUR_NONE;
        UpStyle.mBorder = eui::COLOUR_DARK_GREY;
        UpStyle.mForeground = eui::COLOUR_GREY;

        DownStyle.mBackground = eui::COLOUR_NONE;
        DownStyle.mBorder = eui::COLOUR_DARK_GREY;
        DownStyle.mForeground = eui::COLOUR_GREY;
    }

    if( mPriceChange.find('-') == std::string::npos )
    {
        mControls.LastPriceUK->SetStyle(UpStyle);
    }
    else
    {
        mControls.LastPriceUK->SetStyle(DownStyle);
    }

    mControls.LastPriceUK->SetTextF("£%s",mLastPriceUK.c_str());


    return true;
}

void DisplayBitcoinPrice::UpdateGBP(const std::string& pPrice)
{
    mLastPriceUK = pPrice;
    mPriceGBP = std::stod(pPrice);
}

void DisplayBitcoinPrice::UpdateChange(const std::string& pPrice)
{
    mPriceChange = pPrice;
}


