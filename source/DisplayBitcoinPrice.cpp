
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

DisplayBitcoinPrice::DisplayBitcoinPrice(int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{
    this->SetID("bitcoin");
    this->SetPos(1,0);
    this->SetGrid(2,2);

    eui::Style s;
    s.mBackground = eui::COLOUR_DARK_GREY;
    s.mBorderSize = BORDER_SIZE;
    s.mBorder = eui::COLOUR_WHITE;
    s.mRadius = RECT_RADIUS;
    s.mAlignment = eui::ALIGN_CENTER_CENTER;

    mControls.LastPrice = eui::Element::Create(s);
        mControls.LastPrice->SetPadding(0.05f);
        mControls.LastPrice->SetFont(pNormalFont);
        mControls.LastPrice->SetText("£XXXXXX");
        mControls.LastPrice->SetPadding(CELL_PADDING);
        mControls.LastPrice->SetPos(0,0);
    this->Attach(mControls.LastPrice);

    mControls.PriceChange = eui::Element::Create(s);
        mControls.PriceChange->SetPadding(0.05f);
        mControls.PriceChange->SetFont(pNormalFont);
        mControls.PriceChange->SetText("+XXXXXX");
        mControls.PriceChange->SetPadding(CELL_PADDING);
        mControls.PriceChange->SetPos(0,1);
    this->Attach(mControls.PriceChange);

    mControls.High = eui::Element::Create(s);
        mControls.High->SetPadding(0.05f);
        mControls.High->SetFont(pNormalFont);
        mControls.High->SetText("+XXXXXX");
        mControls.High->SetPadding(CELL_PADDING);
        mControls.High->SetPos(1,0);
    this->Attach(mControls.High);

    mControls.Low = eui::Element::Create(s);
        mControls.Low->SetPadding(0.05f);
        mControls.Low->SetFont(pNormalFont);
        mControls.Low->SetText("£XXXXXX");
        mControls.Low->SetPadding(CELL_PADDING);
        mControls.Low->SetPos(1,1);
    this->Attach(mControls.Low);

    mPriceUpdater.Tick(60*10,[this]()
    {
        std::string jsonData;
        try
        {
            const std::string url = "https://cex.io/api/ticker/BTC/GBP";
            if( DownloadReport(url,jsonData) )
            {
                // We got it, now we need to build the weather object from the json.
                // I would have used rapid json but that is a lot of files to add to this project.
                // My intention is for someone to beable to drop these two files into their project and continue.
                // And so I will make my own json reader, it's easy but not the best solution.
                tinyjson::JsonProcessor json(jsonData);
                const tinyjson::JsonValue price = json.GetRoot();

                mLastPrice = (int)std::stoi(price["last"].GetString());
                mPriceChange = (int)std::stoi(price["priceChange"].GetString());
                m24HourLow = (int)std::stoi(price["low"].GetString());
                m24HourHigh = (int)std::stoi(price["high"].GetString());
            }
        }
        catch(std::runtime_error &e)
        {
            std::cerr << "Failed to download bitcoin price: " << e.what() << "\n";
        }
    });
}

DisplayBitcoinPrice::~DisplayBitcoinPrice()
{
    mPriceUpdater.TellThreadToExitAndWait();
}

bool DisplayBitcoinPrice::OnUpdate()
{
    mControls.LastPrice->SetTextF("£%d",mLastPrice);

    std::string growth = std::to_string(mPriceChange);
    if( mPriceChange > 0 )
        growth = "+" + growth;

    mControls.PriceChange->SetText(growth);

    mControls.High->SetTextF("£%d",m24HourHigh);
    mControls.Low->SetTextF("£%d",m24HourLow);
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

        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }

    return result;
}
