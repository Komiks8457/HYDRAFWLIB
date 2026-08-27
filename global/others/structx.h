#pragma once

#pragma pack(push, 4)
/////////////////////////////////////////////////////////
// SimpleTime (Standard 32-bit, Second Precision)
/////////////////////////////////////////////////////////
#define SIMPLETIME_YEAR_BASE (WORD)2000

struct SimpleTime
{
    union
    {
        DWORD dwTime;
        struct
        {
            DWORD year  : 6;    // 2000 ~ 2063
            DWORD month : 4;    // 1 ~ 12
            DWORD day   : 5;    // 1 ~ 31
            DWORD hour  : 5;    // 0 ~ 23
            DWORD min   : 6;    // 0 ~ 59
            DWORD sec   : 6;    // 0 ~ 59
        };
    };

    SimpleTime() { dwTime = 0; }
    SimpleTime(DWORD t) { dwTime = t; }
    SimpleTime(const SYSTEMTIME& stime)
    {
        year  = (stime.wYear - SIMPLETIME_YEAR_BASE);
        month = stime.wMonth;
        day   = stime.wDay;
        hour  = stime.wHour;
        min   = stime.wMinute;
        sec   = stime.wSecond;
    }

    void SetCurLocalTime()
    {
        SYSTEMTIME stime;
        ::GetLocalTime(&stime);
        year  = (stime.wYear - SIMPLETIME_YEAR_BASE);
        month = stime.wMonth;
        day   = stime.wDay;
        hour  = stime.wHour;
        min   = stime.wMinute;
        sec   = stime.wSecond;
    }

    void ConvertTimeTo(SYSTEMTIME& stime) const
    {
        memset(&stime, 0, sizeof(SYSTEMTIME));
        stime.wYear   = (WORD)(year + SIMPLETIME_YEAR_BASE);
        stime.wMonth  = (WORD)month;
        stime.wDay    = (WORD)day;
        stime.wHour   = (WORD)hour;
        stime.wMinute = (WORD)min;
        stime.wSecond = (WORD)sec;
    }

    static int CompareTime(const SimpleTime& t1, const SimpleTime& t2)
    {
        if (t1.dwTime == t2.dwTime) return 0;
        if (t1.year != t2.year)   return (t1.year > t2.year) ? -1 : 1;
        if (t1.month != t2.month) return (t1.month > t2.month) ? -1 : 1;
        if (t1.day != t2.day)     return (t1.day > t2.day) ? -1 : 1;
        if (t1.hour != t2.hour)   return (t1.hour > t2.hour) ? -1 : 1;
        if (t1.min != t2.min)     return (t1.min > t2.min) ? -1 : 1;
        if (t1.sec != t2.sec)     return (t1.sec > t2.sec) ? -1 : 1;
        return 0;
    }
};

/////////////////////////////////////////////////////////
// SmallDateTime (32-bit, Minute Precision, SQL Compatible)
/////////////////////////////////////////////////////////
#define SMALLDATETIME_YEAR_BASE (WORD)1900

struct SmallDateTime
{
    union
    {
        DWORD dwTime;
        struct
        {
            DWORD year  : 8;    // 1900 ~ 2155
            DWORD month : 4;    // 1 ~ 12
            DWORD day   : 5;    // 1 ~ 31
            DWORD hour  : 5;    // 0 ~ 23
            DWORD min   : 6;    // 0 ~ 59
            DWORD pad   : 4;    // Alignment padding
        };
    };

    SmallDateTime() { dwTime = 0; }
    SmallDateTime(DWORD t) { dwTime = t; }
    SmallDateTime(const SYSTEMTIME& stime)
    {
        year  = (stime.wYear - SMALLDATETIME_YEAR_BASE);
        month = stime.wMonth;
        day   = stime.wDay;
        hour  = stime.wHour;
        min   = stime.wMinute;
        pad   = 0;
    }

    void ConvertTimeTo(SYSTEMTIME& stime) const
    {
        memset(&stime, 0, sizeof(SYSTEMTIME));
        stime.wYear   = (WORD)(year + SMALLDATETIME_YEAR_BASE);
        stime.wMonth  = (WORD)month;
        stime.wDay    = (WORD)day;
        stime.wHour   = (WORD)hour;
        stime.wMinute = (WORD)min;
        stime.wSecond = 0;
    }
};

/////////////////////////////////////////////////////////
// FullDateTime (64-bit, Millisecond Precision)
/////////////////////////////////////////////////////////
#define FULLDATETIME_YEAR_BASE (WORD)1900

struct FullDateTime
{
    union
    {
        unsigned __int64 qwTime; // Use native VC80 64-bit keyword
        struct
        {
            unsigned __int64 year  : 12;  // 0 ~ 4095
            unsigned __int64 month : 4;   // 1 ~ 12
            unsigned __int64 day   : 5;   // 1 ~ 31
            unsigned __int64 hour  : 5;   // 0 ~ 23
            unsigned __int64 min   : 6;   // 0 ~ 59
            unsigned __int64 sec   : 6;   // 0 ~ 59
            unsigned __int64 ms    : 10;  // 0 ~ 1023 (for milliseconds)
            unsigned __int64 pad   : 16;  // Reserved
        };
    };

    FullDateTime() { qwTime = 0; }
    FullDateTime(unsigned __int64 t) { qwTime = t; }
    FullDateTime(const SYSTEMTIME& stime)
    {
        year  = (stime.wYear - FULLDATETIME_YEAR_BASE);
        month = stime.wMonth;
        day   = stime.wDay;
        hour  = stime.wHour;
        min   = stime.wMinute;
        sec   = stime.wSecond;
        ms    = stime.wMilliseconds;
        pad   = 0;
    }

    void SetCurLocalTime()
    {
        SYSTEMTIME stime;
        ::GetLocalTime(&stime);
        year  = (stime.wYear - FULLDATETIME_YEAR_BASE);
        month = stime.wMonth;
        day   = stime.wDay;
        hour  = stime.wHour;
        min   = stime.wMinute;
        sec   = stime.wSecond;
        ms    = stime.wMilliseconds;
    }

    void ConvertTimeTo(SYSTEMTIME& stime) const
    {
        memset(&stime, 0, sizeof(SYSTEMTIME));
        stime.wYear         = (WORD)(year + FULLDATETIME_YEAR_BASE);
        stime.wMonth        = (WORD)month;
        stime.wDay          = (WORD)day;
        stime.wHour         = (WORD)hour;
        stime.wMinute       = (WORD)min;
        stime.wSecond       = (WORD)sec;
        stime.wMilliseconds = (WORD)ms;
    }
};
#pragma pack(pop)

struct RegionInfo
{
    short RegionID;
    short RegionBlockID;
    float PosX;
    float PosY;
    float PosZ;

    RegionInfo(short wRID = 0, float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
        : RegionID(wRID), RegionBlockID(0), PosX(X), PosY(Y), PosZ(Z)
    {}
};

struct TinyTime // smalldatetime (4 bytes)
{
    unsigned short days;    // Days since 1900-01-01
    unsigned short minutes; // Minutes since midnight
};

#pragma pack(push, 1)
struct sSlotInfo
{
    class CGItem* pItem;
    WORD wQuantity;
    WORD wMaxStack;
    BYTE btSlot;

    sSlotInfo() : pItem(NULL), wQuantity(0), wMaxStack(0), btSlot(0)
    {}
};
#pragma pack(pop)

struct sDbConfigs
{
    std::string ACCOUNT;
    std::string SHARD;
    std::string LOG;

    sDbConfigs() : ACCOUNT(""), SHARD(""), LOG("")
    {}
};

struct sVanguard
{
    std::wstring INI;
    std::wstring DLL;

    sVanguard() : INI(L""), DLL(L"")
    {}
};

struct sMacAddress
{
    BYTE b1;
    BYTE b2;
    BYTE b3;
    BYTE b4;
    BYTE b5;
    BYTE b6;

    std::string ToString() const
    {
        char szBuffer[18];
        sprintf_s(szBuffer, sizeof(szBuffer), "%02X:%02X:%02X:%02X:%02X:%02X", b1, b2, b3, b4, b5, b6);
        return std::string(szBuffer);
    }

    sMacAddress(BYTE p1 = 0, BYTE p2 = 0, BYTE p3 = 0, BYTE p4 = 0, BYTE p5 = 0, BYTE p6 = 0)
        : b1(p1), b2(p2), b3(p3), b4(p4), b5(p5), b6(p6)
    {}

    sMacAddress(const BYTE (&byte)[6])
    {
        b1 = byte[0]; b2 = byte[1]; b3 = byte[2]; b4 = byte[3]; b5 = byte[4]; b6 = byte[5];
    }
};
typedef sMacAddress MacAddress;

namespace E_MODULE
{
    enum Type
    {
        AgentServer,
        DownloadServer,
        FarmManager,
        GatewayServer,
        GlobalManager,
        MachineManager,
        SR_GameServer,
        SR_ShardManager,
        UNKNOWN
    };

    inline const char* GetModuleName(Type type)
    {
        switch (type)
        {
            case AgentServer:      return "AgentServer";
            case DownloadServer:   return "DownloadServer";
            case FarmManager:      return "FarmManager";
            case GatewayServer:    return "GatewayServer";
            case GlobalManager:    return "GlobalManager";
            case MachineManager:   return "MachineManager";
            case SR_GameServer:    return "SR_GameServer";
            case SR_ShardManager:  return "SR_ShardManager";
            default:               return "UNKNOWN";
        }
    }

    inline Type GetModuleType(const char* name)
    {
        if (name == 0)
            return UNKNOWN;

        if (std::strcmp(name, "AgentServer") == 0)     return AgentServer;
        if (std::strcmp(name, "DownloadServer") == 0)  return DownloadServer;
        if (std::strcmp(name, "FarmManager") == 0)     return FarmManager;
        if (std::strcmp(name, "GatewayServer") == 0)   return GatewayServer;
        if (std::strcmp(name, "GlobalManager") == 0)   return GlobalManager;
        if (std::strcmp(name, "MachineManager") == 0)  return MachineManager;
        if (std::strcmp(name, "SR_GameServer") == 0)   return SR_GameServer;
        if (std::strcmp(name, "SR_ShardManager") == 0) return SR_ShardManager;

        return UNKNOWN;
    }
};