#ifndef DisplayBitcoinPrice_h__
#define DisplayBitcoinPrice_h__

#include "Graphics.h"
#include "Element.h"
#include "TinyTools.h"

#include <string>

class DisplayBitcoinPrice : public eui::Element
{
public:

    DisplayBitcoinPrice(int pBitcoinFont);
    ~DisplayBitcoinPrice();
    virtual bool OnUpdate(const eui::Rectangle& pContentRect);

    const eui::Style &GetUpStyle()const{return UpStyle;}
    const eui::Style &GetDownStyle()const{return DownStyle;}

    const double GetPriceGBP()const{return mPriceGBP;}

    void UpdateGBP(const std::string& pPrice);
    void UpdateUSD(const std::string& pPrice);
    void UpdateChange(const std::string& pPrice);

private:
    double mPriceGBP = 0.0;

    std::string mLastPriceUK = "N/A";
    std::string mLastPriceUSD = "N/A";

    std::string mPriceChange = "N/A";

    eui::Style UpStyle,DownStyle;

    struct
    {
        eui::ElementPtr LastPriceUK;
        eui::ElementPtr LastPriceUSD;
    }mControls;

};

#endif //DisplayBitcoinPrice_h__
