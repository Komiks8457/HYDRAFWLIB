#pragma once

#ifndef MSG_H
#define MSG_H

#include "pch.h"
#include "MSG_ID_DEF.h"

//////////////////////////////////////
// (1) MsgSize field
//////////////////////////////////////

// masks for size field
#define MSG_MASK_SIZE_ONLY					static_cast<WORD>(0x7FFF)   // Changed from 0x000FFFFF
#define MSG_MASK_SIZEFIELD_FLAGS			static_cast<WORD>(0x8000)   // Changed from 0x80000000

// flags for size field
#define MSG_FLAG_SIZEFIELD_ENCRYPTED		static_cast<WORD>(0x8000)   // Changed from 0x80000000

/////////////////////////////////////
// Each Filed Length (in bytes)
/////////////////////////////////////
#define MSG_HEADER_SIZE                     6  // Changed from 15

// Each Field Length (in bytes)
#define MSG_OFFSET_SIZE                     0
#define MSG_FIELDLEN_SIZE                   2  // Changed from 4 (WORD)

#define MSG_OFFSET_ID                       2  // Stays 2, but now size ends here
#define MSG_FIELDLEN_ID                     2

#define MSG_OFFSET_SEQ                      4  // Changed from 6
#define MSG_FIELDLEN_SEQ                    1  // Changed from 8 (BYTE)

#define MSG_OFFSET_CHECKSUM                 5  // Changed from 14
#define MSG_FIELDLEN_CHECKSUM               1

#define DEFAULT_MSGBUF_SIZE		            4096

#ifndef SAFE_DELVEC
#define SAFE_DELVEC(x)	                    { delete [] (x); (x) = NULL; }
#endif


enum READ_MODE
{
    Reverse,
    Normal,
};

enum MsgException
{
    MsgReadException = 1,
    MsgReverseReadException,
    MsgTypeReadException,
    MsgWriteException,
    MsgOverwriteException,
};

// The wrapper class for VC80 / C++98
class CMsgException : public std::exception {
private:
    MsgException m_Code;

public:
    // Constructor
    CMsgException(MsgException code) throw() : m_Code(code) {}

    // Required by std::exception
    virtual const char* what() const throw()
    {
        switch (m_Code)
        {
            case MsgReadException:
                return "Failed to read data from the stream.";
            case MsgReverseReadException:
                return "Failed to perform a reverse-order read operation.";
            case MsgTypeReadException:
                return "Encountered an unknown or invalid message type ID.";
            case MsgWriteException:
                return "Failed to write data to the destination buffer or file.";
            case MsgOverwriteException:
                return "Attempted to overwrite a protected or non-empty message slot.";
            default:
                return "An undefined internal exception has occurred.";
        }
    }

    // Helper to get the original enum value back
    MsgException GetCode() const throw() { return m_Code; }
};

enum
{
    IO_ACCEPT = 0,
    IO_READ,
    IO_WRITE,
    IO_DISCONNECT,
    IO_CLOSED,
    IOTYPE_NUM
};

enum IO_TARGET
{
    IO_TARGET_UNKNOWN = -1,
    IO_TARGET_DATA = 0,
    IO_TARGET_FILE,
    IO_TARGET_NUM
};

enum IO_MODE
{
    IO_MODE_INVALID = -1,
    IO_MODE_DIRECT  = 0,
    IO_MODE_BUFFERED,
    IO_MODE_NUM
};

enum
{
    CONTEXT_TYPE_MSG = 0,
    CONTEXT_TYPE_CIRCULAR,
    CONTEXT_TYPE_NUM
};

class IIOContext
{
    typedef struct _BSWSABUF
    {
        unsigned long	len;
        char FAR *		buf;

    } BSWSABUF, FAR * LPBSWSABUF;

public:
    IIOContext() : m_nOperation(0), m_dwOPTarget(0), m_nType(0)//, m_nBufNum(0)
    {
        ::ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
        ::ZeroMemory(m_wsaBuf, sizeof(m_wsaBuf));
    }

    OVERLAPPED	m_Overlapped;

protected:
    long		m_nOperation;
    DWORD		m_dwOPTarget;
    long		m_nType;

public:
    //long		m_nBufNum;
    BSWSABUF	m_wsaBuf[2];

    void		SetContextType(long nType) { m_nType = nType; }
    long		GetContextType() const { return m_nType; }
    void        SetOperationMode(long nOperation) { m_nOperation = nOperation; }
    long		GetOperationMode() const { return m_nOperation; }
    DWORD		GetOperationTarget() const { return m_dwOPTarget; }
    void		SetOperationTarget( DWORD dwNewTarget) { m_dwOPTarget = dwNewTarget; }
};

/////////////////////////////////////
// CMsg
/////////////////////////////////////
class CMsg: public IIOContext {
private:
#define _GET_TYPED_VALUE(type)           \
    DWORD nLen = sizeof(type);           \
    if ((GetRdPos() + nLen) > GetWrPos()) \
       throw CMsgException(MsgTypeReadException); \
    type temp = *((type*)GetRdBuffer()); \
    IncRdPos(nLen);                      \
    return temp;

public:
    explicit CMsg(WORD MsgID = 0);
    virtual ~CMsg() { SAFE_DELVEC(m_pMassiveBuffer) }

    void	SetBuffer(BYTE* pBuffer, DWORD cap);
    void	Resize(DWORD wSize);

    ////////////////////////////////////////////////////
    // header relatives
    void	ClearHeader() { ::ZeroMemory(&m_pActiveBuffer[0], MSG_HEADER_SIZE);}
    void	SetMsgID(WORD wMsgID) { *m_pID = wMsgID; }
    WORD	AckMsgID() { return (WORD)*m_pID + 0x4000; }
    WORD	AckMsgID(WORD wMsgID) { return (WORD)*m_pID + wMsgID; }
    void	GetHeader(WORD& wMsgID, DWORD& wSize) { wMsgID = GetMsgID(); wSize = GetMsgSize(); }
    WORD	GetMsgID() { return (WORD)*m_pID; }
    DWORD	GetDataSize();
    DWORD	GetMsgSize()  { return GetDataSize() + MSG_HEADER_SIZE; }			// Header를 포함한 사이즈
    void	SetCheckSum(BYTE btCheckSum) { *m_pCheckSum = btCheckSum; }
    void	SetSequence(BYTE Seq) { *m_pSeq = Seq; }
    BYTE	GetCheckSum() { return *m_pCheckSum; }
    BYTE    GetSequence() { return *m_pSeq; }
    DWORD	GetAvailableBufSize() const	{ _ASSERT(m_nWrCur <= m_nBufCapacity); return m_nBufCapacity - m_nWrCur; }
    void	SetDataSizeAtHeader(DWORD wSize);

    ////////////////////////////////////////////////////
    // buffer offset
    BOOL	SetRdPos(DWORD wPos, BOOL bIsOffsetFromHeader = FALSE);
    BOOL	SetWrPos(DWORD wPos, BOOL bIsOffsetFromHeader = FALSE);
    DWORD	GetRdPos() const { return m_nRdCur; }
    DWORD	GetWrPos() const { return m_nWrCur; }
    void	IncRdPos(DWORD Pos) { m_nRdCur += Pos; }
    void	IncWrPos(DWORD Pos) { m_nWrCur += Pos; }
    void	DecRdPos(DWORD Pos) { m_nRdCur -= Pos; }
    void	DecWrPos(DWORD Pos) { m_nWrCur -= Pos; }
    void	ForceStopRead() { m_nRdCur = m_nWrCur; }
    BOOL	AllDataRead() const { return m_nRdCur == m_nWrCur; }
    DWORD	GetRemainBytesToRead() const { return m_nWrCur - m_nRdCur; }

    ////////////////////////////////////////////////////
    // reference count
    int		GetRefCnt() const { return m_nRefCnt; }
    long	AddRef() { return ::InterlockedIncrement(&m_nRefCnt); }
    long	DecRef()
    {
        if (m_nRefCnt > 0)	// 음수로 가는 일은 없도록...
        {
            ::InterlockedDecrement(&m_nRefCnt);
            return m_nRefCnt;
        }

        return 0;
    }

    ////////////////////////////////////////////////////
    // diagnostics
    BOOL	IsEncMsg();
    void	SetAsEncrypted();
    void	SetAsPlaneMsg();

    ////////////////////////////////////////////////////
    // buffer pointer
    void	ReadBytes(void* pBuf, DWORD nLen);
    void	WriteBytes(void* pBuf, DWORD nLen);
    void	ReadBytesReverse(void* pBuf, DWORD nLen);
    DWORD	GetCapacity() const { return m_nBufCapacity; }
    BYTE*	GetRdBuffer() { return m_pActiveBuffer + m_nRdCur; }
    BYTE*	GetWrBuffer() { return m_pActiveBuffer + m_nWrCur; }
    BYTE*	GetBufferAt(DWORD nOffset)
    {
        _ASSERT(nOffset < m_nBufCapacity);
        return &m_pActiveBuffer[nOffset];
    }
    void	PrepareOperation()
    {
        // 여기서 GetDataSize() 를 안쓰고 굳이 m_nWrCur를 사용한 이유는
        // Msg를 암호화 할 경우 Header에 들어있는 Plane Text상태의 메시지 사이즈는
        // 변하면 안되거든? 그런 관계로 m_nWrCur를 encrypt된 상태의 데이터 사이즈로 외부에서
        // 강제 세팅하고 사용하기 때문이다. 그렇지 않으면 실제 WSASend 호출될 때,
        // 암호화 되기 전의 사이즈 만큼만 날아가게 되거덩...
        m_wsaBuf[0].len = m_nWrCur;
    }

    ////////////////////////////////////////////////////
    // associated objects
    DWORD	GetSessionID() const { return m_dwSession; }
    void	SetSessionID(DWORD dwSession) { m_dwSession = dwSession; }

    void	SetThreadID(DWORD dwThreadID) { m_dwThreadID = dwThreadID; }
    DWORD	GetThreadID() const { return m_dwThreadID; }

    void	SetMsgInUse(long bInUse) { ::InterlockedExchange(&m_bIsInUse, bInUse); }
    BOOL	IsInUse() const { return m_bIsInUse; }

    void	Reset();
    void    Rewind() { m_nRdCur = MSG_HEADER_SIZE; }
    void    ResetPos() { m_nRdCur = MSG_HEADER_SIZE; m_nWrCur = (WORD)(GetDataSize() + MSG_HEADER_SIZE); }
    void	SetReadDirection(READ_MODE bForward)	{ m_bReadForward = bForward; }

    void	BeginPeek() { _ASSERT(m_nPeekPos == 0); m_nPeekPos = m_nRdCur; }
    void	EndPeek() { m_nRdCur = m_nPeekPos; m_nPeekPos = 0; }

    BOOL	IsReadForward() const { return m_bReadForward; }

    DWORD	CopyMsg(CMsg* pSrc, BOOL bUnreadOnly = TRUE);
    void    PrintDumpData(const char *funcName, bool full = false);
    void    WriteDumpData(const char *funcName, bool full = false);
    CMsg*	MsgAck(bool Encrypt = false);

    ////////////////////////////////////////////////////
    // Templates & Helpers
    ////////////////////////////////////////////////////

    // --- Writing ---
    template <typename T>
    void Write(const T& data) { WriteBytes((void*)&data, sizeof(T)); }

    template <>
    void Write<std::string>(const std::string& data) { *this << data; }

    void WriteStringA(const std::string& data) { *this << data; }

    template <>
    void Write<std::wstring>(const std::wstring& data) { *this << data; }

    void WriteStringW(const std::wstring& data) { *this << data; }

    // --- Reading ---
    template <typename T>
    void Read(T& data) { ReadBytesEx(data); }

    template <typename T>
    T Read()
    {
        T data;
        ReadBytesEx(data);
        return data;
    }

    template <>
    void Read<std::string>(std::string& data) { *this >> data; }
    void ReadStringA(std::string& data) { this->Read<std::string>(data); }
    std::string ReadStringA() { std::string temp; *this >> temp; return temp; }

    template <>
    void Read<std::wstring>(std::wstring& data) { *this >> data; }
    void ReadStringW(std::wstring& data) { this->Read<std::wstring>(data); }
    std::wstring ReadStringW() { std::wstring temp; *this >> temp; return temp; }

    // --- Internal Logic ---
    template <class T>
    void ReadBytesEx(T& arg)
    {
        DWORD nLen = sizeof(T);
        if (GetRdPos() + nLen > GetWrPos()) throw CMsgException(MsgReadException);
        arg = *(T*)GetRdBuffer();
        IncRdPos(nLen);
    }

    template <class T>
    void ReadBytesReverseEx(T& arg)
    {
        DWORD nLen = sizeof(T);
        if (nLen > GetWrPos() - MSG_HEADER_SIZE)
            throw CMsgException(MsgReverseReadException);

        DecWrPos(nLen);
        arg = *(T*)GetWrBuffer();

        // Update the header size since we "removed" data from the end
        SetDataSizeAtHeader(GetWrPos() - MSG_HEADER_SIZE);
    }

    template <class T>
    void Overwrite(T arg, DWORD nPos)
    {
        if (nPos + sizeof(T) > GetWrPos())
            throw CMsgException(MsgOverwriteException);

        *(T*)GetBufferAt(nPos) = arg;
    }

    // --- Operators ---
    template <class T>
    CMsg& operator << (const T& arg) { Write(arg); return *this; }

    template <class T>
    CMsg& operator >> (T& arg)
    {
        if (IsReadForward())
            ReadBytesEx(arg);
        else
            ReadBytesReverseEx(arg);
        return *this;
    }

    // --- String Implementations (ASCII) ---
    CMsg& operator << (const char* rhs)
    {
        WORD wLen = rhs == NULL ? 0 : (WORD)::strlen(rhs);
        WriteBytes(&wLen, sizeof(WORD));
        if (wLen > 0) WriteBytes((void*)rhs, wLen);
        return *this;
    }

    template <>
    CMsg& operator << (const std::string& arg) { return *this << arg.c_str(); }

    template <>
    CMsg& operator >> (std::string& arg)
    {
        WORD wLen = 0;
        ReadBytes(&wLen, sizeof(WORD));
        if (wLen > 0) {
            if ((DWORD)wLen > GetRemainBytesToRead()) wLen = (WORD)GetRemainBytesToRead();
            arg.resize(wLen);
            ReadBytes((void*)&arg[0], wLen);
        } else arg.clear();
        return *this;
    }

    // --- String Implementations (Unicode) ---
    CMsg& operator << (const wchar_t* rhs)
    {
        WORD wLen = rhs == NULL ? 0 : (WORD)::lstrlenW(rhs);
        WriteBytes(&wLen, sizeof(WORD));
        if (wLen > 0) WriteBytes((void*)rhs, wLen * sizeof(WCHAR));
        return *this;
    }

    template <>
    CMsg& operator << (const std::wstring& arg) { return *this << arg.c_str(); }

    template <>
    CMsg& operator >> (std::wstring& arg)
    {
        WORD wLen = 0;
        ReadBytes(&wLen, sizeof(WORD));
        if (wLen > 0) {
            DWORD maxChars = GetRemainBytesToRead() / sizeof(WCHAR);
            if ((DWORD)wLen > maxChars) wLen = (WORD)maxChars;
            arg.resize(wLen);
            ReadBytes((void*)&arg[0], wLen * sizeof(WCHAR));
        } else arg.clear();
        return *this;
    }

    int8_t      ReadInt8()                     { _GET_TYPED_VALUE(int8_t) }
    int16_t     ReadInt16()                    { _GET_TYPED_VALUE(int16_t) }
    int32_t     ReadInt32()                    { _GET_TYPED_VALUE(int32_t) }
    long32_t    ReadLong32()                   { _GET_TYPED_VALUE(long32_t) }
    int64_t     ReadInt64()                    { _GET_TYPED_VALUE(int64_t) }
    uint8_t     ReadUInt8()                    { _GET_TYPED_VALUE(uint8_t) }
    uint16_t    ReadUInt16()                   { _GET_TYPED_VALUE(uint16_t) }
    uint32_t    ReadUInt32()                   { _GET_TYPED_VALUE(uint32_t) }
    ulong32_t   ReadULong32()                  { _GET_TYPED_VALUE(ulong32_t) }
    uint64_t    ReadUInt64()                   { _GET_TYPED_VALUE(uint64_t) }
    float       ReadFloat()                    { _GET_TYPED_VALUE(float) }
    double      ReadDouble()                   { _GET_TYPED_VALUE(double) }

    void        WriteInt8(int8_t val)          { WriteBytes(&val, sizeof(int8_t)); }
    void        WriteInt16(int16_t val)        { WriteBytes(&val, sizeof(int16_t)); }
    void        WriteInt32(int32_t val)        { WriteBytes(&val, sizeof(int32_t)); }
    void        WriteLong32(long32_t val)      { WriteBytes(&val, sizeof(long32_t)); }
    void        WriteInt64(int64_t val)        { WriteBytes(&val, sizeof(int64_t)); }
    void        WriteUInt8(uint8_t val)        { WriteBytes(&val, sizeof(uint8_t)); }
    void        WriteUInt16(uint16_t val)      { WriteBytes(&val, sizeof(uint16_t)); }
    void        WriteUInt32(uint32_t val)      { WriteBytes(&val, sizeof(uint32_t)); }
    void        WriteULong32(ulong32_t val)    { WriteBytes(&val, sizeof(ulong32_t)); }
    void        WriteUInt64(uint64_t val)      { WriteBytes(&val, sizeof(uint64_t)); }
    void        WriteFloat(float val)          { WriteBytes(&val, sizeof(float)); }
    void        WriteDouble(double val)        { WriteBytes(&val, sizeof(double)); }

    ////////////////////////////////////////////////////

protected:
    BYTE	    m_Buffer[DEFAULT_MSGBUF_SIZE];  // 0x0032 / 52
    BYTE*	    m_pActiveBuffer;                // 0x1034
    BYTE*	    m_pMassiveBuffer;               // 0x1038
    WORD	    m_nRdCur;		                // 0x103C Header사이즈를 포함한 Offset
    WORD	    m_nWrCur;		                // 0x103E Header사이즈를 포함한 Offset
    DWORD	    m_nPeekPos;                     // 0x1040
    LONG	    m_nRefCnt;                      // 0x1044
    READ_MODE	m_bReadForward;                 // 0x1048
    DWORD	    m_nBufCapacity;                 // 0x104C

    // for msg_header direct accessing
    WORD*	    m_pID;				            // 0x1050 / 4176 Msg ID
    WORD*	    m_pDataSize;		            // 0x1054 / 4180 Msg Size ( Not Header )
    BYTE*	    m_pSeq;			                // 0x1058 / 4184 Msg Sequence
    BYTE*	    m_pCheckSum;		            // 0x105C / 4188 Msg CheckSum

    // temporary associated object
    DWORD	    m_dwSession;		            // 0x1060 SessionID
    DWORD	    m_dwThreadID;                   // 0x1064
    LONG	    m_bIsInUse;                     // 0x1068
    CHAR        unk_106C[16];                   // 0x106C not needed
};

#endif
