/*
   Copyright (C) 2021, Richard e Collins.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
   
   Original code base is at https://github.com/HamAndEggs/TinyJson
   
   */

#ifndef TINY_JSON_H
#define TINY_JSON_H

#include <stdexcept>
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <optional>
#include <assert.h>

namespace tinyjson{ // Using a namespace to try to prevent name clashes as my class names are kind of obvious :)
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Different types of json value.
 * 
 */
#define JSON_TYPES                    \
    DEF_TYPE(JTYPE_STRING,"String")   \
    DEF_TYPE(JTYPE_NUMBER,"Number")   \
    DEF_TYPE(JTYPE_OBJECT,"Object")   \
    DEF_TYPE(JTYPE_ARRAY,"Array")     \
    DEF_TYPE(JTYPE_BOOLEAN,"Boolean") \
    DEF_TYPE(JTYPE_NULL,"NULL")
    
enum JsonValueType
{
    JTYPE_INVALID,
#define DEF_TYPE(JSON_ENUM__,JSON_NAME__) JSON_ENUM__,
    JSON_TYPES
#undef DEF_TYPE
};

/**
 * @brief Fetches the string human readable name for a just type.
 * 
 * @param pType 
 * @return std::string 
 */
inline std::string JsonValueTypeToString(JsonValueType pType)
{
    switch(pType)
    {
#define DEF_TYPE(JSON_ENUM__,JSON_NAME__) case JSON_ENUM__:return JSON_NAME__;
    JSON_TYPES
#undef DEF_TYPE
    case JTYPE_INVALID:
        throw std::runtime_error("JsonValueTypeToString passed an uninitialized type value");    
        break;
    };
    throw std::runtime_error("JsonValueTypeToString passed an unknown type value");    
    return "unknown json type";
}

/**
 * @brief This represents the core data structure that drives Json.
 * It forms the second part of 1 or more the key value pair that represents a Json object.
 */
struct JsonValue
{
    JsonValue():mBoolean(false),mType(JTYPE_INVALID)
    {}

    /**
     * @brief This holds the true or false value if the json value is TRUE or FALSE
     * The json spec defines types, one for false and one for true. That is daft.
     * So I define a boolean type and set my type to JTYPE_BOOLEAN and store the value.
     */
    bool mBoolean;

	/**
	 * @brief I hold all number values as a string, this is because until the user asks I do not know what type they want it as.
	 * I also put the strings in here.
	 * I do not put objects or arrays as could take up a lot of space and also not needed.
	 */
	std::string mValue;

    /**
     * @brief A json object is a list of 1 or more key, value pairs.
     */
    std::map<std::string,JsonValue> mObject;

    /**
     * @brief The storage for an array, which is just an array of json values.
     */
	std::vector<JsonValue> mArray;

    /**
     * @brief This is a handy overload that allows you to do ["key1"]["key2"]["key3"].GetInt() type of thing.
     * throws std::runtime_error if key not found.
     * @param pKey The key string that represents the value you want.
     * @return const JsonValue& The value.
     */
    const JsonValue& operator [](const std::string& pKey)const
    {
        AssertType(JTYPE_OBJECT);
        const auto found = mObject.find(pKey);
        if( found != mObject.end() )
            return found->second;
        throw std::runtime_error("Json value for key " + pKey + " not found");
    }

    /**
     * @brief All0ws you to access the array type with an index and not have to add a ".Array"
     * This means MyJson["songs"][10]["name"].GetString() is possible.
     * @param pIndex 
     * @return const JsonValue& 
     */
    const JsonValue& operator [](size_t pIndex)const
    {
        AssertType(JTYPE_ARRAY);
        return mArray[pIndex];
    }

    /**
     * @brief Set the Type of the value
     * 
     * @param pNewType 
     */
    void SetType(JsonValueType pNewType)
    {
        mType = pNewType;
    }

    /**
     * @brief Checks that the key passed in exists without throwing an exception.
     * If you do MyJson["scores"][10].GetInt() and "scores" was not in the root the code will throw an exception.
     * @param pKey 
     * @return true 
     * @return false 
     */
    bool HasValue(const std::string& pKey)const
    {
        if( mType == JTYPE_OBJECT )
        {
            const auto found = mObject.find(pKey);
            if( found != mObject.end() )
                return true;
        }

        return false;
    }

    /**
     * @brief Get the Type of the jason value
     * 
     * @return JsonValueType 
     */
    JsonValueType GetType()const
    {
        return mType;
    }


    /**
     * @brief Fetches the size of the array, if the type is an array, else zero.
     * 
     * @return size_t 
     */
    size_t GetArraySize()const
    {
        if( mType == JTYPE_ARRAY )
            return mArray.size();
        return 0;
    }

    /**
     * @brief Gets the value as a string, if it is a string type.
     * Else throws an exception.
     * @return const std::string& 
     */
    const std::string& GetString()const
    {
        AssertType(JTYPE_STRING);
        return mValue;
    }

    /**
     * @brief Gets the value as a double, if it is a number type.
     * Else throws an exception.
     * @return double 
     */
    double GetDouble()const
    {
        AssertType(JTYPE_NUMBER);
        return std::stod(mValue);
    }

    /**
     * @brief Gets the value as a float, if it is a number type.
     * Else throws an exception.
     * @return float 
     */
    float GetFloat()const
    {
        AssertType(JTYPE_NUMBER);
        return std::stof(mValue);
    }

    /**
     * @brief Gets the value as an int, if it is a number type.
     * Else throws an exception.
     * @return int 
     */
    int GetInt()const
    {
        return GetInt32();
    }

    /**
     * @brief Gets the value as an uint64_t, if it is a number type.
     * Else throws an exception.
     * @return uint64_t 
     */
    uint64_t GetUInt64()const
    {
        AssertType(JTYPE_NUMBER);
        return std::stoull(mValue);
    }

    /**
     * @brief Gets the value as an uint32_t, if it is a number type.
     * Else throws an exception.
     * @return uint32_t 
     */
    uint32_t GetUInt32()const
    {
        AssertType(JTYPE_NUMBER);
        return std::stoul(mValue);
    }

    /**
     * @brief Gets the value as an int64_t, if it is a number type.
     * Else throws an exception.
     * @return int64_t 
     */
    int64_t GetInt64()const
    {
        AssertType(JTYPE_NUMBER);
        return std::stoll(mValue);
    }

    /**
     * @brief Gets the value as an int32_t, if it is a number type.
     * Else throws an exception.
     * @return int32_t 
     */
    int32_t GetInt32()const
    {
        AssertType(JTYPE_NUMBER);
        return std::stol(mValue);
    }

    /**
     * @brief Gets the value as an boolean, if it is a boolean type.
     * Else throws an exception.
     * @return bool 
     */
    bool GetBoolean()const
    {
        AssertType(JTYPE_BOOLEAN);
        return mBoolean;
    }

    /**
     * @brief Returns true if the type is a NULL.
     * 
     * @return true 
     * @return false 
     */
    bool GetIsNull()const
    {// We don't assert here as the false is an ok answer.
        return mType == JTYPE_NULL;
    }

    /***************************************************
     * Following set of functions are more robust and allow you to supply a default if the key is missing or the expected type is wrong.
     * But beware uses these, although convenient, will hide errors in code. These are best used when you do not have control over the
     * Json being read.
     * If it's one of your files that you generate it is best to use the ones above as they will warn you about errors in the data.
     * 
     * @brief These functions will return the value found if the key is present and the value type is correct, else will return the default.
     * @param pKey The Key of the value to look for.
     * @param pDefault The default value to use if there was a problem.
     * @return
     * 
     ***************************************************/
    #define MAKE_SAFE_FUNCTION(FUNC_NAME__,FUNC_TYPE__,DEFAULT_VALUE__)                             \
    FUNC_TYPE__ FUNC_NAME__(const std::string& pKey,FUNC_TYPE__ pDefault = DEFAULT_VALUE__)const    \
    {                                                                                               \
        try{return (*this)[pKey].FUNC_NAME__();}                                                    \
        catch( const std::runtime_error& ){}/* Ignore exception and return the default.*/           \
        return pDefault;                                                                            \
    }

    MAKE_SAFE_FUNCTION(GetArraySize,size_t,0);
    MAKE_SAFE_FUNCTION(GetString,const std::string&,"");
    MAKE_SAFE_FUNCTION(GetDouble,double,0.0);
    MAKE_SAFE_FUNCTION(GetFloat,float,0.0f);
    MAKE_SAFE_FUNCTION(GetInt,int,0);
    MAKE_SAFE_FUNCTION(GetUInt64,uint64_t,0);
    MAKE_SAFE_FUNCTION(GetUInt32,uint32_t,0);
    MAKE_SAFE_FUNCTION(GetInt64,int64_t,0);
    MAKE_SAFE_FUNCTION(GetInt32,int32_t,0);
    MAKE_SAFE_FUNCTION(GetBoolean,bool,false);
    MAKE_SAFE_FUNCTION(GetIsNull,bool,false);
    MAKE_SAFE_FUNCTION(GetType,JsonValueType,JTYPE_INVALID);
 
    #undef MAKE_SAFE_FUNCTION


private:
	JsonValueType mType;

    /**
     * @brief Throws an exception if the type is not a match.
     * 
     * @param pType 
     */
    void AssertType(JsonValueType pType)const
    {
        if( mType != pType )
        {
            throw std::runtime_error("Json Type is not what is expected, the type is " + JsonValueTypeToString(mType) +" looking for " + JsonValueTypeToString(pType));
        }
    }

};

/**
 * @brief This is the work horse that builds our data structure that mirrors the json data.
 * 
 */
class JsonProcessor
{
public:
    /**
     * @brief Construct a new Json Processor object and parse the json data.
     * throws std::runtime_error if the json is not constructed correctly.
     * @param pJsonString 
     */
	JsonProcessor(const std::string& pJsonString) :
        mJson(pJsonString.c_str()),
        mJsonEnd(pJsonString.c_str() + pJsonString.size() + 1)
    {
        if( pJsonString.size() < 2 )
        {
            throw std::runtime_error("Empty string passed into ParseJson");
        }

        mRoot.SetType(JTYPE_OBJECT);
        MakeObject(mRoot.mObject);// This function will leave json pointing to the next non white space.

        // Skip white space that maybe after it.
        SkipWhiteSpace();

        // Now should be at the end
        if( mJson >= mJsonEnd )
        {
            throw std::runtime_error("Data found after root object, invalid Json");
        }
    }

    /**
     * @brief Get the Root object
     * 
     * @return const JsonValue& 
     */
    const JsonValue& GetRoot()const
    {
        return mRoot;
    }

private:
    const char* mJson;  //!< The current position in the data that we are at.
    const char* const mJsonEnd; //!< Used to detect when we're at the end of the data.
    JsonValue mRoot; //!< When all is done, this contains the json as usable c++ objects.

    /**
     * @brief This is used in several place whilst parsing the data to detect json data that is not complete.
     * For safety does not test for NULL but checks that the mJson pointer has not gone past the end of the data.
     */
    inline void AssertMoreData(const char* pErrorString)
    {
        if( mJson >= mJsonEnd ){throw std::runtime_error(pErrorString);}
    }

    /**
     * @brief Used to check that the expected charater is the correct one, if not tells the user!
     */
    inline void AssertCorrectChar(char c,const char* pErrorString)
    {
        if( *mJson != c ){throw std::runtime_error(pErrorString);}
    }

    /**
     * @brief Builds a Json object, which is a map of key value paris.
     * Constructed in this way to reduce copy by value which is what you would get by returning the completed object. That would be horrendous.
     * @param rObject The json object that is to be built.
     */
    void MakeObject(std::map<std::string,JsonValue>& rObject)
    {
        // Search for the start of the object.
        SkipWhiteSpace();
        AssertCorrectChar('{',"Start of object not found, invalid Json");

        do
        {
            mJson++;// Skip object start char or comma for more key value pairs.
            const std::string objKey = ReadString();

            // Now parse it's value.
            SkipWhiteSpace();
            AssertCorrectChar(':',"Json format error detected, seperator character ':'");
            mJson++;

            MakeValue(rObject[objKey]);

            // Now see if there are more key valuer pairs to add to the object or if we're done.
            // Skip white space, then see if next char is a } (for end of object) or a , for more key value pairs.
            // We leave the pointer there and drop out the function.
            // Down to the caller to test if they carry on or finish the current object.
            SkipWhiteSpace();
            if( *mJson != '}' && *mJson != ',' )
            {
                throw std::runtime_error("Json format error detected, did you forget a comma between key value pairs? For key " + objKey);
            }
        }while (*mJson == ',');

        // Validate end of object.
        if( *mJson == '}' )
        {
            mJson++;
        }
        else
        {
            throw std::runtime_error("End of root object not found, invalid Json");
        }
    }

    /**
     * @brief Builds the core value structure that powers Json.
     * 
     * @param pNewValue As with MakeObject, we don't return the new value but initialise the one passed in.
     */
    void MakeValue(JsonValue& pNewValue)
    {
        SkipWhiteSpace();// skip space and then see if it's an object, string, value or special state (TRUE,FALSE,NULL).

        switch( *mJson )
        {
        case 0:
            break;

        case '{':
            pNewValue.SetType(JTYPE_OBJECT);
            MakeObject(pNewValue.mObject);
            break;

        case '[':
            pNewValue.SetType(JTYPE_ARRAY);
            do
            {
                mJson++;//skip ']' or the ','
                // Looks odd, but is the easiest / optimal way to reduce memory reallocations using c++14 features.
                pNewValue.mArray.resize(pNewValue.mArray.size()+1);
                MakeValue(pNewValue.mArray.back());
                SkipWhiteSpace();
            }while(*mJson == ',');

            // Check we did get to the end.
            if( *mJson != ']' )
            {
                throw std::runtime_error("Json format error detected, array not terminated with ']'");
            }
            mJson++;//skip ']'
            break;

        case '\"':
            pNewValue.SetType(JTYPE_STRING);
            pNewValue.mValue = ReadString();
            break;

        case 'T':
        case 't':
            if( tolower(mJson[1]) == 'u' && tolower(mJson[1]) == 'r' && tolower(mJson[1]) == 'e' )
            {
                mJson += 4;
                pNewValue.SetType(JTYPE_BOOLEAN);
                pNewValue.mBoolean = true;
            }
            else
            {
                throw std::runtime_error(std::string("Invalid character ") + std::string(mJson,10) + " found in json value definition");
            }
            break;

        case 'F':
        case 'f':
            if( tolower(mJson[1]) == 'a' && tolower(mJson[1]) == 'l' && tolower(mJson[1]) == 's' && tolower(mJson[1]) == 'e' )
            {
                mJson += 5;
                pNewValue.SetType(JTYPE_BOOLEAN);
                pNewValue.mBoolean = false;
            }
            else
            {
                throw std::runtime_error(std::string("Invalid character ") + std::string(mJson,10) + " found in json value definition");
            }
            break;

        case 'N':
        case 'n':
            if( tolower(mJson[1]) == 'u' && tolower(mJson[1]) == 'l' && tolower(mJson[1]) == 'l' )
            {
                mJson += 4;
                pNewValue.SetType(JTYPE_NULL);
            }
            else
            {
                throw std::runtime_error(std::string("Invalid character ") + std::string(mJson,10) + " found in json value definition");
            }
            break;

        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            // Scan to white space, comma or object end.
            {
                const char* valueStart = mJson;
                FindEndOfNumber();
                pNewValue.SetType(JTYPE_NUMBER);
                pNewValue.mValue.assign(valueStart,mJson-valueStart);
            }
            break;

        default:
            throw std::runtime_error(std::string("Invalid character ") + std::string(mJson,10) + " found at start of json value definition");
            break;
        }
    }

    /**
     * @brief Skips to the next char that is not white space.
     */
    void SkipWhiteSpace()
    {
        while( isspace(*mJson) )
        {// As per Json spec, look for characters that are not a space, linefeed, carrage return or horizontal tab. isspace does this.
            AssertMoreData("Abrupt end to json whilst skipping white space");
            mJson++;
        }
    }

    /**
     * @brief Reads a string value.
     * 
     * @return std::string 
     */
    std::string ReadString()
    {
        // First find the start of the string
        SkipWhiteSpace();
        AssertCorrectChar('\"',"Json format error detected, expected start of string, did you forget to put the string in quotes?");

        mJson++; // Skip "
        const char* stringStart = mJson;

        // Now scan till we hit the next "
        while( *mJson != '\"' )
        {
        // Did we hit the end?
            AssertMoreData("Abrupt end to json whilst reading string");
            mJson++;
        }
        const size_t len = mJson - stringStart;
        mJson++; // Skip "
        if( len > 0 )
        {
            return std::string(stringStart,len);
        }
        // Empty string valid.
        return "";
    }

    /**
     * @brief Scans for the end of the number that we just found the start too.
     * mJson is set to the end of the number.
     */
    void FindEndOfNumber()
    {
        // As per Json spec, keep going to we see end of accepted number components.
        // There is an order that you do this in, see https://www.json.org/json-en.html
        if( *mJson == '-' )
        {
            mJson++;
        }

        // after accounting the - there must be a number next.
        if( isdigit(*mJson) == false )
        {
            throw std::runtime_error(std::string("Malformed number ") + std::string(mJson-1,20) );
        }

        // Scan for end of digits.
        while( isdigit(*mJson) )
        {
            mJson++;
        }

        // Do we have a decimal?
        if( *mJson == '.' )
        {
            mJson++;
            // Now scan more more digits.
            while( isdigit(*mJson) )
            {
                mJson++;
            }
            // now see if there is an exponent. 
            if( *mJson == 'E' || *mJson == 'e' )
            {
                mJson++;
                // Now must be a sign.
                if( *mJson == '-' || *mJson == '+' )
                {
                    mJson++;

                    // after accounting the - or + there must be a number next.
                    if( isdigit(*mJson) == false )
                    {
                        throw std::runtime_error(std::string("Malformed exponent in number ") + std::string(mJson-1,20) );
                    }

                    // Now scan more more digits.
                    while( isdigit(*mJson) )
                    {
                        mJson++;
                    }
                }
                else
                {
                    throw std::runtime_error(std::string("Malformed exponent in number ") + std::string(mJson-1,20) );
                }
            }
        }
        AssertMoreData("Abrupt end to json whilst reading number");
    }
};//end of struct JsonProcessor

///////////////////////////////////////////////////////////////////////////////////////////////////////////
};// namespace tinyjson

#endif //TINY_JSON_H