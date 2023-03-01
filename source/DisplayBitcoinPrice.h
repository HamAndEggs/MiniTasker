#ifndef DisplayBitcoinPrice_h__
#define DisplayBitcoinPrice_h__

#include "Graphics.h"
#include "Element.h"
#include "TinyTools.h"

#include <string>

class DisplayBitcoinPrice : public eui::Element
{
public:

    DisplayBitcoinPrice(int pBitcoinFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS);
    ~DisplayBitcoinPrice();
    virtual bool OnUpdate(const eui::Rectangle& pContentRect);

    const eui::Style &GetUpStyle()const{return UpStyle;}
    const eui::Style &GetDownStyle()const{return DownStyle;}

    const double GetPriceGBP()const{return mPriceGBP;}

private:
    double mPriceGBP = 0.0;

    std::string mLastPriceUK = "N/A";
    std::string mLastPriceUSD = "N/A";

    std::string mPriceChange = "N/A";
    std::string mPriceChangePercent = "N/A";

    eui::Style UpStyle,DownStyle;

    struct
    {
        eui::ElementPtr LastPriceUK;
        eui::ElementPtr LastPriceUSD;
        eui::ElementPtr PriceChangePercent;
    }mControls;

    tinytools::threading::SleepableThread  mPriceUpdater;

    bool DownloadReport(const std::string& pURL,std::string& rJson)const;

};

#endif //DisplayBitcoinPrice_h__
