#ifndef DisplayTideData_h
#define DisplayTideData_h

#include "Graphics.h"
#include "Element.h"
#include "TinyTools.h"

class DisplayTideData : public eui::Element
{
public:
    DisplayTideData(int pFont);
    ~DisplayTideData();
    
    virtual bool OnUpdate(const eui::Rectangle& pContentRect);

private:
    bool mLoaded = false;
    uint32_t mPortsmouthEngland = 0;
    tinytools::threading::SleepableThread  mTideUpdater;
    bool mLowTideFirst = false;
    eui::ElementPtr mHighTide = nullptr;
    eui::ElementPtr mLowTide = nullptr;


    bool DownloadTideData(const std::string& pURL,std::string& rJson)const;
};

#endif //#ifndef DisplayTideData_h
