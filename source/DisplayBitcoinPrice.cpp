
#include "DisplayBitcoinPrice.h"
#include "TinyJson.h"

#include <curl/curl.h>

static int CURLWriter(char *data, size_t size, size_t nmemb,std::string *writerData)
{
    if(writerData == NULL)
        return 0;

    writerData->append(data, size*nmemb);

    return size * nmemb;
}

DisplayBitcoinPrice::DisplayBitcoinPrice(int pBitcoinFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{
    this->SetID("bitcoin");
    this->SetPos(1,2);
    this->SetGrid(3,2);
    this->SetSpan(2,1);
    this->SetFont(pBitcoinFont);

    eui::Style s;
    s.mBackground = eui::COLOUR_DARK_GREY;
    s.mBorderSize = BORDER_SIZE;
    s.mBorder = eui::COLOUR_WHITE;
    s.mRadius = RECT_RADIUS;
    s.mAlignment = eui::ALIGN_CENTER_CENTER;
    s.mForeground = eui::COLOUR_BLACK;

    UpStyle.mBackground = eui::MakeColour(220,255,220);
    UpStyle.mBorderSize = BORDER_SIZE;
    UpStyle.mBorder = eui::COLOUR_WHITE;
    UpStyle.mRadius = RECT_RADIUS;
    UpStyle.mAlignment = eui::ALIGN_CENTER_CENTER;
    UpStyle.mForeground = eui::COLOUR_BLACK;

    DownStyle.mBackground = eui::MakeColour(255,220,220);
    DownStyle.mBorderSize = BORDER_SIZE;
    DownStyle.mBorder = eui::COLOUR_WHITE;
    DownStyle.mRadius = RECT_RADIUS;
    DownStyle.mAlignment = eui::ALIGN_CENTER_CENTER;
    DownStyle.mForeground = eui::COLOUR_BLACK;

    mControls.LastPrice = eui::Element::Create(s);
        mControls.LastPrice->SetPadding(0.05f);
        mControls.LastPrice->SetText("£XXXXXX");
        mControls.LastPrice->SetPadding(CELL_PADDING);
        mControls.LastPrice->SetPos(0,0);
    this->Attach(mControls.LastPrice);

    mControls.PriceChange = eui::Element::Create(s);
        mControls.PriceChange->SetPadding(0.05f);
        mControls.PriceChange->SetText("+XXXXXX");
        mControls.PriceChange->SetPadding(CELL_PADDING);
        mControls.PriceChange->SetPos(1,0);
    this->Attach(mControls.PriceChange);

    mControls.PriceChangePercent = eui::Element::Create(s);
        mControls.PriceChangePercent->SetPadding(0.05f);
        mControls.PriceChangePercent->SetText("+XXXXXX");
        mControls.PriceChangePercent->SetPadding(CELL_PADDING);
        mControls.PriceChangePercent->SetPos(2,0);
    this->Attach(mControls.PriceChangePercent);


    mControls.LastPriceUSD = eui::Element::Create(s);
        mControls.LastPriceUSD->SetPadding(0.05f);
        mControls.LastPriceUSD->SetText("£XXXXXX");
        mControls.LastPriceUSD->SetPadding(CELL_PADDING);
        mControls.LastPriceUSD->SetPos(0,1);
    this->Attach(mControls.LastPriceUSD);

    mControls.HighUSD = eui::Element::Create(s);
        mControls.HighUSD->SetPadding(0.05f);
        mControls.HighUSD->SetText("+XXXXXX");
        mControls.HighUSD->SetPadding(CELL_PADDING);
        mControls.HighUSD->SetPos(1,1);
    this->Attach(mControls.HighUSD);

    mControls.LowUSD = eui::Element::Create(s);
        mControls.LowUSD->SetPadding(0.05f);
        mControls.LowUSD->SetText("£XXXXXX");
        mControls.LowUSD->SetPadding(CELL_PADDING);
        mControls.LowUSD->SetPos(2,1);
    this->Attach(mControls.LowUSD);

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

                mLastPrice = price["last"].GetString();
                mPriceChange = price["priceChange"].GetString();
                mPriceChangePercent = price["priceChangePercentage"].GetString();
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
                m24HourLowUSD = price["low"].GetString();
                m24HourHighUSD = price["high"].GetString();
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

bool DisplayBitcoinPrice::OnUpdate()
{
    if( mPriceChange.find('-') == std::string::npos )
    {
        mControls.LastPrice->SetStyle(UpStyle);
        mControls.PriceChange->SetStyle(UpStyle);
        mControls.PriceChangePercent->SetStyle(UpStyle);

        mControls.LastPriceUSD->SetStyle(UpStyle);
        mControls.HighUSD->SetStyle(UpStyle);
        mControls.LowUSD->SetStyle(UpStyle);        
    }
    else
    {
        mControls.LastPrice->SetStyle(DownStyle);
        mControls.PriceChange->SetStyle(DownStyle);
        mControls.PriceChangePercent->SetStyle(DownStyle);

        mControls.LastPriceUSD->SetStyle(DownStyle);
        mControls.HighUSD->SetStyle(DownStyle);
        mControls.LowUSD->SetStyle(DownStyle);
    }

    mControls.LastPrice->SetTextF("£%s",mLastPrice.c_str());
    mControls.PriceChange->SetText(mPriceChange);
    mControls.PriceChangePercent->SetTextF("%s%%",mPriceChangePercent.c_str());

    mControls.LastPriceUSD->SetTextF("$%s",mLastPriceUSD.c_str());
    mControls.HighUSD->SetTextF("$%s",m24HourHighUSD.c_str());
    mControls.LowUSD->SetTextF("$%s",m24HourLowUSD.c_str());
    return true;
}

bool DisplayBitcoinPrice::DownloadReport(const std::string& pURL,std::string& rJson)const
{
    bool result = false;
    CURL *curl = curl_easy_init();
    if(curl)
    {
        char errorBuffer[CURL_ERROR_SIZE];
        errorBuffer[0] = 0;
        if( curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer) == CURLE_OK )
        {
            if( curl_easy_setopt(curl, CURLOPT_URL, pURL.c_str()) == CURLE_OK )
            {
                if( curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriter) == CURLE_OK )
                {
                    if( curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rJson) == CURLE_OK )
                    {
                        if( curl_easy_perform(curl) == CURLE_OK )
                        {
                            result = true;
                        }
                    }
                }
            }
        }

        if( result  == false )
		{
			std::cerr << "Lib curl, BITCOIN, failed, [" << errorBuffer << "]\n";
		}


        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }

    return result;
}
