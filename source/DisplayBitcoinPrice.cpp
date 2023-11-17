
#include "DisplayBitcoinPrice.h"
#include "TinyJson.h"
#include "FileDownload.h"


DisplayBitcoinPrice::DisplayBitcoinPrice(int pBitcoinFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{
    this->SetID("bitcoin");
    this->SetFont(pBitcoinFont);

    UpStyle.mBackground = eui::MakeColour(100,255,100);
    UpStyle.mThickness = BORDER_SIZE;
    UpStyle.mBorder = eui::COLOUR_WHITE;
    UpStyle.mRadius = RECT_RADIUS;
    UpStyle.mForeground = eui::COLOUR_BLACK;

    DownStyle.mBackground = eui::MakeColour(255,100,100);
    DownStyle.mThickness = BORDER_SIZE;
    DownStyle.mBorder = eui::COLOUR_WHITE;
    DownStyle.mRadius = RECT_RADIUS;
    DownStyle.mForeground = eui::COLOUR_BLACK;

    mControls.LastPriceUK = new eui::Element;
        mControls.LastPriceUK->SetPadding(0.05f);
        mControls.LastPriceUK->SetText("£XXXXXX");
        mControls.LastPriceUK->SetPadding(CELL_PADDING);
        mControls.LastPriceUK->SetPos(0,0);
    this->Attach(mControls.LastPriceUK);

    mControls.LastPriceUSD = new eui::Element;
        mControls.LastPriceUSD->SetPadding(0.05f);
        mControls.LastPriceUSD->SetText("£XXXXXX");
        mControls.LastPriceUSD->SetPadding(CELL_PADDING);
        mControls.LastPriceUSD->SetPos(1,0);
    this->Attach(mControls.LastPriceUSD);

    mPriceUpdater.Tick(60*10,[this]()
    {
        try
        {
            std::string jsonData;
            const std::string url = "https://cex.io/api/ticker/BTC/GBP";
            if( DownloadReport(url,jsonData) )
            {
                // We got it, now we need to build the weather object from the json.
                // I would have used rapid json but that is a lot of files to add to this project.
                // My intention is for someone to beable to drop these two files into their project and continue.
                // And so I will make my own json reader, it's easy but not the best solution.
                tinyjson::JsonProcessor json(jsonData);
                const tinyjson::JsonValue price = json.GetRoot();

                mLastPriceUK = price["last"].GetString();
                mPriceChange = price["priceChange"].GetString();

                mPriceGBP = std::stod(mLastPriceUK);
            }
        }
        catch(std::runtime_error &e)
        {
            std::cerr << "Failed to download UK bitcoin price: " << e.what() << "\n";
        }

        try
        {
            std::string jsonData;
            // Now do dollar.
            const std::string urlUSD = "https://cex.io/api/ticker/BTC/USD";
            if( DownloadReport(urlUSD,jsonData) )
            {
                // We got it, now we need to build the weather object from the json.
                // I would have used rapid json but that is a lot of files to add to this project.
                // My intention is for someone to beable to drop these two files into their project and continue.
                // And so I will make my own json reader, it's easy but not the best solution.
                tinyjson::JsonProcessor json(jsonData);
                const tinyjson::JsonValue price = json.GetRoot();

                mLastPriceUSD = price["last"].GetString();
            }
        }
        catch(std::runtime_error &e)
        {
            std::cerr << "Failed to download USD bitcoin price: " << e.what() << "\n";
        }

    });
}

DisplayBitcoinPrice::~DisplayBitcoinPrice()
{
    mPriceUpdater.TellThreadToExitAndWait();
}

bool DisplayBitcoinPrice::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( mPriceChange.find('-') == std::string::npos )
    {
        mControls.LastPriceUK->SetStyle(UpStyle);
        mControls.LastPriceUSD->SetStyle(UpStyle);
    }
    else
    {
        mControls.LastPriceUK->SetStyle(DownStyle);
        mControls.LastPriceUSD->SetStyle(DownStyle);
    }

    mControls.LastPriceUK->SetTextF("£%s",mLastPriceUK.c_str());
    mControls.LastPriceUSD->SetTextF("$%s",mLastPriceUSD.c_str());

    return true;
}

bool DisplayBitcoinPrice::DownloadReport(const std::string& pURL,std::string& rJson)const
{
    rJson = DownloadJson(pURL,"DisplayBitcoinPrice");
    return rJson.size() > 2;
}
