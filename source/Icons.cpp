#include "Icons.h"

Icons::Icons(const std::string& pIconFolder)
{
    const int size = 100;
    const int rounded = 15;
    const int padding = 2;
    mIconBG.Resize(size,size,true);
    mIconBG.Clear(0,0,0,0);
    mIconBG.DrawRoundedRectangle(padding,padding,size-padding-1,size-padding-1,rounded,0,0,0,100,true);
    mIconBG.DrawRoundedRectangle(padding,padding,size-padding-1,size-padding-1,rounded,255,255,255);

    // Load the images.
    const std::vector<std::string> files =
    {
        "01d",
        "01n",
        "02d",
        "02n",
        "03d",
        "03n",
        "04d",
        "04n",
        "09d",
        "09n",
        "10d",
        "10n",
        "11d",
        "11n",
        "13d",
        "13n",
        "50d",
        "50n",
    }; 

    tinypng::Loader bg(true);
    for( std::string f : files )
    {
        if( bg.LoadFromFile(pIconFolder + f + "@2x.png") )
        {
            BuildIcon(bg,f);
        }
    }
}

const tiny2d::DrawBuffer& Icons::GetIcon(const std::string& pName)const
{
    auto found = mIcons.find(pName);
    if( found != mIcons.end() )
        return found->second;

    return mIconBG;
}

void Icons::BuildIcon(tinypng::Loader& bg,const std::string pName)
{
    tiny2d::DrawBuffer& icon = mIcons[pName];
    icon.Resize(bg.GetWidth(),bg.GetHeight(),bg.GetHasAlpha());
    if( bg.GetHasAlpha() )
    {
        bg.GetRGBA(icon.mPixels);
    }
    else
    {
        bg.GetRGB(icon.mPixels);
    }
}
