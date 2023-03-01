#include <curl/curl.h> // libcurl4-openssl-dev
#include <iostream>
#include <string>

static int CURLWriter(char *data, size_t size, size_t nmemb,std::string *writerData)
{
	if(writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);

	return size * nmemb;
}

std::string DownloadJson(const std::string& pURL,const std::string& pWho)
{
    CURL *curl = curl_easy_init();
    std::string result;
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
                    std::string json;
                    if( curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json) == CURLE_OK )
                    {
                        if( curl_easy_perform(curl) == CURLE_OK )
                        {
                            result = json;
                        }
                    }
                }
            }
        }

        if( result.size() == 0 )
		{
			std::cerr << "Lib curl, " << pWho << " , failed, [" << errorBuffer << "]\n";
		}


        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }

    return result;
}
