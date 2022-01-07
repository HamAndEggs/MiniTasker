#ifndef DisplayBitcoinPrice_h__
#define DisplayBitcoinPrice_h__

#include "TinyGLES.h"

#include <thread>
#include "TinyTools.h"
class DisplayBitcoinPrice
{
public:
    DisplayBitcoinPrice(tinygles::GLES& pGL,const std::string& pPath);
    ~DisplayBitcoinPrice();

    void Update(int pX,int pY);

private:
    const uint32_t mFont = 0;
    tinygles::GLES& GL;
    int mLastPrice = 0;
    int mPriceChange = 0;
    int m24HourLow = 0;
    int m24HourHigh = 0;
    tinytools::threading::SleepableThread  mPriceUpdater;

    uint32_t mIconDownArrow;
    uint32_t mIconUpArrow;
    uint32_t mIconGBP;

    bool DownloadReport(const std::string& pURL,std::string& rJson)const;
    uint32_t LoadIcon(const std::string& pName);
};

#endif //DisplayBitcoinPrice_h__
