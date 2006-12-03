/*
Module : MfcCDDB.CPP
Purpose: Defines the implementation for an MFC class to wrap access to CDDB
Created: PJN / 24-02-1999
History: PJN / 19-05-1999 1. Added support for MOTD (Message of the Day)
                          2. Added support for submit
                          3. Fixed a number of places where calling the TRACE
                          function was causing an access violation
                          4. Fixed a bug in GetErrorMessage which was causing
                          SDK errors to return the string "The operation completed successfully"
         PJN / 14-07-1999 1. Fixed two potential access violations in the CCDDB::ReadResponse function.
         PJN / 29-11-1999 Fixed a number of bugs when compiled with VC 6.0
         PJN / 17-01-2000 Fixed a nmber of compiler errors which seem to have crept into v1.12
         PJN / 23-01-2000 1. The code has now been fully tested with the open source alternative to CDDB namely 
                          freedb.freedb.org. CDDB has become more commercialized of late, since its introduction 
                          of a binary SDK with closed validation and new licensening rules. I also believe that 
                          CDDB will stop access sometime in 2000 to its databases via non CDDB originated code.
                          Therefore I would suggest that developers using MfcCDDB should provide users with a 
                          configuation dialog to allow the CDDB server to be specified, that way you will not 
                          have irate customers ringing you when CDDB stops allowing access to it. You can use 
                          this open source server instead of the CDDB one in the MfcCDDB code by using 
                          "freedb.freedb.org" instead of "cddb.cddb.com" in the call to CCDDB::Sites and by 
                          specifying CDDB protocol version 3 in the function SetCDDBProtocolVersion.
         PJN / 13-02-2000 1. Now fully supports access when behind a HTTP proxy.
                          2. Also now includes support for proxy authentication
         PJN / 07-05-2000 Fixed a problem in GetUserName and GetHostName with embedded spaces
         PJN / 17-05-2000 Now handles CDDB records which have more than one DTITLE line
         PJN / 30-05-2000 Now prevents creation of empty extended track tags (EXTT=)
         PJN / 05-09-2000 Modified CCDDB::GetTrackPositions to fail when the CD does not contain any audio tracks
         PJN / 09-08-2001 Fixed a bug where the first track title was not being parsed correctly. Thanks to Jared 
                          Allen for spotting this.
         PJN / 06-10-2001 Major update to bring code into line with freedb standard. As mentioned in previous 
                          updates you can no longer use MFCCDDB with the commerical CDDB web site at Gracenote
                          (formerly CDDB). Instead now the code can only be used with the open source alternative
                          of freedb. Updates include:
                          1. Updated copyright information
                          2. Fixed calculation the extra disc length parameter, which must be specified in the query string.
                          3. Fixed bug where discid for submissions was having any leading 0's stripped.
                          4. Updated m_sProductVersion as it was out of date.
                          5. Changed default server in Sites method to "freedb.freedb.org"
                          6. Removed comment line which says "Copyright ... CDDB Inc" from Submit method
                          7. Updated links in documentation to be www.freedb.org
                          8. Fixed a bug in ReadResponse when an error occurs reading from the socket
                          9. Reworked the ReadResponse method to be more inline with my W3MFC socket class
                          10. Updated the sample app to display the last command response when a function fails
                          11. framework now works with version 5 of the CDDB protocol. This gives two extra
                          fields in the CDDB record class namely Year and Genre.
                          12. Complete rewrite of the ParseRecordBody to improve readibilty of the function. This 
                          means that the code now correctly supports all text record values which span multiple 
                          lines
                          13. Made m_ExtendedData member of CCDDBRecord a CString instead of a string array
                          14. Fixed a bug in CCDDB::ParseKeywordLine
                          15. Fixed bug when separating artist and title from DTITLE lines
                          16. Fixed an infinite recursion bug in ReplaceSubString
         PJN / 02-02-2002 1. Fixed a minor parsing bug in CCDDB::GetNextLine. Thanks to Thomas Lang for this fix.
         PJN / 24-06-2002 1. Now fully supports local access to database. The standard and alternate form are
                          both fully supported.
                          2. Optimized #include of 2 files in mfccddb.cpp
         PJN / 03-08-2003 1. Fixed a bug when the MCI device was not being closed if the inserted CD did not contain
                          an Audio CD. Thanks to Christian Inkster for reporting this problem.
         PJN / 03-11-2003 1. Simplified the code in CCDDB::ReadResponse. Thanks to Clarke Brunt for reporting this issue.
         PJN / 20-08-2004 1. General tidy up of the GetUserName and GetHostName functions.


                            
Copyright (c) 1999 - 2003 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/



/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include <iostream>

#ifndef __AFXPRIV_H__
#pragma message("To avoid this message, please put afxpriv.h in your PCH")
#endif
#include <afxpriv.h>
#ifndef _INC_MMSYSTEM
#pragma message("To avoid this message, please put mmsystem.h in your PCH")
#include <mmsystem.h>
#endif
#include "MfcCDDB.h"

#pragma warning( disable : 4267 )
#pragma warning( disable : 4311 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4311 )

//////////////////////////////// Statics / Macros /////////////////////////////

const DWORD READ_RESPONSE_QUERY = 1;
const DWORD READ_RESPONSE_SUBMIT = 2;


char CCDDB::m_base64tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz0123456789+/";
#define EOL "\r\n"
const int BASE64_MAXLINE = 76;


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif




////////////////////////////////// Implementation /////////////////////////////

CCDDBSite::CCDDBSite(BOOL bRetrieve)
{
  m_nPort = 80;
  m_bNorthing = TRUE;
  m_nLatitudeMinutes = 0;
  m_bEasting = TRUE;
  m_nLongitudeMinutes = 0;
  if (bRetrieve)
    m_sAddress = _T("/~cddb/cddb.cgi");
  else
    m_sAddress = _T("/~cddb/submit.cgi");
}

CCDDBSite::CCDDBSite(const CCDDBSite& site)
{
  *this = site;
}

CCDDBSite& CCDDBSite::operator=(const CCDDBSite& site)
{
  m_sSite = site.m_sSite;
  m_nPort = site.m_nPort;
  m_sAddress = site.m_sAddress;
  m_bNorthing = site.m_bNorthing;
  m_nLatitudeMinutes = site.m_nLatitudeMinutes;
  m_bEasting = site.m_bEasting;
  m_nLongitudeMinutes = site.m_nLongitudeMinutes;
  m_sDescription = site.m_sDescription;
  return *this;
}

BOOL CCDDBSite::Parse(LPSTR pszLine)
{
  LPSTR pszField = pszLine;

  //Get the site name
  LPSTR pszEnd = pszField;
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';
  m_sSite = pszField;
  
  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Get the protocol
  pszEnd = pszField;
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';

  CString sProtocol = CString( pszField );
    
  //If the protocol is not http, then ignore it
  if (sProtocol.CompareNoCase(_T("http")) != 0)
    return FALSE;

  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Get the port number
  pszEnd = pszField;
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';
  m_nPort = ::atoi(pszField);

  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Get the address
  pszEnd = pszField;
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';
  m_sAddress = pszField;
  
  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Get the latitude  
  pszEnd = pszField;
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';
  
  //Latitude field must be 7 characters long
  if (strlen(pszField) != 7)
    return FALSE;

  //Parse out the northing field
  if (pszField[0] == 'N')
    m_bNorthing = TRUE;
  else if (pszField[0] == 'S')
    m_bNorthing = FALSE;
  else
    return FALSE;

  //Parse out the Latitude degrees
  char sNum[4];
  sNum[0] = pszField[1];
  sNum[1] = pszField[2];
  sNum[2] = pszField[3];
  sNum[3] = '\0';
  int nLatDeg = ::atoi(sNum);

  //Parse out the Latitude minutes
  sNum[0] = pszField[5];
  sNum[1] = pszField[6];
  sNum[2] = '\0';
  int nLatMin = ::atoi(sNum);
  m_nLatitudeMinutes = nLatMin + 60*nLatDeg;
  
  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Get the longitude  
  pszEnd = pszField;
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';
  
  //Longitude field must be 7 characters long
  if (strlen(pszField) != 7)
    return FALSE;

  //Parse out the easting field
  if (pszField[0] == 'E')
    m_bEasting = TRUE;
  else if (pszField[0] == 'W')
    m_bEasting = FALSE;
  else
    return FALSE;

  //Parse out the Longitude degrees
  sNum[0] = pszField[1];
  sNum[1] = pszField[2];
  sNum[2] = pszField[3];
  sNum[3] = '\0';
  int nLongDeg = ::atoi(sNum);

  //Parse out the Longitude minutes
  sNum[0] = pszField[5];
  sNum[1] = pszField[6];
  sNum[2] = '\0';
  int nLongMin = ::atoi(sNum);
  m_nLongitudeMinutes = nLongMin + 60*nLongDeg;

  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Whats left is put into the description field
  m_sDescription = pszField;

  //Everything has been parsed correctly
  return TRUE;
}





CCDDBQueryResult::CCDDBQueryResult()
{
  m_dwDiscID = 0;
}

CCDDBQueryResult::CCDDBQueryResult(const CCDDBQueryResult& result)
{
  *this = result;
}

CCDDBQueryResult& CCDDBQueryResult::operator=(const CCDDBQueryResult& result)
{
  m_sCategory = result.m_sCategory;
  m_dwDiscID  = result.m_dwDiscID;
  m_sArtist   = result.m_sArtist;
  m_sTitle    = result.m_sTitle;

  return *this;
}

BOOL CCDDBQueryResult::Parse(LPSTR pszLine)
{
  LPSTR pszField = pszLine;

  //Get the category
  LPSTR pszEnd = pszField;
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';
  m_sCategory = pszField;

  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Get over the CDDB DISCID
  while (pszEnd[0] != ' ' && pszEnd[0] != '\0')
    ++pszEnd;
  if (pszEnd[0] == '\0')
    return FALSE;
  pszEnd[0] = '\0';
  sscanf(pszField, "%x", &m_dwDiscID);

  //skip over the whitespace
  pszField = ++pszEnd;
  while (pszField[0] == ' ' && pszField[0] != '\0')
    ++pszField;
  if (pszField[0] == '\0')
    return FALSE;

  //Get the Artist and Title  
  m_sTitle = pszField;

  //Remove any EOL if it is on the end
  int nEOLPos = m_sTitle.Find(_T("\r\n"));
  if (nEOLPos == -1)
    nEOLPos = m_sTitle.Find(_T("\n"));
  if (nEOLPos != -1)
    m_sTitle = m_sTitle.Left(nEOLPos);
  
  //Split into the artist and title
  int nSlashPos = m_sTitle.Find(_T('/'));
  if (nSlashPos != -1)
  {
    m_sArtist = m_sTitle.Left(nSlashPos);
    m_sTitle = m_sTitle.Right(m_sTitle.GetLength() - nSlashPos - 1);

    //Remove any trailing or leading spaces
    m_sArtist.TrimLeft();
    m_sArtist.TrimRight();
    m_sTitle.TrimLeft();
    m_sTitle.TrimRight();
  }

  return TRUE;
}





CCDDBTrackPosition::CCDDBTrackPosition()
{
  m_nMinute = 0;
  m_nSecond = 0;
  m_nFrame  = 0;
}

CCDDBTrackPosition::CCDDBTrackPosition(const CCDDBTrackPosition& position)
{
  *this = position;
}

CCDDBTrackPosition& CCDDBTrackPosition::operator=(const CCDDBTrackPosition& position)
{
  m_nMinute = position.m_nMinute;
  m_nSecond = position.m_nSecond;
  m_nFrame  = position.m_nFrame;

  return *this;
}





CHTTPSocket::CHTTPSocket()
{
  m_hSocket = INVALID_SOCKET; //default to an invalid scoket descriptor
}

CHTTPSocket::~CHTTPSocket()
{
  Close();
}

BOOL CHTTPSocket::Create()
{
  m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
  return (m_hSocket != INVALID_SOCKET);
}

BOOL CHTTPSocket::Connect(LPCTSTR pszHostAddress, int nPort)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);
  
	LPSTR lpszAscii = T2A((LPTSTR) pszHostAddress);

	//Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)nPort);
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);

	//If the address is not dotted notation, then do a DNS 
	//lookup of it.
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszAscii);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
      WSASetLastError(WSAEINVAL); 
			return FALSE;
		}
	}

	//Call the protected version which takes an address 
	//in the form of a standard C style struct.
	return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

BOOL CHTTPSocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	return (connect(m_hSocket, lpSockAddr, nSockAddrLen) != SOCKET_ERROR);
}

BOOL CHTTPSocket::Send(LPCSTR pszBuf, int nBuf)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  return (send(m_hSocket, pszBuf, nBuf, 0) != SOCKET_ERROR);
}

int CHTTPSocket::Receive(LPSTR pszBuf, int nBuf)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  return recv(m_hSocket, pszBuf, nBuf, 0); 
}

void CHTTPSocket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		VERIFY(SOCKET_ERROR != closesocket(m_hSocket));
		m_hSocket = INVALID_SOCKET;
	}
}

BOOL CHTTPSocket::IsReadible(BOOL& bReadible, DWORD dwTimeout)
{
  timeval timeout;
  timeout.tv_sec = dwTimeout/1000;
  timeout.tv_usec = (dwTimeout%1000)*1000;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(m_hSocket, &fds);
  int nStatus = select(0, &fds, NULL, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
  {
    return FALSE;
  }
  else
  {
    bReadible = !(nStatus == 0);
    return TRUE;
  }
}



CCDDBRecord::CCDDBRecord()
{
  m_nDiskLength = 0;
  m_nDatabaseRevision = 0;
  m_dwDiscID = 0;
  m_nYear = 0;
}

CCDDBRecord::CCDDBRecord(const CCDDBRecord& record)
{
  *this = record;
}

CCDDBRecord& CCDDBRecord::operator=(const CCDDBRecord& result)
{
  m_TrackOffsets.Copy(result.m_TrackOffsets);
  m_nDiskLength = result.m_nDiskLength;
  m_nDatabaseRevision = result.m_nDatabaseRevision;
  m_sClientName = result.m_sClientName;
  m_sClientVersion = result.m_sClientVersion;
  m_sClientComments = result.m_sClientComments;
  m_dwDiscID = result.m_dwDiscID;
  m_sArtist = result.m_sArtist;
  m_sTitle = result.m_sTitle;
  m_TrackTitles.Copy(result.m_TrackTitles);
  m_ExtendedData = result.m_ExtendedData;
  m_ExtendedTrackData.Copy(result.m_ExtendedTrackData);
  m_PlayOrder.Copy(result.m_PlayOrder);
  m_nYear = result.m_nYear;
  m_sGenre = result.m_sGenre;

  return *this;
}




CCDDB::CCDDB() : m_sProductName(_T("MfcCDDB")), m_sProductVersion(_T("1.27"))
{
  m_dwLastError = 0;
#ifdef _DEBUG
  m_dwTimeout = 60000; //timeout is set to 60 seconds for debug
#else
  m_dwTimeout = 2000;  //2 seconds for release builds
#endif
  m_nProtocolVersion = 5;
  m_nProxyPort = 80;
}

CCDDB::~CCDDB()
{
}

DWORD CCDDB::GetLastError() const 
{ 
  if (m_dwLastError)
    return m_dwLastError; 
  else
    return ::GetLastError();
}

void CCDDB::SetCDDBProtocolVersion(int nProtocolVersion) 
{
  ASSERT(nProtocolVersion >= 3 && nProtocolVersion <= 5); //MfcCDDB source code only supports v3, v4 & v5.
  m_nProtocolVersion = nProtocolVersion; 
}

CString CCDDB::GetErrorMessage() const
{
  CString sError;
  if (m_dwLastError)
  {
    TCHAR sMessage[129];
    mciGetErrorString(m_dwLastError, sMessage, 128);
    sError = sMessage;
  }
  else
  {
    //Use the SDK function ::FormatMessage to create a string for us
	  LPTSTR lpBuffer = NULL;
    DWORD dwLastError = ::GetLastError();
    BOOL bSuccess = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE, 
			                              NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT), (LPTSTR) &lpBuffer, 0, NULL);
	  if (bSuccess)
	  {
		  sError = lpBuffer;

      //Don't forget to free the memory ::FormatMessage allocated for us
		  LocalFree(lpBuffer);
	  }
    else
    {
      //Restore the error if FormatMessage failed
      SetLastError(dwLastError);
    }
  }

  return sError;
}

DWORD CCDDB::ComputeDiscID(const CArray<CCDDBTrackPosition, CCDDBTrackPosition&>& tracks)
{
  int nTracks = tracks.GetSize() - 1; //Number of tracks is 1 less than the size
                                      //of the array as it also contains the lead 
                                      //out position
  //Validate our parameters
  ASSERT(nTracks > 0);

  //Iterate across all the tracks
  int n=0;
  for (int i=0; i<nTracks; i++)
  {
    int sum = 0;
    int j = tracks[i].m_nMinute*60 + tracks[i].m_nSecond;
    while (j > 0)
    {
      sum += j%10;
      j /=10;
    }
    n += sum;
  }

  //Compute total track length in seconds
  int t = tracks[nTracks].m_nMinute*60 + tracks[nTracks].m_nSecond - tracks[0].m_nMinute - tracks[0].m_nSecond;

  //Compute DISC ID
  DWORD dwDiscID = ((n % 0xFF) << 24 | t << 8 | nTracks);
  return dwDiscID;
}

BOOL CCDDB::GetTrackPositions(CArray<CCDDBTrackPosition, CCDDBTrackPosition&>& tracks, LPCTSTR pszDrive)
{
  //Remove any tracks already in the array
  tracks.RemoveAll();

  //Open the specified "cdaudio" MCI device
  MCI_OPEN_PARMS mciOpenParms;
  mciOpenParms.lpstrDeviceType = _T("cdaudio");
  mciOpenParms.lpstrElementName = pszDrive;
  m_dwLastError = ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_SHAREABLE | MCI_OPEN_TYPE | (pszDrive ? MCI_OPEN_ELEMENT : 0), (DWORD) &mciOpenParms);
  if (m_dwLastError)
  {
    TRACE(_T("Failed to open the cdaudio MCI device, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Set the time format to Minute/Second/Frame (MSF) format
  MCI_SET_PARMS mciSetParms;
  mciSetParms.dwTimeFormat = MCI_FORMAT_MSF;
  m_dwLastError = ::mciSendCommand(mciOpenParms.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD) &mciSetParms);
  if (m_dwLastError)
  {
    //Dont forget to close the MCI device
    ::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);

    TRACE(_T("Failed to set cdaudio MCI device to MSF format, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Get the total track count
  MCI_STATUS_PARMS mciStatusParms;
  mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
  m_dwLastError = ::mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD) &mciStatusParms);
  if (m_dwLastError)
  {
    //Dont forget to close the MCI device
    ::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);

    TRACE(_T("Failed to get number of cdaudio tracks, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }
  int nTotalTracks = (int) mciStatusParms.dwReturn;

  //check that disk contain audio track
  BOOL bAudioTrackPresent = FALSE;	
  for (int i=1; i<=nTotalTracks && !bAudioTrackPresent; i++)
  {                      
		mciStatusParms.dwItem = MCI_CDA_STATUS_TYPE_TRACK;
		mciStatusParms.dwTrack = i;
		m_dwLastError = mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD) &mciStatusParms);
		if (m_dwLastError)
		{
			//Dont forget to close the MCI device
			::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);
			
			TRACE(_T("Failed to get track %d's type, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
			return FALSE;
		}
		if (mciStatusParms.dwReturn == MCI_CDA_TRACK_AUDIO)
			bAudioTrackPresent = TRUE;
	}
  if (!bAudioTrackPresent)  
  {
    //Dont forget to close the MCI device
    ::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);

    TRACE(_T("The CD does not contain any audio tracks!\n"));
    return FALSE;
  } 

  //Iterate through all the tracks getting their starting position
  tracks.SetSize(nTotalTracks + 1);
  for (i=1; i<=nTotalTracks; i++)
  {                      
    mciStatusParms.dwItem = MCI_STATUS_POSITION;
    mciStatusParms.dwTrack = i;
    m_dwLastError = mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD) &mciStatusParms);
    if (m_dwLastError)
    {
      //Dont forget to close the MCI device
      ::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);

      //Remove all the fields if we have an error getting any of the tracks
      tracks.RemoveAll();

      TRACE(_T("Failed to get track %d's starting position, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }

    //Save the track position in MSF format
    CCDDBTrackPosition trackPosition;
    trackPosition.m_nMinute = MCI_MSF_MINUTE(mciStatusParms.dwReturn);
    trackPosition.m_nSecond = MCI_MSF_SECOND(mciStatusParms.dwReturn);
    trackPosition.m_nFrame  = MCI_MSF_FRAME(mciStatusParms.dwReturn);

    //Store the value in the array
    tracks.SetAt(i-1, trackPosition);
  }

  //Get the last track's length
  mciStatusParms.dwItem = MCI_STATUS_LENGTH;
  mciStatusParms.dwTrack = nTotalTracks;
  m_dwLastError = mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD) &mciStatusParms);
  if (m_dwLastError)
  {
    //Dont forget to close the MCI device
    ::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);

    //Remove all the fields if we have an error getting any of the tracks
    tracks.RemoveAll();

    TRACE(_T("Failed to get track %d's length, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }


  //Compute lead-out track position
  DWORD dwLenM = MCI_MSF_MINUTE(mciStatusParms.dwReturn);
  DWORD dwLenS = MCI_MSF_SECOND(mciStatusParms.dwReturn);
  DWORD dwLenF = MCI_MSF_FRAME(mciStatusParms.dwReturn) + 1; //Fix MCI Windows bug according to CDDB Howto doc
  DWORD dwPosM = tracks[nTotalTracks-1].m_nMinute;
  DWORD dwPosS = tracks[nTotalTracks-1].m_nSecond;
  DWORD dwPosF = tracks[nTotalTracks-1].m_nFrame;

  //Compute lead out track position (in frame format)
  DWORD dwPos = (dwPosM*60*75) + (dwPosS*75) + dwPosF + (dwLenM*60*75) + (dwLenS*75) + dwLenF;

  //Convert dwPos back to MSF format
  CCDDBTrackPosition trackPosition;
  trackPosition.m_nFrame = dwPos % 75;
  dwPos /= 75;
  trackPosition.m_nSecond = dwPos % 60;
  dwPos /= 60;
  trackPosition.m_nMinute = dwPos;

  //And store in the array
  tracks.SetAt(nTotalTracks, trackPosition);

  //Dont forget to close the MCI device
  ::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);

  return TRUE;
}

BOOL CCDDB::ComputeDiscID(DWORD& dwDiscID, LPCTSTR pszDrive)
{
  //Get the track details
  CArray<CCDDBTrackPosition, CCDDBTrackPosition&> tracks;
  if (!GetTrackPositions(tracks, pszDrive))
    return FALSE;
  
  //Compute the DISC ID now that we have got all the track information
  dwDiscID = ComputeDiscID(tracks);
  
  return TRUE;    
}
    
void CCDDB::GetCDROMDrives(CStringArray& drives)
{
  //empty out the array
  drives.RemoveAll();

  //Iterate across all the drive letters to find out which ones are CDROMs
  for (int i=1; i<=26; i++)
  {
    CString sDrive;
    sDrive.Format(_T("%c:"), i-1+'A');
    if (GetDriveType(sDrive) == DRIVE_CDROM)
      drives.Add(sDrive);
  }
}

BOOL CCDDB::ReadResponse(CHTTPSocket& socket, LPSTR pszTerminator, LPSTR& pszRecvBuffer, int nGrowBy, DWORD dwHint)
{
  //Allocate the buffer which will receive the data
  pszRecvBuffer = new char[nGrowBy];
  int nBufSize = nGrowBy;

  //Retrieve the reponse using until we
	//get the terminator or a timeout occurs
	BOOL bFoundTerminator = FALSE;
  BOOL bContinue = TRUE;
	int nReceived = 0;
	while (!bFoundTerminator && bContinue)
	{
    //check the socket for readability
    BOOL bReadible;
    if (!socket.IsReadible(bReadible, m_dwTimeout))
    {
      if (pszRecvBuffer && nReceived)
      {
	      pszRecvBuffer[nReceived] = '\0';
        m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
      }
      m_dwLastError = 0;
			return FALSE;
    }
    else if (!bReadible) //no data to receive, fail the function
    {
      if (pszRecvBuffer && nReceived)
      {
		    pszRecvBuffer[nReceived] = '\0';
        m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
      }
      SetLastError(WSAETIMEDOUT);
      m_dwLastError = 0;
			return FALSE;
    }

		//receive the data from the socket
    int nBufRemaining = nBufSize-nReceived-1; //Allows allow one space for the NULL terminator
    if (nBufRemaining<0)
      nBufRemaining = 0;
	  int nData = socket.Receive(pszRecvBuffer+nReceived, nBufRemaining);

    //Increment the count of data received
    if (nData > 0)
		  nReceived += nData;							   

    //If an error occurred receiving the data
		if (nData < 1)
		{
      //NULL terminate the data received
	    pszRecvBuffer[nReceived] = '\0';

      m_dwLastError = 0;
      m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
		  return FALSE; 
		}
		else
		{
      //NULL terminate the data received
      if (pszRecvBuffer)
		    pszRecvBuffer[nReceived] = '\0';

      if (nBufRemaining-nData == 0) //No space left in the current buffer
      {
        //Allocate the new receive buffer
        nBufSize += nGrowBy; //Grow the buffer by the specified amount
        char* pNewBuf = new char[nBufSize];   
        pNewBuf[0] = '\0'; //Initially NULL terminate the data

        //copy the old contents over to the new buffer and assign 
        //the new buffer to the local variable used for Retrieveing 
        //from the socket
        strcpy(pNewBuf, pszRecvBuffer);
        delete [] pszRecvBuffer;
        pszRecvBuffer = pNewBuf;
      }
		}

    //Special case code for reading a Query response
    if (dwHint == READ_RESPONSE_QUERY)
    {
      if (pszRecvBuffer && strlen(pszRecvBuffer))
      {
        //Extract the HTTP body from the response;
        LPSTR pszBody = FindHTTPBody(pszRecvBuffer);
	
		//From the HTTP body get the CDDB response code

		if ( !pszBody )
			bContinue = FALSE;		
		else
		{
			int nResponseCode = GetCDDBReponseCode(pszBody);

			//Only continue to receive data if the response code indicates more data is to be received
			if ((nResponseCode < 210 || nResponseCode > 219))
				bContinue = FALSE;      
		}
      }
    }
    else if (dwHint == READ_RESPONSE_SUBMIT)    //Special case code for reading a submit response
    {
      if (pszRecvBuffer && strlen(pszRecvBuffer))
      {
        //Extract the HTTP body from the response;
        LPSTR pszBody = FindHTTPBody(pszRecvBuffer);

        //From the HTTP body get the CDDB response code
        int nResponseCode = GetCDDBReponseCode(pszBody);

        //Only continue to receive data if the response code indicates more data is to be received
        if ((nResponseCode < 210 || nResponseCode > 219))
          bContinue = FALSE;          
      }
    }

    //Check to see if the terminator character(s) have been found
		bFoundTerminator = (strstr(pszRecvBuffer, pszTerminator) != NULL);
	}

	//Remove the terminator from the response data
  if (bFoundTerminator)
    pszRecvBuffer[nReceived - strlen(pszTerminator)] = '\0';

  return TRUE;
}

CString CCDDB::GetUserName()
{
  //Get the user name 
  CString sUser;
  TCHAR pszUser[_MAX_PATH];
  DWORD dwSize = _MAX_PATH;
  if (::GetUserName(pszUser, &dwSize))
    sUser = pszUser;
  else
    return _T("Unknown");
   
  //convert string to alpha chars and digits only
  int nLength = sUser.GetLength();
  for (int i=0; i<nLength; i++) 
  {
    TCHAR c = sUser.GetAt(i);
    if (!((_istalpha(c)) || (_istdigit(c))))
      sUser.SetAt(i, _T('_'));
  }

  return sUser;
}

CString CCDDB::GetHostName()
{
  CString sHost;
  char pszHost[_MAX_PATH];
  if (gethostname(pszHost, _MAX_PATH) == 0)
    sHost = pszHost;
  else
    return _T("Unknown");

  //convert string to alpha chars and digits only
  int nLength = sHost.GetLength();
  for (int i=0; i<nLength; i++) 
  {
    TCHAR c = sHost.GetAt(i);
    if (!((_istalpha(c)) || (_istdigit(c))))
      sHost.SetAt(i, _T('_'));
  }

  return sHost;
}

CString CCDDB::GetHelloCommand()
{
  CString sCommand;
  sCommand.Format(_T("hello=%s+%s+%s+%s&proto=%d"), GetUserName(), GetHostName(), m_sProductName, m_sProductVersion, m_nProtocolVersion);
  return sCommand;
}

LPSTR CCDDB::FindHTTPBody(LPCSTR pszResponse)
{
  //Validate our parameters
  ASSERT(pszResponse);
  ASSERT(strlen(pszResponse));

  //Find the HTTP body
  LPSTR pszData = strstr(pszResponse, "\r\n\r\n");
  
  //If found, skip over the 2 lines
  if (pszData)
    pszData += 4;

  return pszData;  
}

LPSTR CCDDB::SkipToNextLine(LPSTR pszLine)
{
  //Validate our parameters
  ASSERT(pszLine);

  //Find the next line. Both Dos (\r\n) 
  //and Unix (\n) terminators are valid. First try to
  //find a DOS EOL
  LPSTR lpszData = strstr(pszLine, "\r\n");
  if (lpszData)
    lpszData += 2;
  else
  {
    lpszData = strstr(pszLine, "\n");
    if (lpszData)
      lpszData++;
  }

  return lpszData;  
}

LPSTR CCDDB::GetNextLine(LPSTR pszLine)
{
  //validate our parameters
  ASSERT(pszLine);

  LPSTR pszNextLine = NULL;
  if (strlen(pszLine)>0)
  {
    //Find the start of the next line. Both Dos (\r\n) 
    //and Unix (\n) terminators are valid. First try to
    //find a DOS EOL
    pszNextLine = strstr(pszLine, "\r\n");
    if (pszNextLine)
    {
      LPSTR pszEnd = pszNextLine;  
      pszNextLine = pszNextLine + 2;
      pszEnd[0] = '\0';
    }
    else
    {
      //Failed to get a DOS EOL, try to get a Unix EOL
      pszNextLine = strstr(pszLine, "\n");
      if (pszNextLine)
      {
        LPSTR pszEnd = pszNextLine;  
        pszNextLine = pszNextLine + 1;
        pszEnd[0] = '\0';
      }
    }
  }

  return pszNextLine;
}

int CCDDB::GetCDDBReponseCode(LPSTR pszBody)
{
  //Validate out parameters
  ASSERT(pszBody);
  ASSERT(strlen(pszBody));

  int nResponse = -1;
  int nLength = strlen(pszBody);
  if (nLength >= 3)
  {
    char sNum[4];
    strncpy(sNum, pszBody, 3);
    sNum[3] = '\0';
    nResponse = atoi(sNum);
  }
  return nResponse;
}

BOOL CCDDB::ParseCommentLine(LPCSTR pszLine, CString& sValue)
{
  //Validate out parameters
  ASSERT(pszLine);

  //Duplicate the string into a local variable 
  int nLength = strlen(pszLine);
  LPSTR pszData = new char[nLength + 1];
  strcpy(pszData, pszLine);
  
  //Do the actual parsing
  BOOL bSuccess = FALSE;
  if (pszData[0] == '#')
  {
    sValue.Empty();
    bSuccess = TRUE;

    char* pszValue = pszData+1;
    if (strlen(pszValue) > 1)
    {
      sValue = pszValue;

      //Remove unwanted spaces 
      sValue.TrimLeft();
      sValue.TrimRight();
    }
  }

  //Dont forget to delete our local string
  delete [] pszData;

  return bSuccess;
}

BOOL CCDDB::SplitSemiColonLine(LPCSTR pszLine, CString& sField, CString& sValue)
{
  //Validate out parameters
  ASSERT(pszLine);
  
  //Duplicate the string into a local variable 
  int nLength = strlen(pszLine);
  LPSTR pszData = new char[nLength + 1];
  strcpy(pszData, pszLine);

  //Do the actual parsing
  BOOL bSuccess = FALSE;
  LPSTR pszColon = strstr(pszData, ":");
  if (pszColon)
  {
    pszColon[0] = '\0';
    if (strlen(pszData) && strlen(pszColon+1) > 1)
    {
      sField = pszData;
      sValue = pszColon+1;

      //Remove unwanted spaces 
      sField.TrimLeft();
      sField.TrimRight();
      sValue.TrimLeft();
      sValue.TrimRight();
      bSuccess = TRUE;
    }
  }

  //Dont forget to delete our local string
  delete [] pszData;

  return bSuccess;
}

BOOL CCDDB::Sites(CArray<CCDDBSite, CCDDBSite&>& sites, const CCDDBSite& server)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Create the socket
  CHTTPSocket socket;
  if (!socket.Create())
  {
    m_dwLastError = 0;
    TRACE(_T("Failed to create client socket for sites command, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Connect to the host on the specified port
  BOOL bUseProxy = !m_sProxyHost.IsEmpty();
  if (!socket.Connect(bUseProxy ? m_sProxyHost : server.m_sSite, bUseProxy ? m_nProxyPort : server.m_nPort))
  {
    m_dwLastError = 0;
    TRACE(_T("Could not connect to the CDDB HTTP server %s, GetLastError:%d, %s\n"), server.m_sSite, GetLastError(), GetErrorMessage());
    return FALSE;
  }
  else
  {
    //Form the specialized HTTP request to retrieve the sites data
    CString sRequest;
    if (!bUseProxy)
      sRequest.Format(_T("GET %s?cmd=sites&%s HTTP/1.0\r\n\r\n"), server.m_sAddress, GetHelloCommand());
    else
      sRequest.Format(_T("GET http://%s?cmd=sites&%s HTTP/1.0\r\n%s\r\n\r\n"), server.m_sSite+server.m_sAddress, GetHelloCommand(), GetProxyHeader(server.m_sSite));

    //Send the request through the socket
    LPCSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    int nCmdLength = strlen(pszRequest);
    if (!socket.Send(pszRequest, nCmdLength))
    {
      m_dwLastError = 0;
      TRACE(_T("An unexpected error occurred while sending the HTTP sites request, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }
    
    //Read the response back through the socket
    char* pszBuffer;
    if (ReadResponse(socket, "\r\n.\r\n", pszBuffer))
    {
      ASSERT(pszBuffer);

      //Extract the HTTP body from the response;
      LPSTR pszBody = FindHTTPBody(pszBuffer);

      //Parse the sites info from the response
      bSuccess = ParseSitesBody(pszBody, sites);
    }
    else
    {
      //Reponse could not be retrieved
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
    	TRACE(_T("CDDB server did not respond correctly to the sites command\n"));
    }

    //Don't forget to delete the allocated buffer
    delete [] pszBuffer;

    return bSuccess;
  }
}

BOOL CCDDB::ParseSitesBody(LPSTR pszBody, CArray<CCDDBSite, CCDDBSite&>& sites)
{
  //setup return value
  BOOL bSuccess = FALSE;

  //Hive away the last reponse until everything is ok
  m_sLastCommandResponse = pszBody;

  //From the HTTP body get the CDDB response code
  int nResponseCode = GetCDDBReponseCode(pszBody);

  //If the response is in the expected range (21x) then parse the site data line by line
  if (nResponseCode >= 210 && nResponseCode <= 219)
  {
    //Remove any entries which are in the sites array
    sites.RemoveAll();

    //Get the site data and then iterate through all the lines
    LPSTR pszLine = SkipToNextLine(pszBody);
    while (pszLine)
    {
      //Get the next line before we parse the current line
      LPSTR pszNextLine = GetNextLine(pszLine);

      //If we can parse the current line then add it 
      //to the sites array
      CCDDBSite site;
      if (site.Parse(pszLine))
        sites.Add(site);
    
      //Move on to the next line  
      pszLine = pszNextLine;
    }

    //Everything is successful, empty out the last command reponse string
    m_sLastCommandResponse.Empty();
    bSuccess = TRUE;
  }
  else
  {
    //An unexpected response was retrieved
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a valid sites response code\n"));
  }

  return bSuccess;
}

BOOL CCDDB::Sites(CArray<CCDDBSite, CCDDBSite&>& sites, const CString& sServer, const CString& sAddress, int nPort)
{
  //Form the site which we will be querying
  CCDDBSite server;
  server.m_sSite = sServer;
  server.m_nPort = nPort;
  server.m_sAddress = sAddress;
  
  //Let the other version 
  return Sites(sites, server);
}

BOOL CCDDB::Categories(const CCDDBSite& server, CStringArray& categories)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Create the socket
  CHTTPSocket socket;
  if (!socket.Create())
  {
    m_dwLastError = 0;
    TRACE(_T("Failed to create client socket for categories command, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Connect to the host on the specified port
  BOOL bUseProxy = !m_sProxyHost.IsEmpty();
  if (!socket.Connect(bUseProxy ? m_sProxyHost : server.m_sSite, bUseProxy ? m_nProxyPort : server.m_nPort))
  {
    m_dwLastError = 0;
    TRACE(_T("Could not connect to the CDDB HTTP server %s, GetLastError:%d, %s\n"), server.m_sSite, GetLastError(), GetErrorMessage());
    return FALSE;
  }
  else
  {
    //Form the specialized HTTP request to retreive the sites data
    CString sRequest;
    if (!bUseProxy)
      sRequest.Format(_T("GET %s?cmd=cddb+lscat&%s HTTP/1.0\r\n\r\n"), server.m_sAddress, GetHelloCommand());
    else
      sRequest.Format(_T("GET http://%s?cmd=cddb+lscat&%s HTTP/1.0\r\n%s\r\n\r\n"), server.m_sSite+server.m_sAddress, GetHelloCommand(), GetProxyHeader(server.m_sSite));

    //Send the request through the socket
    LPCSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    int nCmdLength = strlen(pszRequest);
    if (!socket.Send(pszRequest, nCmdLength))
    {
      m_dwLastError = 0;
      TRACE(_T("An unexpected error occurred while sending the HTTP categories request, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }
    
    //Read the response back through the socket
    char* pszBuffer;
    if (ReadResponse(socket, "\r\n.\r\n", pszBuffer))
    {
      ASSERT(pszBuffer);

      //Extract the HTTP body from the response;
      LPSTR pszBody = FindHTTPBody(pszBuffer);

      //Parse the categories info from the response
      bSuccess = ParseCategoriesBody(pszBody, categories);
    }
    else
    {
      //Reponse could not be Retrieved
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
    	TRACE(_T("CDDB server did not respond correctly to the categories command\n"));
    }

    //Don't forget to delete the allocated buffer
    delete [] pszBuffer;

    return bSuccess;
  }
}

BOOL CCDDB::ParseCategoriesBody(LPSTR pszBody, CStringArray& categories)
{
  //setup return value
  BOOL bSuccess = FALSE;

  //Hive away the last reponse until everything is ok
  m_sLastCommandResponse = pszBody;

  //From the HTTP body get the CDDB response code
  int nResponseCode = GetCDDBReponseCode(pszBody);

  //If the response is in the expected range (21x) then parse the categories data line by line
  if (nResponseCode >= 210 && nResponseCode <= 219)
  {
    //Remove any entries which are in the sites array
    categories.RemoveAll();

    //Get the category data and then iterate through all the lines
    LPSTR pszLine = SkipToNextLine(pszBody);
    while (pszLine)
    {
      //Get the next line before we parse the current line
      LPSTR pszNextLine = GetNextLine(pszLine);

      //Add the current line to the categories array
      CString sCategory(pszLine);
      categories.Add(sCategory);
      
      //Move on to the next line  
      pszLine = pszNextLine;
    }

    //Everything is successful, empty out the last command reponse string
    m_sLastCommandResponse.Empty();
    bSuccess = TRUE;
  }
  else
  {
    //An unexpected response was Retrieved
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a valid response code to the categories command\n"));
  }

  return bSuccess;
}

BOOL CCDDB::Status(const CCDDBSite& server, CCDDBStatus& status)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Create the socket
  CHTTPSocket socket;
  if (!socket.Create())
  {
    m_dwLastError = 0;
    TRACE(_T("Failed to create client socket for status command, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Connect to the host on the specified port
  BOOL bUseProxy = !m_sProxyHost.IsEmpty();
  if (!socket.Connect(bUseProxy ? m_sProxyHost : server.m_sSite, bUseProxy ? m_nProxyPort : server.m_nPort))
  {
    m_dwLastError = 0;
    TRACE(_T("Could not connect to the CDDB HTTP server %s, GetLastError:%d, %s\n"), server.m_sSite, GetLastError(), GetErrorMessage());
    return FALSE;
  }
  else
  {
    //Form the specialized HTTP request to retreive the status data
    CString sRequest;
    if (!bUseProxy)
      sRequest.Format(_T("GET %s?cmd=stat&%s HTTP/1.0\r\n\r\n"), server.m_sAddress, GetHelloCommand());
    else
      sRequest.Format(_T("GET http://%s?cmd=stat&%s HTTP/1.0\r\n%s\r\n\r\n"), server.m_sSite+server.m_sAddress, GetHelloCommand(), GetProxyHeader(server.m_sSite));

    //Send the request through the socket
    LPCSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    int nCmdLength = strlen(pszRequest);
    if (!socket.Send(pszRequest, nCmdLength))
    {
      m_dwLastError = 0;
      TRACE(_T("An unexpected error occurred while sending the HTTP stat request, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }

    //Read the response back through the socket
    char* pszBuffer;
    if (ReadResponse(socket, "\r\n.\r\n", pszBuffer))
    {
      ASSERT(pszBuffer);

      //Extract the HTTP body from the response;
      LPSTR pszBody = FindHTTPBody(pszBuffer);

      //Parse out the status from the body
      bSuccess = ParseStatusBody(pszBody, status);
    }
    else
    {
      //Reponse could not be Retrieved
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
    	TRACE(_T("CDDB server did not respond correctly to the stat command\n"));
    }

    //Don't forget to delete the allocated buffer
    delete [] pszBuffer;

    return bSuccess;
  }
}

BOOL CCDDB::ParseStatusBody(LPSTR pszBody, CCDDBStatus& status)
{
  //setup return value
  BOOL bSuccess = FALSE;

  //Hive away the last reponse until everything is ok
  m_sLastCommandResponse = pszBody;

  //From the HTTP body get the CDDB response code
  int nResponseCode = GetCDDBReponseCode(pszBody);

  //If the response is in the expected range (21x) then parse the status info line by line
  if (nResponseCode >= 210 && nResponseCode <= 219)
  {
    //Empty out all the arrays in case they already have something in them
    status.m_Categories.RemoveAll();
    status.m_CategoryEntries.RemoveAll();
    status.m_PendingSites.RemoveAll();
    status.m_PendingEntries.RemoveAll();

    //Get ready for parsing all the fields
    LPSTR pszNextLine;

    //Skip over the first two status lines
    LPSTR pszCurLine = SkipToNextLine(pszBody);
    if (pszCurLine)
      pszCurLine = SkipToNextLine(pszCurLine);

    //iterate through all the data  
    BOOL bContinue = TRUE;
    while (pszCurLine && bContinue)
    {
      //Get the next line before we parse the current one
      pszNextLine = GetNextLine(pszCurLine);
      if (pszNextLine == NULL)
        bContinue = FALSE;

      //Parse each line
      CString sField;
      CString sValue;

      if (strcmp(pszCurLine, "Database entries by category:") == 0)
      {
        //Skip over the heading line  
        pszCurLine = pszNextLine;

        //Handle the database entries
        BOOL bMoreEntries = (pszCurLine != NULL);
        while (bMoreEntries)
        {
          //Get the next line before we parse the current one
          pszNextLine = GetNextLine(pszCurLine);
          if (pszNextLine == NULL)
            bMoreEntries = FALSE;

          //Parse the category info
          if (pszCurLine[0] == ' ')
          {
            if (SplitSemiColonLine(pszCurLine, sField, sValue))
            {
              status.m_Categories.Add(sField);
              int nValue = _ttoi(sValue);
              status.m_CategoryEntries.Add(nValue);
            }
          }
          else
            bMoreEntries = FALSE;

          pszCurLine = pszNextLine;
        }
      }
      else if (strcmp(pszCurLine, "Pending File transmissions:") == 0)
      {
        //Skip over the heading line  
        pszCurLine = pszNextLine;

        //Handle the database entries
        BOOL bMoreSites = (pszCurLine != NULL);
        while (bMoreSites)
        {
          //Get the next line before we parse the current one
          pszNextLine = GetNextLine(pszCurLine);
          if (pszNextLine == NULL)
            bMoreSites = FALSE;

          //Parse the pending sites info
          if (pszCurLine[0] == ' ')
          {
            if (SplitSemiColonLine(pszCurLine, sField, sValue))
            {
              status.m_PendingSites.Add(sField);
              int nValue = _ttoi(sValue);
              status.m_PendingEntries.Add(nValue);
            }
          }
          else
            bMoreSites = FALSE;

          pszCurLine = pszNextLine;
        }
      }

      //Parse out the other types of field
      if (pszCurLine)
      {
        if (SplitSemiColonLine(pszCurLine, sField, sValue))
        {
          if (sField == _T("current proto"))
            status.m_nCurrentProtocol = _ttoi(sValue);  
          else if (sField == _T("max proto"))
            status.m_nMaxProtocol = _ttoi(sValue);
          else if (sField == _T("gets"))
          {
            if (sValue == _T("yes"))
              status.m_bGetsAllowed = TRUE;
            else if (sValue == _T("no"))
              status.m_bGetsAllowed = FALSE;
            else
            {
              bContinue = FALSE;
              m_dwLastError = 0;
              SetLastError(WSAEPROTONOSUPPORT);
    		      TRACE(_T("CDDB server failed to return a valid gets response to the stat command\n"));
            }
          }
          else if (sField == _T("updates"))
          {
            if (sValue == _T("yes"))
              status.m_bUpdatesAllowed = TRUE;
            else if (sValue == _T("no"))
              status.m_bUpdatesAllowed = FALSE;
            else
            {
              bContinue = FALSE;
              m_dwLastError = 0;
              SetLastError(WSAEPROTONOSUPPORT);
    		      TRACE(_T("CDDB server failed to return a valid updates response to the stat command\n"));
            }
          }
          else if (sField == _T("posting"))
          {
            if (sValue == _T("yes"))
              status.m_bPostingAllowed = TRUE;
            else if (sValue == _T("no"))
              status.m_bPostingAllowed = FALSE;
            else
            {
              bContinue = FALSE;
              m_dwLastError = 0;
              SetLastError(WSAEPROTONOSUPPORT);
    		      TRACE(_T("CDDB server failed to return a valid posting response to the stat command\n"));
            }
          }
          else if (sField == _T("quotes"))
          {
            if (sValue == _T("yes"))
              status.m_bQuotes = TRUE;
            else if (sValue == _T("no"))
              status.m_bQuotes = FALSE;
            else
            {
              bContinue = FALSE;
              m_dwLastError = 0;
              SetLastError(WSAEPROTONOSUPPORT);
    		      TRACE(_T("CDDB server failed to return a valid quotes response to the stat command\n"));
            }
          }
          else if (sField == _T("current users"))
            status.m_nCurrentUsers = _ttoi(sValue);  
          else if (sField == _T("max users"))
            status.m_nMaxUsers = _ttoi(sValue);  
          else if (sField == _T("strip ext"))
          {
            if (sValue == _T("yes"))
              status.m_bStripExtended = TRUE;
            else if (sValue == _T("no"))
              status.m_bStripExtended = FALSE;
            else
            {
              bContinue = FALSE;
              m_dwLastError = 0;
              SetLastError(WSAEPROTONOSUPPORT);
    		      TRACE(_T("CDDB server failed to return a valid strip extended response to the stat command\n"));
            }
          }
          else if (sField == _T("Database entries"))
            status.m_nDatabaseEntries = _ttoi(sValue);  

          //Move to the next line 
          pszCurLine = pszNextLine;
        }
        else
          bContinue = FALSE;
      }
    }

    //Everything went ok ??
    bSuccess = TRUE;
    m_sLastCommandResponse.Empty();
  }

  return bSuccess;
}

BOOL CCDDB::Query(const CCDDBSite& server, DWORD dwDiscID, const CArray<CCDDBTrackPosition, CCDDBTrackPosition&>& tracks, 
                  CArray<CCDDBQueryResult, CCDDBQueryResult&>& results)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Create the socket
  CHTTPSocket socket;
  if (!socket.Create())
  {
    m_dwLastError = 0;
    TRACE(_T("Failed to create client socket for query command, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Connect to the host on the specified port
  BOOL bUseProxy = !m_sProxyHost.IsEmpty();
  if (!socket.Connect(bUseProxy ? m_sProxyHost : server.m_sSite, bUseProxy ? m_nProxyPort : server.m_nPort))
  {
    m_dwLastError = 0;
    TRACE(_T("Could not connect to the CDDB HTTP server %s, GetLastError:%d, %s\n"), server.m_sSite, GetLastError(), GetErrorMessage());
    return FALSE;
  }
  else
  {
    //Form the specialized HTTP request to retrieve the query data
    CString sDiscID;
    sDiscID.Format(_T("%08x"), dwDiscID);

    int nTracks = tracks.GetSize() - 1; //Number of tracks is 1 less than the size
                                        //of the array as it also contains the lead 
                                        //out position

    CString sTracks;
    sTracks.Format(_T("%d"), nTracks);

    CString sOffsets;
    for (int i=0; i<nTracks; i++)
    {
      //Form the frame number
      CString sFrame;
      sFrame.Format(_T("%d"), tracks[i].m_nMinute*60*75 + tracks[i].m_nSecond*75 + tracks[i].m_nFrame);

      //Accumulate the frame number into the string
      sOffsets += sFrame;

      //Add a space between each offset
      if (i<(nTracks-1))
       sOffsets += _T("+");
    }

    //Compute the playing length in seconds by using the lead out position
    int nCDlength = tracks[nTracks].m_nMinute*60 + tracks[nTracks].m_nSecond;
    CString sLength;
    sLength.Format(_T("%d"), nCDlength);

    //Form the full request string
    CString sRequest;
    if (m_sProxyHost.IsEmpty())
      sRequest.Format(_T("GET %s?cmd=cddb+query+%s+%s+%s+%s&%s HTTP/1.0\r\n\r\n"), server.m_sAddress, sDiscID, sTracks, sOffsets, sLength, GetHelloCommand());
    else
      sRequest.Format(_T("GET http://%s?cmd=cddb+query+%s+%s+%s+%s&%s HTTP/1.0\r\n%s\r\n\r\n"), server.m_sSite+server.m_sAddress, sDiscID, sTracks, sOffsets, sLength, GetHelloCommand(), GetProxyHeader(server.m_sSite));

    //Send the request through the socket
    LPCSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    int nCmdLength = strlen(pszRequest);
    if (!socket.Send(pszRequest, nCmdLength))
    {
      m_dwLastError = 0;
      TRACE(_T("An unexpected error occurred while sending the HTTP query request, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }
    
    //Read the response back through the socket
    char* pszBuffer;
    if (ReadResponse(socket, "\r\n.\r\n", pszBuffer, 4096, READ_RESPONSE_QUERY))
    {
      ASSERT(pszBuffer);

      //Extract the HTTP body from the response;
      LPSTR pszBody = FindHTTPBody(pszBuffer);

      //Parse out the query info from the body
	  if ( !pszBody )
		  bSuccess = FALSE;
	  else
		bSuccess = ParseQueryBody(pszBody, sDiscID, results);
    }
    else
    {
      //Reponse could not be retrieved
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
    	TRACE(_T("CDDB server did not respond correctly to the query command\n"));
    }

    //Don't forget to delete the allocated buffer
    delete [] pszBuffer;

    return bSuccess;
  }
}

BOOL CCDDB::ParseQueryBody(LPSTR pszBody, const CString& sDiscID, CArray<CCDDBQueryResult, CCDDBQueryResult&>& results)
{
  //setup return value
  BOOL bSuccess = FALSE;

  //Hive away the last reponse until everything is ok
  m_sLastCommandResponse = pszBody;

  //From the HTTP body get the CDDB response code
  int nResponseCode = GetCDDBReponseCode(pszBody);

  /*
  char buffer[32];
  itoa( nResponseCode, buffer, 10 );
  MessageBox( NULL, buffer, NULL, NULL );
  */

  //Remove any entries which are in the query array
  results.RemoveAll();

  //If the response is in the expected range (21x) then parse the query data line by line
  if (nResponseCode >= 210 && nResponseCode <= 219)
  {
    //Get the query data and then iterate through all the lines
    LPSTR pszLine = SkipToNextLine(pszBody);
    while (pszLine)
    {
      //Get the next line before we parse the current line
      LPSTR pszNextLine = GetNextLine(pszLine);

      //If we can parse the current line then add it 
      //to the query array
      CCDDBQueryResult result;
      if (result.Parse(pszLine))
      {
        results.Add(result);
        bSuccess = TRUE;
      }
      
      //Move on to the next line  
      pszLine = pszNextLine;
    }

    //Everything is successful, empty out the last command reponse string
    m_sLastCommandResponse.Empty();
  }
  else if (nResponseCode == 200 && strlen(pszBody) > 5)
  {
    //found an exact match, If we can parse the 
    //current line then add it to the query array
    CCDDBQueryResult result;
    if (result.Parse(pszBody+4))
    {
      results.Add(result);
      bSuccess = TRUE;
    }

    //Everything is successful, empty out the last command reponse string
    m_sLastCommandResponse.Empty();
  }
  else if (nResponseCode == 202)
  {
    //Deliberately do nothing other than a trace
    TRACE(_T("CDDB server does not contain the specifed album with DISCID:%s in its database\n"), sDiscID);
  }
  else
  {
    //An unexpected response was retrieved
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a valid query response code\n"));
  }

  return bSuccess;
}

BOOL CCDDB::Read(const CCDDBSite& server, DWORD dwDiscID, const CString& sCategory, CCDDBRecord& record)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Create the socket
  CHTTPSocket socket;
  if (!socket.Create())
  {
    m_dwLastError = 0;
    TRACE(_T("Failed to create client socket for read command, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Connect to the host on the specified port
  BOOL bUseProxy = !m_sProxyHost.IsEmpty();
  if (!socket.Connect(bUseProxy ? m_sProxyHost : server.m_sSite, bUseProxy ? m_nProxyPort : server.m_nPort))
  {
    m_dwLastError = 0;
    TRACE(_T("Could not connect to the CDDB HTTP server %s, GetLastError:%d, %s\n"), server.m_sSite, GetLastError(), GetErrorMessage());
    return FALSE;
  }
  else
  {
    //Form the full request string
    CString sDiscID;
    sDiscID.Format(_T("%08x"), dwDiscID);

    CString sRequest;
    if (!bUseProxy)
      sRequest.Format(_T("GET %s?cmd=cddb+read+%s+%s&%s HTTP/1.0\r\n\r\n"), server.m_sAddress, sCategory, sDiscID, GetHelloCommand());
    else
      sRequest.Format(_T("GET http://%s?cmd=cddb+read+%s+%s&%s HTTP/1.0\r\n%s\r\n\r\n"), server.m_sSite+server.m_sAddress, sCategory, sDiscID, GetHelloCommand(), GetProxyHeader(server.m_sSite));  

    //Send the request through the socket
    LPCSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    int nCmdLength = strlen(pszRequest);
    if (!socket.Send(pszRequest, nCmdLength))
    {
      m_dwLastError = 0;
      TRACE(_T("An unexpected error occurred while sending the HTTP stat request, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }

    //Read the response back through the socket
    char* pszBuffer;
    if (ReadResponse(socket, "\r\n.\r\n", pszBuffer))
    {
      ASSERT(pszBuffer);

      //Hive away the last reponse until everything is ok
      m_sLastCommandResponse = pszBuffer;

      //Extract the HTTP body from the response;
      LPSTR pszBody = FindHTTPBody(pszBuffer);

      //Parse the CDDB record
      bSuccess = ParseRecordBody(pszBody, record);

      if (record.m_dwDiscID != dwDiscID)
      {
        TRACE(_T("Returned DiscID from CDDB read (%x) is different than the computed value (%x), using the computed value\n"), record.m_dwDiscID, dwDiscID);
        record.m_dwDiscID = dwDiscID;      
      }
    }
    else
    {
      //Reponse could not be Retrieved
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
    	TRACE(_T("CDDB server did not respond correctly to the stat command\n"));
    }

    //Don't forget to delete the allocated buffer
    delete [] pszBuffer;

    return bSuccess;
  }
}

void CCDDB::ReplaceSubString(CString& sText, const CString& sFind, const CString& sReplacement)
{
  CString sRet;

  CString sLocalText(sText);
  int nFindLength = sFind.GetLength();
  int nFind = sLocalText.Find(sFind);
  BOOL bAnythingReplaced = FALSE;
  while (nFind != -1)
  {
    bAnythingReplaced = TRUE;
    CString sTemp = sLocalText.Left(nFind);
    sTemp += sReplacement;
    sRet += sTemp;

    //Look for any more strings to replace prior to looping around
    sLocalText = sLocalText.Right(sLocalText.GetLength() - nFind - nFindLength);
    nFind = sLocalText.Find(sFind);
    if (nFind == -1)
      sRet += sLocalText;
  }

  if (bAnythingReplaced)
    sText = sRet;
}

BOOL CCDDB::ParseKeywordLine(LPCSTR pszLine, CString& sKeyword, CString& sValue)
{
  //Validate out parameters
  ASSERT(pszLine);
  
  //Duplicate the string into a local variable 
  int nLength = strlen(pszLine);
  LPSTR pszData = new char[nLength + 1];
  strcpy(pszData, pszLine);

  //Do the actual parsing
  BOOL bSuccess = FALSE;
  char* pszToken = strstr(pszData, "=");
  if (pszToken)
  {
    pszToken[0] = '\0';
    sKeyword = pszData;
    sValue = pszToken+1;

    //Replace all strings as specified in the cddb document
    ReplaceSubString(sValue, _T("\\n"), _T("\n"));
    ReplaceSubString(sValue, _T("\\t"), _T("\t"));
    ReplaceSubString(sValue, _T("\\\\"), _T("\\"));

    bSuccess = TRUE;
  }

  //Dont forget to delete our local string
  delete [] pszData;

  return bSuccess;
}

BOOL CCDDB::ParseRecordBodyHelper(LPSTR pszRecord, CCDDBRecord& record)
{
  //Setup all the booleans which indicate a successful record parse.
  //These will be check
  BOOL bFoundXMCDComment = FALSE;
  BOOL bParsingTrackFrameOffsets = FALSE;
  BOOL bFoundDiscLength = FALSE;
  BOOL bFoundSubmittedVia = FALSE;
  BOOL bFoundDISCID = FALSE;
  BOOL bFoundDTITLE = FALSE;
  CString sTitle;
  CString sPlayOrder;

  //Initialize the record to default values
  record = CCDDBRecord();

  //Loop thro each line 
  LPSTR pszNextLine = NULL;
  do
  {
    //Get the next line before we parse the current one
    pszNextLine = GetNextLine(pszRecord);

    //Is it a comment line?
    CString sValue;
    if (ParseCommentLine(pszRecord, sValue))
    {
      if (sValue.Find(_T("xmcd")) == 0)
      {
        //Handle the "# xmcd" line
        bFoundXMCDComment = TRUE;
      }
      else if (sValue.Find(_T("Track frame offsets:")) != -1)
      {
        //Handle the "# Track frame offsets:" line
        bParsingTrackFrameOffsets = TRUE;
      }
      else if (sValue.Find(_T("Disc length:")) == 0)
      {
        //If we've found the disc length line then there are no more track frame offset lines
        bParsingTrackFrameOffsets = FALSE;

        //Handle the "# Disc length:" line
        bFoundDiscLength = TRUE;
        sValue = sValue.Right(sValue.GetLength() - 13);
        record.m_nDiskLength = ::_ttoi(sValue);
      }
      else if (sValue.Find(_T("Revision:")) == 0)
      {
        //Handle the "# Revision:" line
        sValue = sValue.Right(sValue.GetLength() - 9);
        record.m_nDatabaseRevision = ::_ttoi(sValue);
      }
      else if (sValue.Find(_T("Submitted via:")) == 0)
      {
        //Handle the "# Submitted via:" line
        bFoundSubmittedVia = TRUE;

        TCHAR pszSeparators[] = _T(" ");
        sValue = sValue.Right(sValue.GetLength() - 14);
        TCHAR* pszValue = sValue.GetBuffer(sValue.GetLength());
        TCHAR* pszToken = _tcstok(pszValue, pszSeparators);
        if (pszToken)
          record.m_sClientName = pszToken;
        else
        {
          sValue.ReleaseBuffer();
          m_dwLastError = 0;
          SetLastError(WSAEPROTONOSUPPORT);
          TRACE(_T("CDDB server failed to return a record with a valid Submitted via: line\n"));
          return FALSE;
        }

        //Pull out the client version
        pszToken = _tcstok(NULL, pszSeparators);
        if (pszToken)
          record.m_sClientVersion = pszToken;
        else
        {
          sValue.ReleaseBuffer();
          m_dwLastError = 0;
          SetLastError(WSAEPROTONOSUPPORT);
          TRACE(_T("CDDB server failed to return a record with a valid Submitted via: line\n"));
          return FALSE;
        }

        //Pull out the client comments
        pszToken = _tcstok(NULL, pszSeparators);
        if (pszToken)
          record.m_sClientComments = pszToken;
        else
          record.m_sClientComments.Empty();

        sValue.ReleaseBuffer();
      }
      else if (bParsingTrackFrameOffsets)
      {
        if (sValue.GetLength())
        {
          //Handle all the actual track frame offsets lines
          int nTrackOffset = ::_ttoi(sValue);
          record.m_TrackOffsets.Add(nTrackOffset);
        }
      }
    }
    else
    {
      //Musn't be a comment line
      CString sKeyword;
      if (ParseKeywordLine(pszRecord, sKeyword, sValue))
      {
        if (sKeyword == _T("DISCID"))
        {
          if (sValue.GetLength() >= 8)
          {
            //handle the "DISCID" line
            bFoundDISCID = TRUE;
            _stscanf(sValue, _T("%x"), &record.m_dwDiscID);
          }            
        }
        else if (sKeyword == _T("DTITLE"))
        {
          //Handle the "DTITLE" line
          bFoundDTITLE = TRUE;
          sTitle += sValue;
        }
        else if (sKeyword == _T("DYEAR"))
        {
          //Handle the "DYEAR" line
          record.m_nYear = ::_ttoi(sValue);
        }
        else if (sKeyword == _T("DGENRE"))
        {
          //Handle the "DGENRE" line
          record.m_sGenre += sValue;
        }
        else if (sKeyword.Find(_T("TTITLE")) == 0)
        {
          //Handle the "TTITLE" lines

          //work out the offset and add the string
          CString sTrack = sKeyword.Right(sKeyword.GetLength() - 6);
          int nOffset = _ttoi(sTrack);
          record.m_TrackTitles.SetSize(nOffset+1);
          record.m_TrackTitles.ElementAt(nOffset) += sValue;
        }
        else if (sKeyword == _T("EXTD"))
        {
          //Handle any "EXTD" lines
          record.m_ExtendedData += sValue;
        }
        else if (sKeyword.Find(_T("EXTT")) == 0)
        {
          //work out the offset and add the string
          CString sTrack = sKeyword.Right(sKeyword.GetLength() - 4);
          int nOffset = _ttoi(sTrack);
          record.m_ExtendedTrackData.SetSize(nOffset+1);
          record.m_ExtendedTrackData.ElementAt(nOffset) += sValue;
        }
        else if (sKeyword == _T("PLAYORDER"))
        {
          //Handle the "PLAYORDER" line
          sPlayOrder += sValue;
        }
      }
    }

    //Get ready for the next time around
    pszRecord = pszNextLine;
  }
  while (pszNextLine);

  //Handle all the parsing errors
  if (!bFoundXMCDComment)
  {
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a record with a XMCD comment line\n"));
    return FALSE;
  }

  if (record.m_TrackOffsets.GetSize() == 0)
  {
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a record with any Track frame offset lines\n"));
    return FALSE;
  }

  if (!bFoundDiscLength)
  {
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a record with a Disc length line\n"));
    return FALSE;
  }

  if (!bFoundSubmittedVia)
  {
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a record with a Submiteed via line\n"));
    return FALSE;
  }

  if (!bFoundDISCID)
  {
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a record with a DISCID line\n"));
    return FALSE;
  }

  if (!bFoundDTITLE)
  {
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a record with a DTITLE line\n"));
    return FALSE;
  }

  if (record.m_TrackTitles.GetSize() != record.m_ExtendedTrackData.GetSize())
  {
    m_dwLastError = 0;
    SetLastError(WSAEPROTONOSUPPORT);
    TRACE(_T("CDDB server failed to return a record with an equal amount of TTITLE lines to EXTT lines\n"));
    return FALSE;
  }

  //Put together the details from the "DTITLE" line(s)
  int nFind = sTitle.Find(_T(" / "));
  if (nFind != -1)
  {
    record.m_sArtist = sTitle.Left(nFind);
    record.m_sTitle = sTitle.Right(sTitle.GetLength() - nFind - 3);
  }
  else
  {
    //Artist & Title are the same
    record.m_sTitle = sTitle;
    record.m_sArtist = sTitle;
  }

  //Handle the "PLAYORDER" line
  BOOL bContinue;
  do 
  {
    int nFind = sPlayOrder.Find(_T(","));
    if (nFind != -1)
    {
      CString sNum = sPlayOrder.Left(nFind);
      int nNum = ::_ttoi(sNum);
      record.m_PlayOrder.Add(nNum);
      sPlayOrder = sPlayOrder.Right(sPlayOrder.GetLength() - nFind - 1);
      bContinue = TRUE;
    }
    else 
    {
      if (sPlayOrder.GetLength())
      {
        int nNum = ::_ttoi(sPlayOrder);
        record.m_PlayOrder.Add(nNum);
      }
      bContinue = FALSE;
    }
  }
  while (bContinue);

  return TRUE;
}

BOOL CCDDB::ParseRecordBody(LPSTR pszBody, CCDDBRecord& record)
{
  //Hive away the last reponse until everything is ok
  m_sLastCommandResponse = pszBody;

  //From the HTTP body get the CDDB response code
  int nResponseCode = GetCDDBReponseCode(pszBody);

  //If the response is in the expected range (21x) then parse the query data line by line
  if (nResponseCode >= 210 && nResponseCode <= 219)
  {
    //Get the query data and then iterate through all the lines
    LPSTR pszCurLine = SkipToNextLine(pszBody);
    if (pszCurLine == NULL)
    {
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
      TRACE(_T("CDDB server failed to return a valid xmcd record\n"));
      return FALSE;
    }

    //Hand of to the helper function
    if (!ParseRecordBodyHelper(pszCurLine, record))
      return FALSE;
  }

  //Everything successful, then we can empty out the last command response
  m_sLastCommandResponse.Empty();

  //Return the status code
  return TRUE;
}

void CCDDB::PrepareTextForSubmit(CString& sText)
{
  ReplaceSubString(sText, _T("\\"), _T("\\\\"));
  ReplaceSubString(sText, _T("\n"), _T("\\n"));
  ReplaceSubString(sText, _T("\t"), _T("\\t"));
}

CString CCDDB::FormSubmitLine(const CString& sKeyword, const CString& sValue)
{
  CString sLine;
  CString sLocalValue(sValue);
  PrepareTextForSubmit(sLocalValue);
  int nLen = sLocalValue.GetLength();

  do
  {
    int nTotalSize = sKeyword.GetLength() + sLocalValue.GetLength() + 1;
    int nLineLength = min(254, nTotalSize);
    int nValueLength = nLineLength - sKeyword.GetLength() - 1;

    //Form the current line
    CString sBuf;
    sBuf.Format(_T("%s=%s\r\n"), sKeyword, sLocalValue.Left(nValueLength));

    //Add to what will be the return value
    sLine += sBuf;

    //Prepare for the next time around
    sLocalValue = sLocalValue.Right(sLocalValue.GetLength() - nValueLength);
  }
  while (sLocalValue.GetLength());

  return sLine;
}


BOOL CCDDB::Submit(const CCDDBSite& server, const CString& sCategory, const CString& sEmailAddress, 
                   CCDDBRecord& record, const CString& sRejectionNotice, BOOL bReallySubmit)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Create the socket
  CHTTPSocket socket;
  if (!socket.Create())
  {
    m_dwLastError = 0;
    TRACE(_T("Failed to create client socket for submit command, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Connect to the host on the specified port
  BOOL bUseProxy = !m_sProxyHost.IsEmpty();
  if (!socket.Connect(bUseProxy ? m_sProxyHost : server.m_sSite, bUseProxy ? m_nProxyPort : server.m_nPort))
  {
    m_dwLastError = 0;
    TRACE(_T("Could not connect to the CDDB HTTP server %s, GetLastError:%d, %s\n"), server.m_sSite, GetLastError(), GetErrorMessage());
    return FALSE;
  }
  else
  {
    //Form the specialized HTTP request to submit the album

    //Form the body
    
    //First the first few intro lines
    CString sBody = _T("# xmcd\r\n#\r\n# Track frame offsets:\r\n");  

    //Then the track offsets
    CString sBuf;
    for (int i=0; i<record.m_TrackOffsets.GetSize(); i++) 
    {
      sBuf.Format(_T("# %d\r\n"), record.m_TrackOffsets.GetAt(i));
      sBody += sBuf;
    }

    //Then the disc length
    sBuf.Format(_T("#\r\n# Disc length: %d seconds\r\n#\r\n"), record.m_nDiskLength); 
    sBody += sBuf;

    //Then the database revision
    sBuf.Format(_T("#\r\n# Revision: %d\r\n"), record.m_nDatabaseRevision); 
    sBody += sBuf;

    //Then the client details
    sBuf.Format(_T("# Submitted via: %s %s %s"), record.m_sClientName, record.m_sClientVersion, record.m_sClientComments);
    PrepareTextForSubmit(sBuf);
    ASSERT(sBuf.GetLength() <= 254); //Only 254 chars per line supported for the "# Submitted via:" line
    sBody += sBuf; 
    sBody += _T("\r\n#\r\n");

    sBuf.Format(_T("DISCID=%08x\r\n"), record.m_dwDiscID);  
    sBody += sBuf; //Then the discid

    //Then the DTITLE
    if (record.m_sArtist.GetLength())
      sBuf.Format(_T("%s / %s"), record.m_sArtist, record.m_sTitle);
    else
      sBuf = record.m_sTitle;
    sBuf = FormSubmitLine(_T("DTITLE"), sBuf);
    sBody += sBuf;  

    if (m_nProtocolVersion >= 5)
    {
      //Then the DYEAR
      ASSERT(record.m_nYear >= 0 && record.m_nYear <= 9999);
      if (record.m_nYear != 0)
        sBuf.Format(_T("DYEAR=%04d\r\n"), record.m_nYear);
      else
        sBuf = _T("DYEAR=\r\n");
      sBody += sBuf;

      //Then the DGENRE
      sBuf = FormSubmitLine(_T("DGENRE"), record.m_sGenre);
      sBody += sBuf;
    }

    //Then the track titles
    for (i=0; i<record.m_TrackTitles.GetSize(); i++)  
    {
      CString sKeyword;
      sKeyword.Format(_T("TTITLE%d"), i);
      sBuf = FormSubmitLine(sKeyword, record.m_TrackTitles.ElementAt(i));
      sBody += sBuf;
    }

    //Then the extended data
    sBuf = FormSubmitLine(_T("EXTD"), record.m_ExtendedData);
    sBody += sBuf; 

    //Then the extended track data
    for (i=0; i<record.m_ExtendedTrackData.GetSize(); i++) 
    {
      CString sLine;

      CString sKeyword;
      sKeyword.Format(_T("EXTT%d"), i);
      sBuf = FormSubmitLine(sKeyword, record.m_ExtendedTrackData.ElementAt(i));
      sBody += sBuf;
    }

    //Finally the PLAYORDER line (which is deliberately empty)
    sBody += _T("PLAYORDER="); 

    //Form the header into "sRequest"
    CString sRequest;
    if (m_sProxyHost.IsEmpty())
      sRequest.Format(_T("POST %s HTTP/1.0\r\nCategory: "), server.m_sAddress); //First the request line
    else
      sRequest.Format(_T("POST http://%s HTTP/1.0\r\n%s\r\nCategory: "), server.m_sSite+server.m_sAddress, GetProxyHeader(server.m_sSite)); //First the request line

    sRequest += sCategory; //Then the category
    sRequest += _T("\r\n");

    sBuf.Format(_T("Discid: %08x\r\n"), record.m_dwDiscID); //Then the DISCID
    sRequest += sBuf;

    sBuf.Format(_T("User-Email: %s\r\n"), sEmailAddress);
    sRequest += sBuf; //Then the User's email address

    if (bReallySubmit) //Then the submit mode
      sRequest += _T("Submit-Mode: submit\r\n");
    else
      sRequest += _T("Submit-Mode: test\r\n");                        

    ASSERT(sRejectionNotice.GetLength() <= 70); //Rejection notice is limited to 70 characters
    sBuf.Format(_T("X-Cddbd-Note: %s\r\n"), sRejectionNotice); //Then the rejection notice
    sRequest += sBuf;

    sBuf.Format(_T("Content-Length: %d\r\n\r\n"), sBody.GetLength()); 
    sRequest += sBuf; //Then the Content-Length

    //Add the body to the end of the request
    sRequest += sBody;   

    //Send the request through the socket
    LPCSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    int nCmdLength = strlen(pszRequest);
    if (!socket.Send(pszRequest, nCmdLength))
    {
      m_dwLastError = 0;
      TRACE(_T("An unexpected error occurred while sending the HTTP stat request, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }

    //Read the response back through the socket
    char* pszBuffer;
    if (ReadResponse(socket, "\r\n.\r\n", pszBuffer, 4096, READ_RESPONSE_SUBMIT))
    {
      ASSERT(pszBuffer);

      //Extract the HTTP body from the response;
      LPSTR pszBody = FindHTTPBody(pszBuffer);

      //Parse out the response
      bSuccess = ParseSubmitBody(pszBody);
    }
    else
    {
      //Reponse could not be Retrieved
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
    	TRACE(_T("CDDB server did not respond correctly to the submit command\n"));
    }

    //Don't forget to delete the allocated buffer
    delete [] pszBuffer;

    return bSuccess;
  }
}

BOOL CCDDB::ParseSubmitBody(LPSTR pszBody)
{
  //setup return value
  BOOL bSuccess = FALSE;

  //Hive away the last reponse until everything is ok
  m_sLastCommandResponse = pszBody;

  //From the HTTP body get the CDDB response code
  int nResponseCode = GetCDDBReponseCode(pszBody);

  //If the response is in the expected range (20x) then return success
  if (nResponseCode >= 200 && nResponseCode <= 209)
  {
    bSuccess = TRUE;
    m_sLastCommandResponse.Empty();
  }

  return bSuccess;
}

BOOL CCDDB::MessageOfTheDay(CString& sMessage, const CString& sServer, const CString& sAddress, int nPort)
{
  //Form the site which we will be getting the message from
  CCDDBSite server;
  server.m_sSite = sServer;
  server.m_nPort = nPort;
  server.m_sAddress = sAddress;
  
  //Let the other version 
  return MessageOfTheDay(server, sMessage);
}

BOOL CCDDB::MessageOfTheDay(const CCDDBSite& server, CString& sMessage)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Create the socket
  CHTTPSocket socket;
  if (!socket.Create())
  {
    m_dwLastError = 0;
    TRACE(_T("Failed to create client socket for motd command, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
    return FALSE;
  }

  //Connect to the host on the specified port
  BOOL bUseProxy = !m_sProxyHost.IsEmpty();
  if (!socket.Connect(bUseProxy ? m_sProxyHost : server.m_sSite, bUseProxy ? m_nProxyPort : server.m_nPort))
  {
    m_dwLastError = 0;
    TRACE(_T("Could not connect to the CDDB HTTP server %s, GetLastError:%d, %s\n"), server.m_sSite, GetLastError(), GetErrorMessage());
    return FALSE;
  }
  else
  {
    //Form the specialized HTTP request to retrieve the status data
    CString sRequest;
    if (m_sProxyHost.IsEmpty())
      sRequest.Format(_T("GET %s?cmd=motd&%s HTTP/1.0\r\n\r\n"), server.m_sAddress, GetHelloCommand());
    else
      sRequest.Format(_T("GET http://%s?cmd=motd&%s HTTP/1.0\r\n%s\r\n\r\n"), server.m_sSite+server.m_sAddress, GetHelloCommand(), GetProxyHeader(server.m_sSite));

    //Send the request through the socket
    LPCSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    int nCmdLength = strlen(pszRequest);
    if (!socket.Send(pszRequest, nCmdLength))
    {
      m_dwLastError = 0;
      TRACE(_T("An unexpected error occurred while sending the HTTP stat request, GetLastError:%d, %s\n"), GetLastError(), GetErrorMessage());
      return FALSE;
    }

    //Read the response back through the socket
    char* pszBuffer;
    if (ReadResponse(socket, "\r\n.\r\n", pszBuffer))
    {
      ASSERT(pszBuffer);

      //Extract the HTTP body from the response;
      LPSTR pszBody = FindHTTPBody(pszBuffer);

      //Parse out the message from the body
      bSuccess = ParseMessageBody(pszBody, sMessage);
    }
    else
    {
      //Reponse could not be Retrieved
      m_dwLastError = 0;
      SetLastError(WSAEPROTONOSUPPORT);
    	TRACE(_T("CDDB server did not respond correctly to the stat command\n"));
    }

    //Don't forget to delete the allocated buffer
    delete [] pszBuffer;

    return bSuccess;
  }
}

BOOL CCDDB::ParseMessageBody(LPSTR pszBody, CString& sMessage)
{
  //setup return value
  BOOL bSuccess = FALSE;

  //Hive away the last reponse until everything is ok
  m_sLastCommandResponse = pszBody;

  //From the HTTP body get the CDDB response code
  int nResponseCode = GetCDDBReponseCode(pszBody);

  //If the response is in the expected range (21x) then copy the message into "sMessage"
  if (nResponseCode >= 210 && nResponseCode <= 219)
  {
    //Get the query data and then iterate through all the lines
    LPSTR pszLine = SkipToNextLine(pszBody);

    if (pszLine)
    {
      sMessage = pszLine;
      bSuccess = TRUE;
      m_sLastCommandResponse.Empty();
    }
  }

  return bSuccess;
}

void CCDDB::SetProxyDetails(const CString& sHost, int nPort, const CString& sUser, const CString& sPassword)
{
  m_sProxyHost = sHost;
  m_nProxyPort = nPort;
  m_sProxyUser = sUser;
  m_sProxyPassword = sPassword;
}

int CCDDB::Base64BufferSize(int nInputSize)
{
  int nOutSize = (nInputSize+2)/3*4;                    // 3:4 conversion ratio
  nOutSize += strlen(EOL)*nOutSize/BASE64_MAXLINE + 3;  // Space for newlines and NUL
  return nOutSize;
}

BOOL CCDDB::EncodeBase64(const char* pszIn, int nInLen, char* pszOut, int nOutSize, int* nOutLen)
{
  //Input Parameter validation
  ASSERT(pszIn);
  ASSERT(pszOut);
  ASSERT(nOutSize);
  ASSERT(nOutSize >= Base64BufferSize(nInLen));

#ifndef _DEBUG
  //justs get rid of "unreferenced formal parameter"
  //compiler warning when doing a release build
  nOutSize;
#endif

  //Set up the parameters prior to the main encoding loop
  int nInPos  = 0;
  int nOutPos = 0;
  int nLineLen = 0;

  // Get three characters at a time from the input buffer and encode them
  for (int i=0; i<nInLen/3; ++i) 
  {
    //Get the next 2 characters
    int c1 = pszIn[nInPos++] & 0xFF;
    int c2 = pszIn[nInPos++] & 0xFF;
    int c3 = pszIn[nInPos++] & 0xFF;

    //Encode into the 4 6 bit characters
    pszOut[nOutPos++] = m_base64tab[(c1 & 0xFC) >> 2];
    pszOut[nOutPos++] = m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
    pszOut[nOutPos++] = m_base64tab[((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6)];
    pszOut[nOutPos++] = m_base64tab[c3 & 0x3F];
    nLineLen += 4;

    //Handle the case where we have gone over the max line boundary
    if (nLineLen >= BASE64_MAXLINE-3) 
    {
      char* cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      nLineLen = 0;
    }
  }

  // Encode the remaining one or two characters in the input buffer
  char* cp;
  switch (nInLen % 3) 
  {
    case 0:
    {
      cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      break;
    }
    case 1:
    {
      int c1 = pszIn[nInPos] & 0xFF;
      pszOut[nOutPos++] = m_base64tab[(c1 & 0xFC) >> 2];
      pszOut[nOutPos++] = m_base64tab[((c1 & 0x03) << 4)];
      pszOut[nOutPos++] = '=';
      pszOut[nOutPos++] = '=';
      cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      break;
    }
    case 2:
    {
      int c1 = pszIn[nInPos++] & 0xFF;
      int c2 = pszIn[nInPos] & 0xFF;
      pszOut[nOutPos++] = m_base64tab[(c1 & 0xFC) >> 2];
      pszOut[nOutPos++] = m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
      pszOut[nOutPos++] = m_base64tab[((c2 & 0x0F) << 2)];
      pszOut[nOutPos++] = '=';
      cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      break;
    }
    default: 
    {
      ASSERT(FALSE); 
      break;
    }
  }
  pszOut[nOutPos] = 0;
  *nOutLen = nOutPos;
  return TRUE;
}

CString CCDDB::GetProxyHeader(const CString& sHost)
{
  //Validate the parameters
  ASSERT(m_sProxyHost.GetLength());

	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  CString sLine;

  //If we need authentication
  if (m_sProxyUser.GetLength())
  {
    CString sCombinedUserAndPassword;
    sCombinedUserAndPassword.Format(_T("%s:%s"),m_sProxyUser, m_sProxyPassword);
	  LPSTR lpszAsciiCombinedUserAndPassword = T2A((LPTSTR) (LPCTSTR) sCombinedUserAndPassword);
    int nSize = strlen(lpszAsciiCombinedUserAndPassword);

    //allocate the encoded buffer
    int nOutSize = Base64BufferSize(nSize);
    char* pszEncoded = new char[nOutSize];

    //Do the encoding
    int nEncodedSize=0;
    EncodeBase64(lpszAsciiCombinedUserAndPassword, nSize, pszEncoded, nOutSize, &nEncodedSize);

    //Form the header 
    sLine.Format(_T("Proxy-Authorization: Basic %s\r\nHost: %s"), A2T(pszEncoded), sHost);

    //delete the encoded temp buffer
    delete [] pszEncoded;
  }
  else
  {
    //Just add the Host header
    sLine.Format(_T("Host: %s"), sHost);
  }

  //Return the formed string
  return sLine;
}

BOOL CCDDB::Read(const CString& sDirectory, DWORD dwDiscID, BOOL bStandardForm, CCDDBRecord& record, CString& sFileSystemGenre)
{
  //Assume the worst
  BOOL bFound = FALSE;

  //First iterate thro the genre subdirectories
  CFileFind genreFileFind;
  BOOL bContinueGenreSearch = genreFileFind.FindFile(sDirectory + _T("\\*.*"));
  while (bContinueGenreSearch && !bFound)
  {
    //Get ready for the next time around
    bContinueGenreSearch = genreFileFind.FindNextFile();
    if (!genreFileFind.IsDots() && genreFileFind.IsDirectory())
    {
      if (bStandardForm)
      {
        CString sFilename;
        sFilename.Format(_T("%s\\%08X"), genreFileFind.GetFilePath(), dwDiscID);
        bFound = ParseRecordBodyFromFile(dwDiscID, sFilename, TRUE, record);

        //Update the Genre details of the record
        if (bFound)
          sFileSystemGenre = genreFileFind.GetFileTitle();
      }
      else
      {
        //Now iterate thro all the files in this genre directory
        CFileFind filesFileFind;
        BOOL bContinueFileSearch = filesFileFind.FindFile(genreFileFind.GetFilePath() + _T("\\??to??"));
        while (bContinueFileSearch && !bFound)
        {
          //Get ready for the next time around
          bContinueFileSearch = filesFileFind.FindNextFile();

          //Is the filename in the range of the discid we are looking for
          CString sFile = filesFileFind.GetFileTitle();

          DWORD dwLowerRange = _tcstoul(sFile.Left(2), NULL, 16);
          DWORD dwUpperRange = _tcstoul(sFile.Mid(4, 2), NULL, 16);
          if ((dwLowerRange <= (dwDiscID>>24)) && ((dwDiscID>>24) <= dwUpperRange))
          {
            bFound = ParseRecordBodyFromFile(dwDiscID, filesFileFind.GetFilePath(), FALSE, record);

            //Update the Genre details of the record
            if (bFound)
              sFileSystemGenre = genreFileFind.GetFileTitle();
          }
        }
      }
    }
  }

  return bFound;
}

BOOL CCDDB::ParseRecordBodyFromFile(DWORD dwDiscID, const CString& sFile, BOOL bStandardForm, CCDDBRecord& record)
{
  USES_CONVERSION;

  //Assume the worst
  BOOL bFound = FALSE;

  if (bStandardForm)
  {
    CFile file;
    if (file.Open(sFile, CFile::modeRead | CFile::shareDenyWrite))
    {
      try
      {
        //Read all the data in 
        DWORD dwSize = file.GetLength();
        char* pszRecordData = new char[dwSize+1];
        pszRecordData[0] = '\0';
        UINT nRead = file.Read(pszRecordData, dwSize);
        pszRecordData[nRead] = '\0';

        //Hand of all the work to the helper function
        bFound = ParseRecordBodyHelper(pszRecordData, record);

        //tidy up the heap memory we have used
        delete [] pszRecordData;
      }
      catch(CFileException* pEx)
      {
        SetLastError(pEx->m_lOsError);
        pEx->Delete();
      }
    }
  }
  else
  {
    CStdioFile file;
    if (file.Open(sFile, CFile::typeText | CFile::modeRead | CFile::shareDenyWrite))
    {
      CString sRecordData;

      CString sFirstInterestingLine;
      sFirstInterestingLine.Format(_T("#FILENAME=%08X"), dwDiscID);

      BOOL bMore = TRUE;
      while (bMore)
      {
        CString sLine;
        try
        {
          bMore = file.ReadString(sLine);
          CString sTempLine(sLine);
          sTempLine.MakeUpper();
          if (sTempLine == sFirstInterestingLine)
            bFound = TRUE;
          else if (bFound)
          {
            if (sLine.Find(_T("#FILENAME")) != -1)
              bMore = FALSE;
            else
            {
              sRecordData += sLine;
              sRecordData += _T("\r\n");
            }
          }
        }
        catch(CFileException* pEx)
        {
          SetLastError(pEx->m_lOsError);
          pEx->Delete();
          bMore = FALSE;
        }
      }

      //Hand of all the work to the helper function
      if (bFound)
        bFound = ParseRecordBodyHelper(T2A((LPTSTR) (LPCTSTR) sRecordData), record);
    }
  }

  return bFound;
}