#ifndef DisplayBitcoinPrice_h__
#define DisplayBitcoinPrice_h__

#include "Graphics.h"
#include "Element.h"
#include "TinyTools.h"

#include <string>

class DisplayBitcoinPrice : public eui::Element
{
public:

    DisplayBitcoinPrice(int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS);
    ~DisplayBitcoinPrice();
    virtual bool OnUpdate();

private:
    std::string mLastPrice = "N/A";
    std::string mPriceChange = "N/A";
    std::string mPriceChangePercent = "N/A";

    std::string mLastPriceUSD = "N/A";
    std::string m24HourLowUSD = "N/A";
    std::string m24HourHighUSD = "N/A";

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
