///////////////////////////////////////////////////

#if defined( USE_BASS )

///////////////////////////////////////////////////

#ifndef __C_MUSIK_BASS_STREAM_H__
#define __C_MUSIK_BASS_STREAM_H__

///////////////////////////////////////////////////

#include "musikCore.h"
#include "bass.h"

namespace musikCore {

///////////////////////////////////////////////////
    
class MUSIKEXPORT Stream
{
public:

	typedef HSTREAM (*LOADFILE)( const String& file );
	typedef bool (*PLAY)( HSTREAM stream, int offset );
	typedef bool (*CANSEEK)();
	typedef bool (*SEEK)( HSTREAM stream, int ms );
	typedef bool (*PAUSE)(HSTREAM stream );
	typedef bool (*RESUME)(HSTREAM stream );
	typedef bool (*STOP)( HSTREAM stream );
	typedef int (*GETDURATION)( HSTREAM stream );
	typedef int (*GETTIME)( HSTREAM stream );
	typedef bool (*ISACTIVE)( HSTREAM stream );
	typedef bool (*CANPLAY)( const String& file );
	typedef bool (*WRITETAG)( const SongInfo& info );
	typedef bool (*READTAG)( const String& fn, SongInfo& target );
	typedef void (*UNLOAD)();

	typedef const String& (*GETPLUGINFORMATS)();
	typedef const String& (*GETPLUGINNAME)();
	typedef const String& (*GETPLUGINDESCRIPTION)();

public:

	Stream();
	Stream( const String& filename );
	~Stream();
	bool IsLoaded(){ return m_IsLoaded; }
	HSTREAM GetStream(){ return m_Stream; }

	LOADFILE LoadFile;
	PLAY Play;
	CANSEEK CanSeek;
	SEEK Seek;
	PAUSE Pause;
	RESUME Resume;
	STOP Stop;
	GETDURATION GetDuration;
	GETTIME GetTime;
	ISACTIVE IsActive;
	CANPLAY CanPlay;
	WRITETAG WriteTag;
	READTAG ReadTag;
	UNLOAD Unload;
	GETPLUGINFORMATS GetPluginFormats;
	GETPLUGINNAME GetPluginName;
	GETPLUGINDESCRIPTION GetPluginDescription;

	HSTREAM m_Stream;		// our stream
	HDSP	m_DSP;			// used by PlayerBASS for equalizer

	bool m_IsLoaded;

public: 

	paramlist m_Paramlist;	// for the equalizer
	ShibatchEQ m_Shibatch;	// our param list
};    

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

