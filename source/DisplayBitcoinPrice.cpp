#include "DisplayBitcoinPrice.h"

#include <assert.h>

#include <curl/curl.h>
#include "TinyJson.h"
#include "TinyTools.h"

static int CURLWriter(char *data, size_t size, size_t nmemb,std::string *writerData)
{
	if(writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);

	return size * nmemb;
}
/*
static std::string TrimZeros(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (isspace(*it) || *it == '0') )
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && (isspace(*rit) || *rit == '0') )
        rit++;

    return std::string(it, rit.base());
}*/

DisplayBitcoinPrice::DisplayBitcoinPrice(tinygles::GLES& pGL,const std::string& pFontPath):
    mFont( pGL.FontLoad( pFontPath + "liberation_serif_font/LiberationSerif-Regular.ttf",52)),
    GL(pGL)
{
    mPriceUpdater.Tick(60*10,[this]()
    {
        std::string jsonData;
        try
        {
            if( DownloadReport("https://api.blockchain.com/v3/exchange/tickers/BTC-GBP",jsonData) )
            {
                // We got it, now we need to build the weather object from the json.
                // I would have used rapid json but that is a lot of files to add to this project.
                // My intention is for someone to beable to drop these two files into their project and continue.
                // And so I will make my own json reader, it's easy but not the best solution.
                tinyjson::JsonProcessor json(jsonData);
                const tinyjson::JsonValue price = json.GetRoot();

                mLastPrice = (int)price.GetDouble("last_trade_price");
                mLast24Price = (int)price.GetDouble("price_24h");
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

void DisplayBitcoinPrice::Update(int pX,int pY)
{
    const std::string price = std::to_string(mLastPrice) + " GBP";
    if( price.size() > 0 )
    {
        const int width = 106 + ((price.size()-1)*22);
        const int x = pX - width;
        GL.RoundedRectangle(x,pY,pX-20,pY+70,12,255,255,255,130,true);
        if( mLastPrice > mLast24Price )
        {
            GL.FontSetColour(mFont,0,120,0);
        }
        else
        {
            GL.FontSetColour(mFont,120,0,0);
        }

        GL.FontPrint(mFont,x + 10,pY + 50,price);
    }

}

bool DisplayBitcoinPrice::DownloadReport(const std::string& pURL,std::string& rJson)const
{
	bool result = false;
	CURL *curl = curl_easy_init();
	if(curl)
	{
		char errorBuffer[CURL_ERROR_SIZE];
		errorBuffer[0] = 0;

		const char* funcName = "CURLOPT_ERRORBUFFER";
		if( curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer) == CURLE_OK )
		{
			funcName = "CURLOPT_URL";
			if( curl_easy_setopt(curl, CURLOPT_URL, pURL.c_str()) == CURLE_OK )
			{
				funcName = "CURLOPT_WRITEFUNCTION";
				if( curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriter) == CURLE_OK )
				{
					funcName = "CURLOPT_WRITEDATA";
					if( curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rJson) == CURLE_OK )
					{
						funcName = "curl_easy_perform";
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
			std::cerr << "Lib curl " << funcName << " failed, [" << errorBuffer << "]\n";
		}

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}

	return result;
}