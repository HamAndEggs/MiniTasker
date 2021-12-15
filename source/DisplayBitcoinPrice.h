#ifndef DisplayBitcoinPrice_h__
#define DisplayBitcoinPrice_h__

#include "TinyGLES.h"

#include <thread>
#include "TinyTools.h"
class DisplayBitcoinPrice
{
public:
    DisplayBitcoinPrice(tinygles::GLES& pGL,const std::string& pFontPath);
    ~DisplayBitcoinPrice();

    void Update(int pX,int pY);

private:
    const uint32_t mFont = 0;
    tinygles::GLES& GL;
    int mLastPrice = 0;
    int mLast24Price = 0;
    tinytools::threading::SleepableThread  mPriceUpdater;

    bool DownloadReport(const std::string& pURL,std::string& rJson)const;
};

#endif //DisplayBitcoinPrice_h__
