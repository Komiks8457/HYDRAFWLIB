#pragma once

#include "Msg.h"

#define NEWMSG(MsgID, Encrypt) \
    g_pNetEngine->NewMsg(MsgID, Encrypt)

#define NEWMSGFROM(MsgID, File, Line, FuncName, Encrypt) \
    g_pNetEngine->NewMsgFrom_IMPL(MsgID, File, Line, FuncName, Encrypt)

#define DELMSG(pMsg) \
    g_pNetEngine->DelMsg(pMsg)

#define SENDMSG(SessionID, pMsg) \
    g_pNetEngine->SendMsg(SessionID, pMsg)

#define GETMAC(SessionID) \
    g_pNetEngine->GetMacAddress(SessionID)

struct __declspec(novtable) IUnknownInterface
{
    virtual HRESULT	__stdcall QueryInterface(REFIID riid, LPVOID FAR * ppvObj) = 0;
    virtual ULONG	__stdcall AddRef()  = 0;
    virtual ULONG	__stdcall Release() = 0;
};

struct __declspec(novtable) IBSNet : public IUnknownInterface
{
    // IUnknown methods
    virtual HRESULT	__stdcall QueryInterface(REFIID riid, LPVOID FAR * ppvObj) = 0;
    virtual ULONG	__stdcall AddRef()  = 0;
    virtual ULONG	__stdcall Release() = 0;

    enum NERR
    {
        NERR_PEER_DOESNT_RESPONSE = 32789,
        NERR_FAILED_TO_CREATE_SESSION = 32788,
        NERR_TARGET_TASK_LOST = 32787,
        NERR_COMPLETION_TARGET_MISMATCH = 32786,
        NERR_INVALID_MSG_HEADER = 32785,
        NERR_NET_ACCESSED_BEFORE_HANDSHAKE = 32784,
        NERR_INVALID_TARGET_FILE = 32783,
        NERR_FAILED_TO_ACCEPT = 32782,
        NERR_INVALID_MSGSIZE = 32781,
        NERR_INVALID_CHECKSUM = 32780,
        NERR_INVALID_SEQUENCE = 32779,
        NERR_FAILED_TO_TRANSFILE = 32778,
        NERR_INVALID_SESSION = 32777,
        NERR_INVALID_SOCKET = 32776,
        NERR_NO_MSGTARGET = 32775,
        NERR_NODATA_TO_SEND = 32774,
        NERR_TOO_MANY_OUTSTANDING = 32773,
        NERR_BUFFER_FULL = 32772,
        NERR_SESSION_LOST = 32771,
        NERR_UNKNOWN = 32770,
        NERR_TIMEOUT = 32769,
        NERR_GOOD = 0,
    };
};

struct NETENGINE_CONFIG
{};

class CNetEngine : public IBSNet {
public:
    static bool     Initialize(void* ptr);
    CMsg*           NewMsg(WORD wMsgID, bool IsEncrypted);
    CMsg*           NewMsgFrom(WORD wMsgID, const char* szFile, BOOL nLine, const char* szFnName, bool IsEncrypted);
    void            DelMsg(CMsg* pMsg);
    NERR            SendMsg(DWORD dwSessionID, CMsg *pMsg);
    MacAddress      GetMacAddress(DWORD dwSession) const;

private:
    virtual ~CNetEngine();
    virtual HRESULT	__stdcall QueryInterface(REFIID riid, LPVOID FAR * ppvObj) = 0;
    virtual ULONG	__stdcall AddRef()  = 0;
    virtual ULONG	__stdcall Release() = 0;

    /*virtual BOOL    __stdcall Create(NETENGINE_CONFIG &config) = 0;
    virtual BOOL    __stdcall DummyFunc_04(BOOL a2, BOOL a3, BOOL a4, BOOL a5) = 0;
    virtual BOOL    __stdcall Connect(LPCWSTR lpszAddrConnect, WORD nPort, LPCWSTR lpszAddrBind, DWORD dwTaskToBind, BOOL bKeepAlive) = 0;
    virtual BOOL    __stdcall DummyFunc_06() = 0;
    virtual BOOL    __stdcall DummyFunc_07() = 0;
    virtual BOOL    __stdcall DummyFunc_08() = 0;
    virtual BOOL    __stdcall DummyFunc_09() = 0;
    virtual BOOL    __stdcall DummyFunc_10() = 0;
    virtual BOOL    __stdcall DummyFunc_11() = 0;
    virtual BOOL    __stdcall DummyFunc_12() = 0;
    virtual BOOL    __stdcall DummyFunc_13() = 0;
    virtual BOOL    __stdcall DummyFunc_14() = 0;
    virtual BOOL    __stdcall DummyFunc_15() = 0;
    virtual BOOL    __stdcall DummyFunc_16() = 0;
    virtual BOOL    __stdcall DummyFunc_17() = 0;
    virtual CMsg*   __stdcall NewMsg(bool bEncrypt) = 0; //18
    virtual CMsg*   __stdcall NewMsgFrom(const char* szFile, BOOL nLine, const char* szFnName, bool bEncrypt) = 0; //19
    virtual void    __stdcall DelMsg(CMsg* pMsg) = 0; //20
    virtual BOOL    __stdcall DummyFunc_21() = 0;
    virtual NERR    __stdcall SendMsg(DWORD dwSession, CMsg* pMsg) = 0; //22
    virtual BOOL    __stdcall DummyFunc_23() = 0;
    virtual BOOL    __stdcall DummyFunc_24() = 0;
    virtual BOOL    __stdcall DummyFunc_25() = 0;
    virtual BOOL    __stdcall DummyFunc_26() = 0;
    virtual BOOL    __stdcall DummyFunc_27() = 0;
    virtual BOOL    __stdcall DummyFunc_28() = 0;
    virtual BOOL    __stdcall DummyFunc_29() = 0;
    virtual BOOL    __stdcall DummyFunc_30() = 0;
    virtual BOOL    __stdcall DummyFunc_31() = 0;
    virtual BOOL    __stdcall DummyFunc_32() = 0;
    virtual BOOL    __stdcall DummyFunc_33() = 0;
    virtual BOOL    __stdcall DummyFunc_34() = 0;
    virtual BOOL    __stdcall DummyFunc_35() = 0;
    virtual BOOL    __stdcall GetMacAddress(DWORD dwSession, BYTE* pAddress) = 0; //36
    virtual BOOL    __stdcall DummyFunc_37() = 0;
    virtual void    __stdcall DisplayVersion(int a1) = 0; //38
    virtual BOOL    __stdcall DummyFunc_39() = 0;
    virtual BOOL    __stdcall DummyFunc_40() = 0;
    virtual BOOL    __stdcall DummyFunc_41() = 0;
    virtual BOOL    __stdcall DummyFunc_42() = 0;
    virtual void    __stdcall RegisterMaxConnections(int, int) = 0;*/
//--------------------------------------------------------------------
    typedef std::map<int, DWORD> vfTableMap;
    static vfTableMap m_vftableMap;
};

extern CNetEngine *g_pNetEngine;
