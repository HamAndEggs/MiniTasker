/*
   Copyright (C) 2017, Richard e Collins.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#include "MQTTData.h"

#include <assert.h>
#include <unistd.h>

#include <iostream>


void MQTTData::CallbackConnected(struct mosquitto *mosq, void *userdata, int result)
{
    assert(mosq);
	if(result == MOSQ_ERR_SUCCESS)
    {
        assert(userdata);
        ((MQTTData*)userdata)->OnConnected();
	}
    else
    {
		std::cout << "MQTT Error: Failed to connect\n";
	}
}

void MQTTData::CallbackMessage(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    assert(mosq);
    assert(message);
	if(message && message->payloadlen)
    {
        assert(userdata);

        const std::string topic = message->topic;
        const std::string data = ((const char*)message->payload);

        ((MQTTData*)userdata)->mOnData(topic,data);

	}
}

#ifdef VERBOSE_BUILD
static void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	std::cout << "Subscribed (mid: " << mid << "): " << granted_qos[0];
	for(int i=1; i<qos_count; i++)
    {
		std::cout << ", " << granted_qos[i];
	}
    std::cout << "\n";
}
#endif

static void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    assert(mosq);

#ifdef VERBOSE_BUILD
    if( str )
    {
	    std::cout << str << "\n";
    }
#endif
}

MQTTData::MQTTData(const std::string& pHost,int pPort,
    const std::vector<std::string> pTopics,
    std::function<void(const std::string &pTopic,const std::string &pData)> pOnData):
    mHost(pHost),
    mPort(pPort),
    mTopics(pTopics),
    mOnData(pOnData)
{
    assert(pHost.size() > 0 && pPort);

    bool clean_session = true;
    mosquitto_lib_init();
    mMQTT = mosquitto_new(NULL, clean_session, this);
    if( !mMQTT )
    {
        std::cerr << "MQTT Init Error: Out of memory\n";
    }
    else
    {
        std::cout << "MQTT started\n";
    }
}

void MQTTData::Tick()
{

    if( mMQTT && mConnected == false )
    {
        int keepalive = 60;
        mosquitto_log_callback_set(mMQTT, my_log_callback);
        mosquitto_connect_callback_set(mMQTT, CallbackConnected);
        mosquitto_message_callback_set(mMQTT, CallbackMessage);
#ifdef VERBOSE_BUILD
        mosquitto_subscribe_callback_set(mMQTT, my_subscribe_callback);
#endif

        if( mosquitto_connect(mMQTT, mHost.c_str(), mPort, keepalive) == MOSQ_ERR_SUCCESS )
        {
            mConnected = true;
            // Now start the loop.
            if( mosquitto_loop_start(mMQTT) == MOSQ_ERR_SUCCESS )
            {
                mOk = true;
            }
            else
            {
                std::cout << "MQTT Init Error: Failed to start networking loop\n";
            }
        }
        else
        {
            std::cout << "No connection\n";
        }
    }
}


MQTTData::~MQTTData()
{
    if( mMQTT )
    {
        mosquitto_destroy(mMQTT);
        if( mosquitto_loop_stop(mMQTT,false) != MOSQ_ERR_SUCCESS )
        {
            mosquitto_loop_stop(mMQTT,true);// Force the close.
        }
    }
    mosquitto_lib_cleanup();
}

void MQTTData::OnConnected()
{
#ifdef DEBUG_BUILD
    std::cout << "MQTT OnConnected\n";
#endif

    // Subscribe to broker information topics on successful connect.
    for( auto topic : mTopics )
    {
        Subscribe(topic);
    }
}

void MQTTData::Subscribe(const std::string& pTopic)
{
    assert( pTopic.size() > 0 );
    assert( mMQTT );

    const int ret = mosquitto_subscribe(mMQTT, NULL, pTopic.c_str(), MQTT_QOS_AT_MOST_ONCE);
    switch( ret )
    {
    default:
        std::cerr << "Subscribing too " << pTopic << " failed with unknown error:" << ret << "\n";
        break;

    case MOSQ_ERR_SUCCESS:
#ifdef DEBUG_BUILD
         std::cout << "Subscribing too " << pTopic << "\n";
#endif
        break;

    case MOSQ_ERR_INVAL:
        std::cerr << "MQTT Subscribe Error: [" << pTopic << "] The input parameters were invalid";
        break;

    case MOSQ_ERR_NOMEM:
        std::cerr << "MQTT Subscribe Error: [" << pTopic << "]an out of memory condition occurred\n";
        break;

    case MOSQ_ERR_NO_CONN:
        std::cerr << "MQTT Subscribe Error: [" << pTopic << "]the client isn't connected to a broker.\n";
        break;

    case MOSQ_ERR_MALFORMED_UTF8:
        std::cerr << "MQTT Subscribe Error: [" << pTopic << "]the topic is not valid UTF-8\n";
        break;
#if LIBMOSQUITTO_REVISION > 7
    case MOSQ_ERR_OVERSIZE_PACKET:
        std::cerr << "MQTT Subscribe Error: [" << pTopic << "] Over sized packet\n";
        break;
#endif
    }
}
