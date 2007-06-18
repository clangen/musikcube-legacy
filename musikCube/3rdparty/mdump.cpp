
#include "stdafx.h"

#include "mdump.h"



MiniDumper::MiniDumper( /*const char szAppName*/ )
{
	// if this assert fires then you have two instances of MiniDumper
	// which is not allowed

	::SetUnhandledExceptionFilter( TopLevelFilter );
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	USES_CONVERSION;
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, A2W(szDbgHelpPath), _MAX_PATH ))
	{
		char *pSlash = strchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy( A2W(pSlash+1), A2W("DBGHELP.DLL") );
			hDll = ::LoadLibrary( A2W(szDbgHelpPath) );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( A2W("DBGHELP.DLL") );
	}

	char* szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char szDumpPath[260];
			strcat(szDumpPath, "./musikcube.dmp");
			char* szScratch;
			
			// ask the user if they want to save a dump file
			// create the file
				HANDLE hFile = ::CreateFile( A2W("./musikcube.dmp"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
					if (bOK)
					{
						sprintf( szScratch, "Saved dump file");
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						sprintf( szScratch, "Failed to save dump file (error %d)", GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					sprintf( szScratch, "Failed to create dump file (error %d)", GetLastError() );
					szResult = szScratch;
				}
			
		}
		else
		{
			strcat(szResult, "DBGHELP.DLL too old");
		}
	}
	else
	{
		strcat(szResult, "DBGHELP.DLL not found");
	}

	if (szResult)
		::MessageBox( NULL, (LPCWSTR)szResult, (LPCWSTR)"musikCube", MB_OK );

	return retval;
}
