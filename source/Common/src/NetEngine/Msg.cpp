#include "Msg.h"
#include "AppLogger.h"
#include "Logger/Logger.h"
#include "NetEngine/NetEngine.h"

CMsg::CMsg(WORD MsgID)
{
    m_nType             = CONTEXT_TYPE_MSG;
    m_nOperation        = IO_WRITE;
    m_dwOPTarget        = IO_TARGET_DATA;

    m_pActiveBuffer     = NULL;
    m_pMassiveBuffer    = NULL;
    m_bReadForward      = Normal;
    m_nRefCnt           = 0;
    m_nPeekPos          = 0;
    m_bIsInUse          = FALSE;
    m_dwSession         = 0;
    m_dwThreadID        = 0;

    SetBuffer(m_Buffer, DEFAULT_MSGBUF_SIZE);

    m_wsaBuf[0].buf     = (LPSTR)m_pActiveBuffer; // Now points to valid m_Buffer memory!

    m_nRdCur            = MSG_HEADER_SIZE;
    m_nWrCur            = MSG_HEADER_SIZE;

    *m_pID              = MsgID;
}

void CMsg::SetBuffer(BYTE* pBuffer, DWORD cap)
{
    if (pBuffer == NULL || m_pActiveBuffer == pBuffer)
        return;

    BYTE* pOldBuffer = m_pActiveBuffer; // Store the current buffer before switching
    m_pActiveBuffer = pBuffer;
    m_nBufCapacity  = cap;

    // 1. Remap header field pointers to the new buffer location
    m_pDataSize = (WORD*)&m_pActiveBuffer[MSG_OFFSET_SIZE];
    m_pID       = (WORD*)&m_pActiveBuffer[MSG_OFFSET_ID];
    m_pSeq      = (BYTE*)&m_pActiveBuffer[MSG_OFFSET_SEQ];
    m_pCheckSum = (BYTE*)&m_pActiveBuffer[MSG_OFFSET_CHECKSUM];

    // 2. Copy the header from the old buffer to the new one
    // This ensures your MsgID and Size aren't wiped out during a Resize
    if (pOldBuffer != NULL)
    {
        ::memcpy(m_pActiveBuffer, pOldBuffer, MSG_HEADER_SIZE);
    }

    m_wsaBuf[0].buf = (char*)pBuffer;

    // just filling it with 0's
    memset(unk_106C, 0, sizeof(unk_106C));
}

void CMsg::Resize(DWORD wSize)
{
    // Ensure we don't exceed the protocol's 15-bit size limit (32,767 bytes)
    _ASSERT(wSize <= MSG_MASK_SIZE_ONLY);

    BYTE* pNewBuffer = new BYTE[wSize];

    // Copy the current contents (Header + Data) to the new memory
    // We copy m_nWrCur bytes because that represents the total used space
    ::memcpy(pNewBuffer, m_pActiveBuffer, m_nWrCur);

    // Update internal pointers to the new buffer
    SetBuffer(pNewBuffer, wSize);

    // Clang-Tidy: 'if' removed. delete[] handles NULL internally.
    delete [] m_pMassiveBuffer;

    m_pMassiveBuffer = pNewBuffer;
}

DWORD CMsg::GetDataSize()
{
    return ((*m_pDataSize) & MSG_MASK_SIZE_ONLY);
}

BOOL CMsg::SetRdPos(DWORD wPos, BOOL bIsOffsetFromHeader)
{
    DWORD nTargetPos;

    if (bIsOffsetFromHeader == TRUE)
    {
        // wPos 0 means start right after the 6-byte header
        nTargetPos = MSG_HEADER_SIZE + wPos;
    }
    else
    {
        nTargetPos = wPos;
    }

    // CRITICAL FIX: Must not exceed the current Write Position (actual data end)
    // Also ensure we don't set a position before the header starts
    if (nTargetPos < MSG_HEADER_SIZE || nTargetPos > m_nWrCur)
    {
        // If we are resetting to start (0, TRUE), but no data was written yet,
        // m_nWrCur will be exactly MSG_HEADER_SIZE.
        if (nTargetPos == MSG_HEADER_SIZE && m_nWrCur == MSG_HEADER_SIZE)
        {
            m_nRdCur = (WORD)nTargetPos;
            return TRUE;
        }
        return FALSE;
    }

    m_nRdCur = (WORD)nTargetPos;
    return TRUE;
}

BOOL CMsg::SetWrPos(DWORD wPos, BOOL bIsOffsetFromHeader)
{
    DWORD nTargetPos = bIsOffsetFromHeader ? (MSG_HEADER_SIZE + wPos) : wPos;

    // Check 1: Physical buffer limits
    if (nTargetPos > m_nBufCapacity)
        return FALSE;

    // Check 2: Header protection
    if (nTargetPos < MSG_HEADER_SIZE)
        return FALSE;

    m_nWrCur = (WORD)nTargetPos;

    // IMPORTANT: When you move the write pointer,
    // you must update the size in the header immediately.
    SetDataSizeAtHeader(m_nWrCur - MSG_HEADER_SIZE);

    return TRUE;
}

BOOL CMsg::IsEncMsg()
{
    return ((*m_pDataSize) & MSG_FLAG_SIZEFIELD_ENCRYPTED) != 0;
}

void CMsg::SetAsEncrypted()
{
    (*m_pDataSize) |= MSG_FLAG_SIZEFIELD_ENCRYPTED;
}

void CMsg::SetAsPlaneMsg()
{
    (*m_pDataSize) &= ~MSG_FLAG_SIZEFIELD_ENCRYPTED;
}

void CMsg::Reset()
{
    m_bReadForward	= Normal;

    m_nRefCnt		= 0;
    m_dwSession		= 0;
    m_dwThreadID	= 0;

    m_nRdCur	    = m_nWrCur = MSG_HEADER_SIZE;
    m_nPeekPos	    = 0;

    //m_bIsInUse	= FALSE;

    if (m_bIsInUse == TRUE)
    {
        _asm int 3
        PutLog(FATAL, _T("someone about to reset msg in use !!! [0x%x]"), GetMsgID());
    }

    SetBuffer(m_Buffer, DEFAULT_MSGBUF_SIZE);

    ClearHeader();

    SAFE_DELVEC(m_pMassiveBuffer)
}

void CMsg::SetDataSizeAtHeader(DWORD wSize)
{
    //ASSERTA0(wSize <= MSG_MASK_SIZE_ONLY, _T("Reset() 함수가 호출되지 않고 Msg가 사용된 듯 하다...") );
    *m_pDataSize = ((*m_pDataSize & MSG_MASK_SIZEFIELD_FLAGS) | wSize);
}

void CMsg::ReadBytesReverse(void* pBuf, DWORD nLen)
{
    // Ensure we don't try to read back into the 6-byte header
    if (nLen > (m_nWrCur - MSG_HEADER_SIZE))
        throw CMsgException(MsgReverseReadException);

    m_nWrCur = m_nWrCur - nLen;

    BYTE* pCur = GetWrBuffer();
    ::memcpy(pBuf, pCur, nLen);

    // CRITICAL: Update the header size after removing bytes
    SetDataSizeAtHeader(m_nWrCur - MSG_HEADER_SIZE);
}

void CMsg::ReadBytes(void* pBuf, DWORD nLen)
{
    if ((m_nRdCur + nLen) > m_nWrCur)
        throw CMsgException(MsgReadException);

    BYTE* pCur = GetRdBuffer();
    ::memcpy(pBuf, pCur, nLen);
    m_nRdCur = m_nRdCur + nLen;
}

void CMsg::WriteBytes(void* pBuf, DWORD nLen)
{
    if ((m_nWrCur + nLen) > (MSG_MASK_SIZE_ONLY + MSG_HEADER_SIZE))
        throw CMsgException(MsgWriteException);

    if (m_nWrCur + nLen > m_nBufCapacity)
    {
        DWORD buffer_count = (nLen / DEFAULT_MSGBUF_SIZE) + 1;
        DWORD nNewCapacity = m_nBufCapacity + (buffer_count * DEFAULT_MSGBUF_SIZE);

        BYTE* pNewRawBuffer = new BYTE[nNewCapacity];
        ::memcpy(pNewRawBuffer, m_pActiveBuffer, m_nWrCur);

        SetBuffer(pNewRawBuffer, nNewCapacity);

        // Clang-Tidy fix: removed unnecessary if-check
        delete [] m_pMassiveBuffer;
        m_pMassiveBuffer = pNewRawBuffer;
    }

    BYTE* pCur = GetWrBuffer();
    if (pBuf && nLen > 0)
    {
        ::memcpy(pCur, pBuf, nLen);
        m_nWrCur = m_nWrCur + (WORD)nLen;
    }

    SetDataSizeAtHeader(m_nWrCur - MSG_HEADER_SIZE);
}

DWORD CMsg::CopyMsg(CMsg* pSrc, BOOL bUnreadOnly)
{
    if (!pSrc) return 0;

    BYTE* pSourceBuf = NULL;
    DWORD wDataSizeToCopy; // Use 0 instead of NULL for DWORD

    if (bUnreadOnly)
    {
        wDataSizeToCopy = pSrc->GetRemainBytesToRead();
        pSourceBuf = pSrc->GetRdBuffer();
    }
    else
    {
        wDataSizeToCopy = pSrc->GetDataSize();
        pSourceBuf = pSrc->GetBufferAt(MSG_HEADER_SIZE);
    }

    if (wDataSizeToCopy > 0 && pSourceBuf != NULL)
    {
        WriteBytes(pSourceBuf, wDataSizeToCopy);
    }

    pSrc->ForceStopRead();

    return wDataSizeToCopy;
}

void CMsg::PrintDumpData(const char* funcName, bool full)
{
    DWORD totalBytes = GetWrPos();
    DWORD skipBytes = full ? 0 : 6;
    DWORD displayLength = (totalBytes > skipBytes) ? (totalBytes - skipBytes) : 0;
    BYTE* pPayload = m_pActiveBuffer + skipBytes;

    Logger::warn("[%s] MsgID: 0x%04X, Encrypted: %s, Length: %u, SessionID: %d",
                 funcName,
                 GetMsgID(),
                 IsEncMsg() ? "YES" : "NO",
                 displayLength,
                 m_dwSession);

    for (DWORD i = 0; i < displayLength; i += 16)
    {
        // 1. Print Offset
        printf("%04X: ", (unsigned int)i);

        // 2. Print Hex bytes
        for (DWORD j = 0; j < 16; j++)
        {
            if (i + j < displayLength)
                printf("%02X ", pPayload[i + j]);
            else
                printf("   ");
        }

        printf(" ");

        // 3. Print ASCII representation
        for (DWORD j = 0; j < 16; j++)
        {
            if (i + j < displayLength)
            {
                BYTE b = pPayload[i + j];
                printf("%c", isprint(b) ? b : '.');
            }
        }
        printf("\n");
    }

    printf("-------------------------------------------------------------------\n");
}

void CMsg::WriteDumpData(const char* funcName, bool full)
{
    WORD  wMsgID = GetMsgID();
    DWORD totalBytes = GetWrPos();
    DWORD skipBytes = full ? 0 : 6;

    if (totalBytes <= skipBytes)
        return;

    DWORD displayLength = totalBytes - skipBytes;
    BYTE* pPayload = m_pActiveBuffer + skipBytes;

    std::string sanitizedName = funcName ? funcName : "UnknownFunc";
    size_t pos = 0;
    while ((pos = sanitizedName.find("::", pos)) != std::string::npos) {
        sanitizedName.replace(pos, 2, "__");
        pos += 2;
    }

    char idBuffer[16];
    _snprintf(idBuffer, sizeof(idBuffer), "_0x%04X", wMsgID);
    std::string fileName = sanitizedName + idBuffer + ".log";

    std::ofstream outFile(fileName.c_str(), std::ios::app);
    if (!outFile.is_open()) return;

    char headerBuf[256];
    _snprintf(headerBuf, sizeof(headerBuf), "[%s] MsgID: 0x%04X, Encrypted: %s, Length: %u, SessionID: %d\n",
              funcName, wMsgID, IsEncMsg() ? "YES" : "NO", displayLength, m_dwSession);
    outFile << headerBuf;

    for (DWORD i = 0; i < displayLength; i += 16)
    {
        char rowHeader[16];
        _snprintf(rowHeader, sizeof(rowHeader), "%04X: ", i);
        outFile << rowHeader;

        for (DWORD j = 0; j < 16; j++)
        {
            if (i + j < displayLength) {
                char hex[4];
                _snprintf(hex, sizeof(hex), "%02X ", pPayload[i + j]);
                outFile << hex;
            } else {
                outFile << "   ";
            }
        }

        outFile << " ";

        for (DWORD j = 0; j < 16; j++)
        {
            if (i + j < displayLength) {
                BYTE b = pPayload[i + j];
                outFile << (char)(isprint(b) ? b : '.');
            }
        }
        outFile << "\n";
    }

    outFile << "-------------------------------------------------------------------\n";
    outFile.close();
}

CMsg* CMsg::MsgAck(bool Encrypt)
{
    CMsg* pReplyMsg = NEWMSG(AckMsgID(), Encrypt);
    return pReplyMsg;
}
