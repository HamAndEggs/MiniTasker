#include "Icons.h"
#include "TheWeather.h"

static const int ICON_WIDTH = 160;


Icons::Icons(const std::string& pPath) : 
    mIconFont(pPath + "liberation_serif_font/LiberationSerif-Bold.ttf",30),
    mTemperatureFont( pPath + "liberation_serif_font/LiberationSerif-Regular.ttf",60)
{
    mTemperatureFont.SetPenColour(0,0,0);
    
    const int sizeY = 160;
    const int rounded = 15;
    const int padding = 2;
    mIconBG.Resize(ICON_WIDTH,sizeY,true);
    mIconBG.Clear(0,0,0,0);
    mIconBG.FillRoundedRectangle(padding,padding,ICON_WIDTH-padding-1,sizeY-padding-1,rounded,0,0,0,200);
    mIconBG.DrawRoundedRectangle(padding,padding,ICON_WIDTH-padding-1,sizeY-padding-1,rounded,255,255,255);
    

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

    tinypng::Loader bg;
    for( std::string f : files )
    {
        if( bg.LoadFromFile(pPath + "icons/" + f + ".png") )
        {
            BuildIcon(bg,f);
        }
    }
}

void Icons::RenderWeatherForcast(tiny2d::DrawBuffer& RT,int pY,const tm& pCurrentTime,const TheWeather& pWeather)
{
     // Show next six icons.
    // I could render this to an offscreen image and only update once an hour.
    // But for now, render each time.
    const getweather::HourlyIconVector icons = pWeather.GetNextHourlyIcons();
    int n = 0;
    int x = (RT.GetWidth()/2) - ((ICON_WIDTH*6) / 2);
    int y = pY;
    for( const auto& icon : icons )
    {
        RT.Blend(GetIconBG(),x,y);
        RT.Blend(GetIcon(icon.second),x-20,y-10);

        // https://www.npl.co.uk/resources/q-a/is-midnight-12am-or-12pm
        std::string hour; 
        if( icon.first == 0 )
        {// Special case zero houndred, IE 12am...
            hour = "Midnight";
        }
        else if( icon.first < 12 )
        {
            hour = std::to_string(icon.first) + "am";
        }
        else if( icon.first == 12 )
        {// Special case 12 houndred, IE 12pm, or am, or pm.......
            hour = "Midday";
        }
        else
        {
            hour = std::to_string(icon.first-12) + "pm";
        }

        mIconFont.Print(RT,x+12,y+30,hour.c_str());

        x += ICON_WIDTH;
        n++;

        if( n == 6 )
        {// Sorry, this is crap. ;)
            break;
        }
    }

    // Draw temperature, if we have one.
    const std::string temperature = pWeather.GetCurrentTemperature();
    if( temperature.size() > 0 )
    {
        mTemperatureFont.Print(RT,RT.GetWidth() - 200,y - 20,temperature.c_str());
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

    std::vector<uint8_t> pixels;

    if( bg.GetHasAlpha() )
    {
        bg.GetRGBA(pixels);
        icon.BlitRGBA(pixels.data(),0,0,bg.GetWidth(),bg.GetHeight());
        icon.PreMultiplyAlpha();
    }
    else
    {
        bg.GetRGB(pixels);
        icon.BlitRGB(pixels.data(),0,0,bg.GetWidth(),bg.GetHeight());
    }
}
