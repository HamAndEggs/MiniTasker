
#include "DisplayBitcoinPrice.h"
#include "TinyJson.h"
#include "FileDownload.h"


DisplayBitcoinPrice::DisplayBitcoinPrice(int pBitcoinFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{
    this->SetID("bitcoin");
    this->GetStyle().mFont = pBitcoinFont;

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
        mControls.LastPriceUK->SetPos(0,0);
    this->Attach(mControls.LastPriceUK);

    mControls.LastPriceUSD = new eui::Element;
        mControls.LastPriceUSD->SetPadding(0.05f);
        mControls.LastPriceUSD->SetText("£XXXXXX");
        mControls.LastPriceUSD->SetPadding(CELL_PADDING);
        mControls.LastPriceUSD->SetPos(1,0);
    this->Attach(mControls.LastPriceUSD);
}

DisplayBitcoinPrice::~DisplayBitcoinPrice()
{

}

bool DisplayBitcoinPrice::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( mPriceChange.find('-') == std::string::npos )
    {
        mControls.LastPriceUK->SetStyle(UpStyle);
        mControls.LastPriceUSD->SetStyle(UpStyle);
    }
    else
    {
        mControls.LastPriceUK->SetStyle(DownStyle);
        mControls.LastPriceUSD->SetStyle(DownStyle);
    }

    mControls.LastPriceUK->SetTextF("£%s",mLastPriceUK.c_str());
    mControls.LastPriceUSD->SetTextF("$%s",mLastPriceUSD.c_str());

    return true;
}

void DisplayBitcoinPrice::UpdateGBP(const std::string& pPrice)
{
    mLastPriceUK = pPrice;
    mPriceGBP = std::stod(pPrice);
}

void DisplayBitcoinPrice::UpdateUSD(const std::string& pPrice)
{
    mLastPriceUSD = pPrice;
}

void DisplayBitcoinPrice::UpdateChange(const std::string& pPrice)
{
    mPriceChange = pPrice;
}


