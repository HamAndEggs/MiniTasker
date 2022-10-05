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
    virtual bool OnUpdate();

    const eui::Style &GetUpStyle()const{return UpStyle;}
    const eui::Style &GetDownStyle()const{return DownStyle;}

    const double GetPriceGBP()const{return mPriceGBP;}

private:
    double mPriceGBP = 0.0;

    std::string mLastPrice = "N/A";
    std::string mPriceChange = "N/A";
    std::string mPriceChangePercent = "N/A";

    std::string mLastPriceUSD = "N/A";
    std::string m24HourLowUSD = "N/A";
    std::string m24HourHighUSD = "N/A";

    eui::Style UpStyle,DownStyle;

    struct
    {
        eui::ElementPtr LastPrice;
        eui::ElementPtr PriceChange;
        eui::ElementPtr PriceChangePercent;

        eui::ElementPtr LastPriceUSD;
        eui::ElementPtr LowUSD;
        eui::ElementPtr HighUSD;

    }mControls;

    tinytools::threading::SleepableThread  mPriceUpdater;

    bool DownloadReport(const std::string& pURL,std::string& rJson)const;

};

#endif //DisplayBitcoinPrice_h__
