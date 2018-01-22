#pragma once

#define HEARTEBEAT_TIME 5000

enum emMessType
{
  // Client
  heartbeat = 0 ,
  cmdtext ,
  screenbmp ,
  processtext ,
  processkillsecc ,
  processkillfail ,
  filedisk ,
  filelist ,

  // Server
  cmdstart = 100 ,
  cmdstext ,
  screenrecv ,
  processrequest ,
  processkill ,
  filediskrequest ,
  filerequest ,
  filerootrequest ,
  filepathrequest ,
};

enum emTimerType
{
  // client
  tHeartbeat = 0 ,

  // server
  tsHeartBeat = 100 ,
};

class CFileDlg;
class CTaskDlg;
class CScreen;
class CCmdDlg;

typedef struct _SocketInfo
{
  CFileDlg* pFileWnd;
  CTaskDlg* pTaskWnd;
  CScreen*  pScreenWnd;
  CCmdDlg*  pCmdWnd;
  BOOL      isHeart;
  CCriticalSection csSend;
  inline _SocketInfo()
    : pFileWnd(NULL)
    , pTaskWnd(NULL)
    , pScreenWnd(NULL)
    , pCmdWnd(NULL)
    , isHeart(FALSE)
  {}
} SocketInfo, *pSocketInfo;


inline char* RecivPacket(SOCKET sSocket)
{
    DWORD dwPacketSize = 0;
    BYTE eDataType;

    //����DWORD�ĳ���
    int nTotalRecivByte = 0;
    int nRealRecivByte = 0;
    int nShouldReciveByte = sizeof(DWORD);
    while (nTotalRecivByte < nShouldReciveByte)
    {
        nTotalRecivByte =
            recv(sSocket, (char*)&dwPacketSize + nRealRecivByte, sizeof(DWORD)-nTotalRecivByte, 0);
        if (nRealRecivByte == SOCKET_ERROR)
        {
            return NULL;
        }
        nTotalRecivByte += nRealRecivByte;
    }

    //�������ݰ�
    char* packet = new char[dwPacketSize + sizeof(DWORD)];
    *(DWORD*)packet = dwPacketSize;

    //����eunm�������ͳ���
    nTotalRecivByte = 0;
    nRealRecivByte = 0;
    nShouldReciveByte = sizeof(BYTE);
    while (nTotalRecivByte < nShouldReciveByte)
    {
        nRealRecivByte =
            recv(sSocket, (char*)&eDataType + nRealRecivByte, sizeof(BYTE)-nTotalRecivByte, 0);
        if (nRealRecivByte == SOCKET_ERROR)
        {
            return NULL;
        }
        nTotalRecivByte += nRealRecivByte;
    }
    *(BYTE*)(packet + sizeof(DWORD)) = eDataType;

    //�������ݳ���
    if(dwPacketSize > sizeof(BYTE))
    {
      nTotalRecivByte = 0;
      nRealRecivByte = 0;
      nShouldReciveByte = dwPacketSize - sizeof(BYTE);
      while (nTotalRecivByte < nShouldReciveByte)
      {
          nRealRecivByte =
              recv(sSocket, packet + sizeof(DWORD) + sizeof(BYTE) + nRealRecivByte, nShouldReciveByte - nTotalRecivByte, 0);
          if (nRealRecivByte == SOCKET_ERROR)
          {               
              return NULL;
          }
          nTotalRecivByte += nRealRecivByte;
      }

    }
    //�������
    return packet;
}

inline int SendPacket(SOCKET sSocket, BYTE emType, char* szSendData, int nSendDataBytes)
{
    //�������ݹ�����������
    int nTotaBytes = sizeof(DWORD)+sizeof(BYTE)+nSendDataBytes;
    char* packet =
        new char[nTotaBytes];
    if (packet == NULL)
    {
        return SOCKET_ERROR;
    }

    *(DWORD*)packet = nSendDataBytes + sizeof(BYTE);
    *(BYTE*)(packet + sizeof(DWORD)) = emType;
    if(szSendData != NULL)
    {
      memcpy(packet + sizeof(DWORD) + sizeof(BYTE), szSendData, nSendDataBytes);
    }

    //�������ݰ�
    int nTotoSentBytes = 0;
    int nSentBytes = 0;
    while (nTotoSentBytes < nTotaBytes)
    {
        nSentBytes = send(sSocket, (char*)packet + nSentBytes, nTotaBytes - nSentBytes, 0);
        if (nSentBytes == SOCKET_ERROR)
        {
            if (packet != NULL)
            {
                delete packet;
            }
            return SOCKET_ERROR;
        }
        nTotoSentBytes += nSentBytes;
    }

    if (packet != NULL)
    {
        delete packet;
    }
    
    return 0;
}
