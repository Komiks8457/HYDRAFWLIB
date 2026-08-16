#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <map>

class JsonBuilder
{
private:
    std::ostringstream m_ss;
    bool m_first;

    static std::string EscapeString(const std::string& str)
    {
        std::string escaped;
        escaped.reserve(str.length());
        for (size_t i = 0; i < str.length(); ++i)
        {
            char c = str[i];
            switch (c)
            {
                case '"':  escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\n': escaped += "\\n";  break;
                case '\r': escaped += "\\r";  break;
                case '\t': escaped += "\\t";  break;
                default:
                    if (c >= 32) escaped += c;
                    break;
            }
        }
        return escaped;
    }

public:
    JsonBuilder() : m_first(true)
    {}

    void Add(const std::string& key, const std::string& value)
    {
        if (!m_first) m_ss << ",";
        m_ss << "\"" << EscapeString(key) << "\":\"" << EscapeString(value) << "\"";
        m_first = false;
    }

    void Add(const std::string& key, int value)
    {
        if (!m_first) m_ss << ",";
        m_ss << "\"" << EscapeString(key) << "\":" << value;
        m_first = false;
    }

    void Add(const std::string& key, bool value)
    {
        if (!m_first) m_ss << ",";
        m_ss << "\"" << EscapeString(key) << "\":" << (value ? "true" : "false");
        m_first = false;
    }

    void AddRaw(const std::string& key, const std::string& rawJson)
    {
        if (!m_first) m_ss << ",";
        m_ss << "\"" << EscapeString(key) << "\":" << rawJson;
        m_first = false;
    }

    // Static Helpers for Arrays
    static std::string EncodeArray(const std::vector<std::string>& vec)
    {
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i > 0) ss << ",";
            ss << "\"" << EscapeString(vec[i]) << "\"";
        }
        ss << "]";
        return ss.str();
    }

    static std::string EncodeArray(const std::vector<int>& vec)
    {
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i > 0) ss << ",";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    }

    std::string GetString()
    {
        return "{" + m_ss.str() + "}";
    }
};

class JsonValue
{
public:
    enum Type { TYPE_NULL, TYPE_STRING, TYPE_INT, TYPE_BOOL, TYPE_ARRAY, TYPE_OBJECT };

    Type type;
    std::string strVal;
    int intVal;
    bool boolVal;
    std::vector<JsonValue> arrVal;
    std::map<std::string, JsonValue> objVal;

    // Constructors for implicit type conversion (like PHP!)
    JsonValue() : type(TYPE_NULL), intVal(0), boolVal(false) {}
    JsonValue(const char* s) : type(TYPE_STRING), strVal(s), intVal(0), boolVal(false) {}
    JsonValue(const std::string& s) : type(TYPE_STRING), strVal(s), intVal(0), boolVal(false) {}
    JsonValue(int i) : type(TYPE_INT), intVal(i), boolVal(false) {}
    JsonValue(bool b) : type(TYPE_BOOL), intVal(0), boolVal(b) {}
};

namespace JsonUtil
{
    // Fast inline JSON string escaper
    inline std::string EscapeString(const std::string& str)
    {
        std::string escaped;
        escaped.reserve(str.length()); // Pre-allocate memory to prevent re-allocations
        for (size_t i = 0; i < str.length(); ++i)
        {
            char c = str[i];
            switch (c)
            {
                case '"':  escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\n': escaped += "\\n";  break;
                case '\r': escaped += "\\r";  break;
                case '\t': escaped += "\\t";  break;
                default:
                    if (static_cast<unsigned char>(c) >= 32)
                        escaped += c;
                break;
            }
        }
        return escaped;
    }

    inline std::string EncodeArray(const std::vector<std::string>& vec)
    {
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i > 0) ss << ",";
            ss << "\"" << EscapeString(vec[i]) << "\"";
        }
        ss << "]";
        return ss.str();
    }

    inline std::string EncodeArray(const std::vector<int>& vec)
    {
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i > 0) ss << ",";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    }
}