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
    int mLastPrice = 0;
    int mPriceChange = 0;
    int m24HourLow = 0;
    int m24HourHigh = 0;

    struct
    {
        eui::ElementPtr LastPrice;
        eui::ElementPtr PriceChange;
        eui::ElementPtr Low;
        eui::ElementPtr High;
    }mControls;

    tinytools::threading::SleepableThread  mPriceUpdater;

    bool DownloadReport(const std::string& pURL,std::string& rJson)const;

};

#endif //DisplayBitcoinPrice_h__
