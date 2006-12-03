/*
Module : MfcCDDB.H
Purpose: Defines the interface for an MFC class to wrap access to CDDB
Created: PJN / 26-02-1999


Copyright (c) 1999 - 2003 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 


*/

#ifndef __MFCCDDB_H__
#define __MFCCDDB_H__


#ifndef _WINSOCKAPI_
#pragma message("CDDB classes require winsock.h or afxsock.h in your PCH")
#endif

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#pragma message("to avoid this message, you should put afxtempl.h in your PCH")
#endif


//Automatically pull in the library winmm.lib since we
//need MCI to retrieve the cdaudio track details
#pragma comment(lib, "winmm.lib")


//Class which encapsulates a CDDB site
class CCDDBSite
{
public:
//Constructors / Destructors
  CCDDBSite(BOOL bRetrieve = TRUE);
  CCDDBSite(const CCDDBSite& site);

//Methods
  CCDDBSite& operator=(const CCDDBSite& site);

//Data variables
  CString m_sSite;             //The site name e.g. "freedb.freedb.org"
  int     m_nPort;             //The port number to connect on, normally the HTTP port i.e. 80
  CString m_sAddress;          //The address of the HTTP file to use, normally either "/~cddb/cddb.cgi" or "/~cddb/submit.cgi"
  BOOL    m_bNorthing;         //Is this site north of the equator
  int     m_nLatitudeMinutes;  //Minutes of degrees north of the equator 
  BOOL    m_bEasting;          //Is this site east of the prime meridian
  int     m_nLongitudeMinutes; //Minutes of degrees east of the prime meridian
  CString m_sDescription;      //Short description of the geographical location of the site

protected:
  BOOL Parse(LPSTR pszLine);
  
  friend class CCDDB;
};


//Class which represents the single instance of the
//results of a call to CCDDB::Query
class CCDDBQueryResult
{
public:
  CCDDBQueryResult();
  CCDDBQueryResult(const CCDDBQueryResult& result);

//Methods
  CCDDBQueryResult& operator=(const CCDDBQueryResult& result);

//Data variables
  CString m_sCategory;  //The category of this CD
  DWORD   m_dwDiscID;   //The CDDB DISCID of this CD
  CString m_sArtist;    //The Artist of this CD
  CString m_sTitle;     //The Disc title of the CD

protected:
  BOOL Parse(LPSTR pszLine);

  friend class CCDDB;
};


//Class which represents a single instance of the
//results as returned from CCDDB::Read and used in 
//the call to CCDDB::Submit
class CCDDBRecord
{
public:
  CCDDBRecord();
  CCDDBRecord(const CCDDBRecord& record);

//Methods
  CCDDBRecord& operator=(const CCDDBRecord& result);

//Data variables
  CArray<int, int&> m_TrackOffsets;             //The offsets of each track in frames
  int               m_nDiskLength;              //The Disc length in seconds
  int               m_nDatabaseRevision;        //The database revision number of this record
  CString           m_sClientName;              //The name of the client which has submitted this record
  CString           m_sClientVersion;           //The version of the client which has submitted this record
  CString           m_sClientComments;          //Optional client comments
  DWORD             m_dwDiscID;                 //The CDDB DISC-ID of this CD
  CString           m_sArtist;                  //The Artist of this CD
  CString           m_sTitle;                   //The Disc title of the CD
  CStringArray      m_TrackTitles;              //All the track titles on this CD
  CString           m_ExtendedData;             //The extended data for this CD
  CStringArray      m_ExtendedTrackData;        //The extended data for each track on this CD
  CArray<int, int&> m_PlayOrder;                //The track play order if any for this CD
  int               m_nYear;                    //Year in which the CD was released
  CString           m_sGenre;                   //The genre of the disc in textual form
};


//Class which represents the result as returned from
//CCDDB::Status
class CCDDBStatus
{
public:
  int               m_nCurrentProtocol; //The servers current operating protocol level
  int               m_nMaxProtocol;     //The maximum supported protocol level
  BOOL              m_bGetsAllowed;     //Whether or not the client is allowed to get log information
  BOOL              m_bUpdatesAllowed;  //Whether or not the client is allowed to initiate a database update
  BOOL              m_bPostingAllowed;  //Whether or not the client is allowed to post new entries
  BOOL              m_bQuotes;          //Whether or not quoted arguements are enabled
  int               m_nCurrentUsers;    //The number of users currently connected to the server
  int               m_nMaxUsers;        //The number of users that can concurrently connect to the server
  BOOL              m_bStripExtended;   //Whether or not extended data is stripped by the server before presented to the user
  int               m_nDatabaseEntries; //Total number of entries in the database
  CStringArray      m_Categories;       //The categories held in the database
  CArray<int, int&> m_CategoryEntries;  //The number of entries for each category in the database
  CStringArray      m_PendingSites;     //List of sites that are fed new database entries from this server
  CArray<int, int&> m_PendingEntries;   //The number of entries that have yet to be transmitted to each site
};


//Simple class to store a tracks position in MSF format. This is
//required for computing the CDDB DISCID
class CCDDBTrackPosition
{
public:
//Constructors / Destructors
  CCDDBTrackPosition();
  CCDDBTrackPosition(const CCDDBTrackPosition& position);

//Methods
  CCDDBTrackPosition& operator=(const CCDDBTrackPosition& position);

//Member variables
  int m_nMinute;
  int m_nSecond;
  int m_nFrame;
};


//Simple socket wrapper class, used internally by the CCDDB class
class CHTTPSocket
{
public:
//Constructors / Destructors
  CHTTPSocket();
  ~CHTTPSocket();

//methods
  BOOL  Create();
  BOOL  Connect(LPCTSTR pszHostAddress, int nPort = 110);
  BOOL  Send(LPCSTR pszBuf, int nBuf);
  void  Close();
  int   Receive(LPSTR pszBuf, int nBuf);
  BOOL  IsReadible(BOOL& bReadible, DWORD dwTimeout);

protected:
  BOOL   Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
  SOCKET m_hSocket;
};


//The main CDDB support class
class CCDDB
{
public:
//Constructors / Destructors
  CCDDB();
  ~CCDDB();

//Local Methods
  void    GetCDROMDrives(CStringArray& drives);
  BOOL    ComputeDiscID(DWORD& dwDiscID, LPCTSTR pszDrive = NULL);
  DWORD   ComputeDiscID(const CArray<CCDDBTrackPosition, CCDDBTrackPosition&>& tracks);
  BOOL    GetTrackPositions(CArray<CCDDBTrackPosition, CCDDBTrackPosition&>& tracks, LPCTSTR pszDrive = NULL);

//Remote CDDB access methods
  void    SetCDDBProtocolVersion(int nProtocolVersion);
  int     GetCDDBProtocolVersion() const { return m_nProtocolVersion; };
  BOOL    Sites(CArray<CCDDBSite, CCDDBSite&>& sites, const CString& sServer = _T("freedb.freedb.org"), const CString& sAddress = _T("/~cddb/cddb.cgi"), int nPort = 80);
  BOOL    Sites(CArray<CCDDBSite, CCDDBSite&>& sites, const CCDDBSite& server);
  BOOL    Categories(const CCDDBSite& server, CStringArray& categories);
  BOOL    Status(const CCDDBSite& server, CCDDBStatus& status);
  BOOL    Query(const CCDDBSite& server, DWORD dwDiscID, const CArray<CCDDBTrackPosition, CCDDBTrackPosition&>& tracks, 
                CArray<CCDDBQueryResult, CCDDBQueryResult&>& results); 
  BOOL    Read(const CCDDBSite& server, DWORD dwDiscID, const CString& sCategory, CCDDBRecord& record);
  BOOL    Read(const CString& sDirectory, DWORD dwDiscID, BOOL bStandardForm, CCDDBRecord& record, CString& sFileSystemGenre);
  BOOL    Submit(const CCDDBSite& server, const CString& sCategory, const CString& sEmailAddress, 
                 CCDDBRecord& record, const CString& sRejectionNotice, BOOL bReallySubmit = TRUE);
  BOOL    MessageOfTheDay(CString& sMessage, const CString& sServer = _T("freedb.freedb.org"), const CString& sAddress = _T("/~cddb/cddb.cgi"), int nPort = 80);
  BOOL    MessageOfTheDay(const CCDDBSite& server, CString& sMessage);

//Simple Accessors / Mutators
  void    SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };
  DWORD   GetTimeout() const { return m_dwTimeout; };
  DWORD   GetLastError() const;
  CString GetErrorMessage() const;
  CString GetLastCommandResponse() const { return m_sLastCommandResponse; };
  CString GetProductName() const { return m_sProductName; };
  void    SetProductName(const CString& sProductName) { m_sProductName = sProductName; };
  CString GetProductVersion() const { return m_sProductVersion; };
  void    SetProductVersion(const CString& sProductVersion) { m_sProductVersion = sProductVersion; };

//Proxy support
  void    SetProxyDetails(const CString& sHost, int nPort, const CString& sUser, const CString& sPassword);
  void    SetProxyHost(const CString& sProxyHost) { m_sProxyHost = sProxyHost; };
  void    SetProxyPort(int nPort) { m_nProxyPort = nPort; };
  void    SetProxyUser(const CString& sUser) { m_sProxyUser = sUser; };
  void    SetProxyPassword(const CString& sPassword) { m_sProxyPassword = sPassword; };
  CString GetProxyHost() const { return m_sProxyHost; };
  int     GetProxyPort() const { return m_nProxyPort; };
  CString GetProxyUser() const { return m_sProxyUser; };
  CString GetProxyPassword() const { return m_sProxyPassword; };

//Virtual functions which derived classes can customize
  virtual CString GetHelloCommand();

protected:
//Internally used functions
  virtual CString GetUserName();
  virtual CString GetHostName();
  virtual BOOL ReadResponse(CHTTPSocket& socket, LPSTR pszTerminator, LPSTR& pszRecvBuffer, int nGrowBy = 4096, DWORD dwHint = 0);
  LPSTR   FindHTTPBody(LPCSTR pszResponse);
  LPSTR   SkipToNextLine(LPSTR pszLine);
  int     GetCDDBReponseCode(LPSTR pszBody);
  LPSTR   GetNextLine(LPSTR pszLine);
  BOOL    SplitSemiColonLine(LPCSTR pszLine, CString& sField, CString& sValue);
  BOOL    ParseStatusBody(LPSTR pszBody, CArray<CCDDBSite, CCDDBSite&>& sites);
  BOOL    ParseSitesBody(LPSTR pszBody, CArray<CCDDBSite, CCDDBSite&>& sites);
  BOOL    ParseCategoriesBody(LPSTR pszBody, CStringArray& categories);
  BOOL    ParseStatusBody(LPSTR pszBody, CCDDBStatus& status);
  BOOL    ParseQueryBody(LPSTR pszBody, const CString& sDiscID, CArray<CCDDBQueryResult, CCDDBQueryResult&>& results);
  BOOL    ParseRecordBody(LPSTR pszBody, CCDDBRecord& record);
  BOOL    ParseRecordBodyFromFile(DWORD dwDiscID, const CString& sFile, BOOL bStandardForm, CCDDBRecord& record);
  BOOL    ParseRecordBodyHelper(LPSTR pszRecord, CCDDBRecord& record);
  BOOL    ParseCommentLine(LPCSTR pszLine, CString& sValue);
  BOOL    ParseKeywordLine(LPCSTR pszLine, CString& sKeyword, CString& sValue);
  BOOL    ParseMessageBody(LPSTR pszBody, CString& sMessage);
  BOOL    ParseSubmitBody(LPSTR pszBody);
  void    ReplaceSubString(CString& sText, const CString& sFind, const CString& sReplacement);
  void    PrepareTextForSubmit(CString& sText);
  BOOL    EncodeBase64(const char* pszIn, int nInLen, char* pszOut, int nOutSize, int* nOutLen);
  int     Base64BufferSize(int nInputSize);
  CString GetProxyHeader(const CString& sHost);
  CString FormSubmitLine(const CString& sKeyword, const CString& sValue);

//Internal member variables
  CString m_sLastCommandResponse;
  DWORD   m_dwLastError;  
  DWORD   m_dwTimeout;
  CString m_sProductName;
  CString m_sProductVersion;
  int     m_nProtocolVersion;
  CString m_sProxyHost;
  int     m_nProxyPort;
  CString m_sProxyUser;
  CString m_sProxyPassword;
  static char m_base64tab[];
};


#endif //__MFCCDDB_H__