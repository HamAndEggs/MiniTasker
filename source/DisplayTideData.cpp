
#include "DisplayTideData.h"
#include "TinyJson.h"
#include "FileDownload.h"
#include <time.h>
#include <chrono>
#include <sstream>

DisplayTideData::DisplayTideData(int pFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{
    SetGrid(3,1);

    eui::Style timeStyle;
    timeStyle.mBackground = eui::MakeColour(100,100,255);
    timeStyle.mThickness = BORDER_SIZE;
    timeStyle.mBorder = eui::COLOUR_WHITE;
    timeStyle.mRadius = RECT_RADIUS;
    timeStyle.mForeground = eui::COLOUR_BLACK;

    mHighTide = new eui::Element;
        mHighTide->SetPadding(0.05f);
        mHighTide->GetStyle().mAlignment = eui::ALIGN_CENTER_CENTER;
        mHighTide->SetFont(pFont);
        mHighTide->SetStyle(timeStyle);
        mHighTide->SetPadding(CELL_PADDING);
    this->Attach(mHighTide);

    mLowTide = new eui::Element;
        mLowTide->SetPadding(0.05f);
        mLowTide->GetStyle().mAlignment = eui::ALIGN_CENTER_CENTER;
        mLowTide->SetFont(pFont);
        mLowTide->SetStyle(timeStyle);
        mLowTide->SetPadding(CELL_PADDING);
    this->Attach(mLowTide);

    mLowTide->SetText("Loading...");

    mTideUpdater.Tick(60*60,[this]()
    {// Fetch once an hour
        try
        {
            std::string stationJson;
            if( DownloadTideData("https://easytide.admiralty.co.uk/Home/GetStations",stationJson) )
            {
                // We got it, now we need to build the weather object from the json.
                // I would have used rapid json but that is a lot of files to add to this project.
                // My intention is for someone to beable to drop these two files into their project and continue.
                // And so I will make my own json reader, it's easy but not the best solution.
                tinyjson::JsonProcessor stationData(stationJson);
                const tinyjson::JsonValue stationDataRoot = stationData.GetRoot();
                const tinyjson::JsonValue features = stationDataRoot["features"];

                // Find our station ID
                std::string stationID;
                for( auto f : features.mArray)
                {
                    if( f["properties"]["Name"].GetString() == "Ryde" &&
                        f["properties"]["Country"].GetString() == "England" )
                    {
                        stationID = f["properties"]["Id"].GetString();
                        break;
                    }
                }

                if( stationID.size() > 0 )
                {
                    // Now download the tide data for that station
                    std::string tideJson;
                    if( DownloadTideData("https://easytide.admiralty.co.uk/Home/GetPredictionData?stationId=" + stationID,tideJson) )
                    {
                        // We got it, now we need to build the weather object from the json.
                        // I would have used rapid json but that is a lot of files to add to this project.
                        // My intention is for someone to beable to drop these two files into their project and continue.
                        // And so I will make my own json reader, it's easy but not the best solution.
                        tinyjson::JsonProcessor tideData(tideJson);
                        const tinyjson::JsonValue tideDataRoot = tideData.GetRoot();

                        const tinyjson::JsonValue tidalEventList = tideDataRoot["tidalEventList"];

                        std::time_t result = std::time(nullptr);
                        tm *currentTime = localtime(&result);

                        bool gotHightTide = false;
                        bool gotLowTide = false;
                        tm highTide;
                        tm lowTide;
                        // Find the next tide events.
                        for( auto event : tidalEventList.mArray)
                        {
                            const std::string timeString = event["dateTime"].GetString();
                            std::istringstream time(timeString);
                            tm eventTime;
                            time >> std::get_time(&eventTime, "%Y-%m-%dT%H:%M:%S");
                            if (time.fail())
                            {
                                std::cerr << "Failed to parse event time\n";
                            }
                            else if( difftime(std::mktime(&eventTime),std::mktime(currentTime)) > 0 )
                            {
                                if( gotHightTide == false && event["eventType"].GetInt() == 0 )
                                {
                                    highTide = eventTime;
                                    std::cout << "Event time " << timeString << " -> " << std::put_time(&eventTime, "%c") << "\n";
                                    gotHightTide = true;
                                    mHighTide->SetTextF("HIGH: %02d:%02d",eventTime.tm_hour,eventTime.tm_min);
                                }
                                else if( gotLowTide == false && event["eventType"].GetInt() == 1 )
                                {
                                    lowTide = eventTime;
                                    std::cout << "Event time " << timeString << " -> " << std::put_time(&eventTime, "%c") << "\n";
                                    gotLowTide = true;
                                    mLowTide->SetTextF("LOW: %02d:%02d",eventTime.tm_hour,eventTime.tm_min);
                                }
                            }

                            if( difftime(std::mktime(&lowTide),std::mktime(&highTide)) < 0 )
                            {
                                mLowTide->SetPos(0,0);
                                mHighTide->SetPos(1,0);
                            }
                            else
                            {
                                mHighTide->SetPos(0,0);
                                mLowTide->SetPos(1,0);
                            }
                        }
                    }
                }
            }
        }
        catch(std::runtime_error &e)
        {
            std::cerr << "Failed to download UK bitcoin price: " << e.what() << "\n";
        }
    });
}

DisplayTideData::~DisplayTideData()
{
    mTideUpdater.TellThreadToExitAndWait();
}
    
bool DisplayTideData::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( mPortsmouthEngland > 0 )
    {

    }

    return true;
}

bool DisplayTideData::DownloadTideData(const std::string& pURL,std::string& rJson)const
{
    rJson = DownloadJson(pURL,"DisplayTideData");
    return rJson.size() > 2;
}
