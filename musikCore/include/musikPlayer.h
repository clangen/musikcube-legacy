///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://www.musikcube.com
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2002-2004
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey at musikcube dot com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_PLAYER_H
#define C_MUSIK_PLAYER_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"
#include "musikPlaylist.h"
#include "musikTask.h"
#include "musikCrossfader.h"

#include "paramlist.hpp"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/** 
 * Playmode bit masks
 */
MUSIKEXPORT enum
{
	// playmodes
	MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE = 1,
	MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST = 2,
	MUSIK_PLAYER_PLAYMODE_INTRO = 4,
    MUSIK_PLAYER_PLAYMODE_RANDOM = 8
};

///////////////////////////////////////////////////

/** 
* Song advance types
*/
MUSIKEXPORT enum
{
	// song advance
	MUSIK_PLAYER_SONG_USER_ADVANCE,
	MUSIK_PLAYER_SONG_AUTO_ADVANCE
};

///////////////////////////////////////////////////

/** 
* Player status
*/
MUSIKEXPORT enum
{
	// init states
	MUSIK_PLAYER_INIT_ERROR = -1,
	MUSIK_PLAYER_INIT_UNINITIALIZED = 0,
	MUSIK_PLAYER_INIT_SUCCESS,

	// initialization values
	MUSIK_PLAYER_INIT_START,
	MUSIK_PLAYER_INIT_STOP,
	MUSIK_PLAYER_INIT_RESTART
};

///////////////////////////////////////////////////

/** 
* Time types
*/
MUSIKEXPORT enum
{
	MUSIK_TIME_SECS = 0,
	MUSIK_TIME_MSECS
};

///////////////////////////////////////////////////

/** 
* File extensions of supported formats. It is a good 
* idea to initialize this in the constructor of your
* derived class.
*/
extern StringArray g_PlayerFormats;
MUSIKEXPORT const StringArray& GetPlayerFormats();

///////////////////////////////////////////////////

/**
 * \ingroup musikCore
 * A "stream" is a currently playing song. This class
 * can be anything you want it to be, as long as your
 * derived class knows how to handle it.
 *
 * So, in your derived class create a new class...
 *
 * class Stream
 * {
 *   ...
 *   ...
 *   ...
 * };
 */
class MUSIKEXPORT Stream; // forward declaration

///////////////////////////////////////////////////

MUSIKEXPORT enum
{
    MUSIK_PLAYER_NORMAL = 0,
    MUSIK_PLAYER_CDROM
};

///////////////////////////////////////////////////

/**
Player and Library are the heart and soul of musikCore.

Player is a thread that controls playback, crossfading,
equalizer processing, and song i/o

A KEY POINT:

To make things easy, Player will manage it's own playlist, which
CAN be set via SetPlaylist( Playlist* ). After passing the address
of the existing playlist, Player is assumed as the new owner -- that is
it will take on freeing, modifying, and maintaining it. Once a user
has passed the playlist to the player with this call, he should never, ever
delete it manually.

There is also a SetPlaylist( Playlist& ) method that makes a copy of the
playlist passed by reference. This is a slower but safer method than
SetPlaylist( Playlist* )

- If in Windows, the programmer should use PlayerBASS as the sound system.
- If in Linux, the programmer should use PlayerXINE.

@author Casey Langen
*/
class MUSIKEXPORT Player : public Task
{
     
public: 	

	/**
	 * \param functor functor that events will be posted to.
	 * \param library library that will be used for querying currently playing 
	 * song information.
	 */
	Player( Functor* functor, Library* library );
	virtual ~Player();

	/**
	 * Initialize the musikCore sound system. 
	 *
	 * \param device device id. get a list of devices from Player::GetSoundDevices()
	 * \param driver driver id. get a list of drivers from Player::GetSoundDrivers()
	 * \param rate playback rate.
	 * \param mode init mode. MUSIK_PLAYER_INIT_START, MUSIK_PLAYER_INIT_STOP, MUSIK_PLAYER_INIT_RESTART
	 * \return MUSIK_PLAYER_INIT_SUCCESS if sound successfully initialized
	 */
	int  InitSound( int device, int driver, int rate, int mode = MUSIK_PLAYER_INIT_START );

	/**
	 * Deinitialize the sound system. This will also stop playback.
	 */
	void DeinitSound(){ DeinitSoundSystem(); }

	/**
	 * \return Retrieve the active or last known crossfade type. Return codes may include
	 * MUSIK_CROSSFADER_NONE, MUSIK_CROSSFADER_NEW_SONG, MUSIK_CROSSFADER_PAUSE_RESUME, 
	 * MUSIK_CROSSFADER_SEEK, MUSIK_CROSSFADER_STOP, MUSIK_CROSSFADER_EXIT
	 */
	int GetFadeType(){ return m_FadeType; }
	/**
	 * \return The settings (durations) for the active crossfader.
	 */
	Crossfader* GetCrossfader(){ return m_Crossfader; }
	/**
	 * \param fader new desired settings (crossfade durations)
	 * \return true on success, false otherwise.
	 */
	bool SetCrossfader( const Crossfader& fader );
	/**
	 * \param enabled if true crossfader will be enabled, disabled otherwise.
	 */
	void EnableCrossfader( bool enabled = true );

	/**
	 * \param enable if true the equalizer will be enabled, disabled otherwise.
     * \param use_global if true the same equalizer will be used for every song
	 */
	void EnableEqualizer( bool enable, bool use_global );
	/*
	 * \param use_global if true, all songs will share a global equalizer. if
	 * false, the equalizers will be loaded from the database on a per song basis
	 */
    void UseGlobalEQSettings( bool use_global );

	/**
	 * \return a pointer to the internal song information object. this object
	 * should never be deleted by the developer.
	 */
	SongInfo* GetPlaying(){ return &m_CurrSong; }
	SongInfo m_CurrSong;
    
	/**
	 * Sets the internal playlist. 
	 *
	 * After calling, the Player is assumed to be the new owner of the playlist.
	 * It will, from this point on, handle the playlist's memory, including deleting
	 * it once it is no longer needed. This is done for efficient memory management,
	 * especially when dealing with large (e.g., 10,000+ song) playlists. The user can
	 * later modify the playlist directly by calling Player::GetPlaylist(),
	 * which returns a pointer to the managed playlist.
	 *
	 * If the user wishes to maintain a copy of the playlist for his own use he should 
	 * make a copy of it before calling this function OR call SetPlaylist( const Playlist& )
	 *
	 * \param playlist playlist the player will take ownership of.
	 */
	void SetPlaylist( Playlist* playlist );
	/**
	 * Sets the internal playlist.  
	 *
	 * Copies the contest of playlist to musikCore::Player's internal playlist. See also
	 * SetPlaylist( Playlist* )
	 *
	 * \param playlist
	 */
	void SetPlaylist( const Playlist& playlist );
	/**
	 * It may be useful to later switch to another library to query for
	 * crossfader, equalizer, and song information.
	 *
	 * \param library address of the new library to query.
	 */
    void SetLibrary( Library* library ){ m_Library = library; }
    Library* GetLibrary(){ return m_Library; }

	/**
	 * \return The index in the playlist of the currently playing song.
	 */
	int GetIndex(){ return m_Index; }
	/**
	 * If the developer modifies the playlist directly (e.g., by calling 
	 * Playlist::GetPlaylist()), the currently playing item index may
	 * no longer reflect its position.
	 *
	 * Example: the currently playing song is "My Song" at playlist.at(10). The
	 * user rearranges the playlist so "Some other Song" now occupies playlist.at(10), 
	 * but m_Index still reflects position 10, which is wrong.
	 
	 * FindNewIndex() attempts to resynchronize the index.
	 *
	 * \param songid 
	 * \return 
	 */
	bool FindNewIndex( int songid = -1 );

	/**
	 * Get the time right now.
	 *
	 * \param mode MUSIK_TIME_SECS or MUSIK_TIME_MSECS
	 * \return current play time.
	 */
	int	GetTimeNow( int mode );
	/**
	 * Get the time now in percent form.
	 *
	 * \return 0% - 100%
	 */
	int GetTimeNowPer();
	/**
	 * Get the time remaining. Total time - current time.
	 *
	 * \param mode MUSIK_TIME_SECS or MUSIK_TIME_MSECS
	 * \return remaining play time.
	 */
	int	GetTimeRemain( int mode );
	/**
	 * Gets the current song's total direction.
	 *
	 * \param mode MUSIK_TIME_SECS or MUSIK_TIME_MSECS
	 * \return song duration.
	 */
	int	GetDuration	( int mode );
	/**
	 * Static function to get the string form of the any time in milliseconds form.
	 * For example, Player::GetTimeStr( m_Player->GetTimeNow( MUSIK_TIME_MSECS ) )
	 * will return something like "2:24".
	 *
	 * \param time_ms time in milliseconds 
	 * \return a formatted time string 
	 */
	static String GetTimeStr( int time_ms );
	/**
	 * Set (seek) the current play time based on a percentage. 
	 * \param percent 0-100
	 */
	void SetTimeNowPer( int percent );
	/**
	 * Set the time right now.
	 *
	 * \param mode MUSIK_TIME_SECS or MUSIK_TIME_MSECS
	 * \return current play time.
	 */
	void SetTimeNow( int time, int mode );
	/**
	 * Gets the string value of time based on a percent. For example,
	 * GetTimePerSter( 25 ) on a song with 60 a second duration would return 
	 * "0:15" (60*(1/4)). This can be useful for an interface with a track progress
	 * indicator that ranges from 0-100. Practical usage would be something
	 * like Player::GetTimeNowPer( Player::GetTimeNowPer() )
	 *
	 * \param percent percent to convert to time
	 * \return string value of the time.
	 */
	String GetTimePerStr( int percent );
	/**
	 * \param percent 0-100
	 * \return millisecond value of the time at percent
	 */
	int GetTimePer ( int percent );

	/**
	 * \return The maximum allowed volume for playback. Ranges from 0-255
	 */
	int GetMaxVolume(){ return m_Volume; }
	/**
	 * Sets the maximum output volume for sound playback.
	 *
	 * \param vol volume, ranging from 0-255. 
	 * \param set_now if true the current volume for the active stream is
	 * immediately changed.
	 */
	void SetMaxVolume( int vol, bool set_now = true );

	/**
	 * Sets the volume for a specified channel
	 *
	 * \param channel_id channel to be set
	 * \param vol volume, ranging from 0-255
	 */
	void SetVolume( int channel_id, int vol );

	/**
	 * Start playback
	 *
	 * \param index song index in the playlist
	 * \param fade_type crossfader type, defaults to none. 
	 * \param start_pos stream starting position in seconds.
     * \param force force playback to start, even if paused
	 * \return true if successful, false otherwise.
	 */
	bool Play( int index = 0, int start_pos = 0, int advance_type = MUSIK_PLAYER_SONG_USER_ADVANCE, bool force = true );
	/**
     * Start cdrom playback
     * \return true if playback succeeded
     */
    bool PlayCD( int track, int start_pos = 0 );
	/**
	 * \return a pointer to the internal information
	 * used to describe the currently active audio cdrom.
	 */
	SongInfoArray* GetCDInfo(){ return &m_CDInfo; }
	/**
	 * \return currently active cd audio drive
	 */
	int GetDiscDrive(){ return m_DiscDrive; }
	/**
	 * allows the user to update the internal information about
	 * the currently playing audio cd
	 * \param info track information
	 * \param drive drive id to the track information
	 */
	void SetCDInfo( SongInfoArray info, int drive );
    /**
	 * Starts playback on the next item in the playlist.
	 *
	 * \param expired true if the stream expired naturally (e.g., called within
	 * Player()), false if the index was manually advanced.
	 * \return 
	 */
	bool Next( bool expired = false );
	/**
	 * Starts playback on the previous item in the playlist if this function
	 * is called less than 2 seconds after playback started. If the current
	 * stream has been playing for more than 2 seconds it is started over.
	 * \return true if successful, false otherwise
	 *
	 */
	bool Prev();
	/**
	 * Pause playback
	 *
	 * \return true if successful, false otherwise
	 */
	bool Pause();
	/**
	 * Resume playback from a paused state.
	 *
	 * \return true if successful, false otherwise
	 */
	bool Resume();
	/**
	 * Stop playback on all channels.
	 *
	 * \return true if successful, false otherwise
	 */
	void Stop();
	/**
	 * Triggers the exit crossfade and, upon completion, cleans any remaining
	 * sound streams. This should be called before the object is deleted.
     *
     * \param bool wait if false the function will return immediately
	 */
	void Exit( bool wait = true );
	/**
	 * Shuffles the current playlist. FindNewIndex() will be called automatically.
	 */
	void Shuffle();

	/**
	 * \return true if the soundsystem has been correctly initialized, false otherwise.
	 */
	bool IsSoundInitialized(){ return ( m_State == MUSIK_PLAYER_INIT_SUCCESS ? true : false ); }
	/**
	 * \return true if audio is playing, false otherwise
	 */
	bool IsPlaying()			{ return m_IsPlaying; }
	/**
	 * \return true if paused, false otherwise.
	 */
	bool IsPaused()				{ return m_IsPaused; }
	/**
	 * \return true if equalizer is flagged as enabled, false otherwise.
	 */
	bool IsEqualizerEnabled(){ return m_IsEQActive; };
	/**
	 * \return true if crossfader is flagged as enabled, false otherwise
	 */
	bool IsCrossfaderEnabled();

	/**
	 * \return number of active streams (playing songs)
	 */
	size_t GetStreamCount();
	/**
	 * \return a unique handle to the currently playing song. can be used
	 * by a thread as a low overhead way of checking to see if a the currently
	 * playing song has changed or not.
	 */
	int GetHandle(){ return m_Handle; }

	/**
	 * \return the current play mode. Modes can be any combination of the following:
	 * MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE,	MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST,
	 * MUSIK_PLAYER_PLAYMODE_INTRO
	 */
	unsigned long GetPlaymode(){ return m_Playmode; }
	/**
	 * Set a new default play mode.
	 *
	 * \param mode playback mode. Can be any combination of the following:
	 * MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE, MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST,
	 * MUSIK_PLAYER_PLAYMODE_INTRO
	 */
	void SetPlaymode( unsigned long mode ){ m_Playmode = mode; }
	/**
	 * Modifies the active playmode. Valid parameters include combinations of the
	 * aformentioned playlist modes.
	 *
	 * \param add modes to add.
	 * \param remove modes to remove.
	 * \param remove_first if true modes will be removed before they are added,
	 * otherwise they will be added before they are removed.
	 */
	void ModifyPlaymode( unsigned long add, unsigned long remove, bool remove_first = true );

	/**
	 * \return address of the currently managed playlist. 
	 */
	Playlist* GetPlaylist(){ return m_Playlist; }

	/**
	 * Checks if all songs currently in the playlist are still in the library.
	 * This function has to be called after songs have been deleted from the library 
	 * to enshure that they are removed from the Player's internal playlist as well.
	 */
	void UpdatePlaylist();

	/**
	 * Can be called to requery the current equalizer information from the library.
	 * This can be useful if the equalizer values were manually modified and
	 * saved back to the library already, and the user wishes them to take
	 * affect immediately.
     *
     * if refreshonly is true, the current eq is assumed to be already
     * modified, and therefor is not required from the library
     *
     * if callfunctor is false, the respective functor will not be called. this
     * can be useful in some cases, specifically when a UI calls UpdateEQSettings()
	 */
	void UpdateEQSettings( bool refreshonly = false, bool callfunctor = true );

	/**
	 * Start thread execution. Player will not work properly until
	 * this has been called manually.
	 */
	virtual void run();

	/**
	 * Current song index. This can be modified manually, but don't mess with it
	 * unless you know what are you are doing.
	 */
	int m_Index;

	/**
	 * Returns the format of the currently playing stream
	 */
	String GetFormat() { return m_Format; }

	/**
	 * Will get called automatically by a musik::Alarm object, but can
	 * be called manually. This is not recommended unless you are 
	 * implementing your own snooze / sleep function.
	 */
	void TriggerAlarm(){ m_Functor->OnAlarmTrigger(); }
    /**
     * This can be called to make a Player emit an
     * OnNewSong event.
     */
    void TriggerOnNewSong(){ m_Functor->OnNewSong(); }
    
	/**
	 * Returns the ID of the previous song
	 */
	int GetPrevSong(){ return m_PrevSong; }
    
	/** 
	 * Returns the player type (now playing, or cdrom)
	 */
	int GetPlayerType(){ return m_PlayerType; }

	/** 
	 * Informs the player that a library has been disconnected.
	 * We must remove all references to this library in the 
	 * playlist 
	 */
	void LibraryDetach( Library* detach );

	//
	// optionally override these in your derived
	// class.
	//
    static int GetFileLengthMS( const String& fn );
	virtual void GetSoundDevices( StringArray& target ){ }
	virtual void GetSoundDrivers( StringArray& target ){ }
    virtual void GetSpectrum( float* target ){ }
	virtual void SetBufferSize( int msecs ){ }

	// applications should call these
	void InitEQ();
	void DeinitEQ();
	void UpdateEQTable();
	EQSettings& GetEQSettings(){ return m_EQSettings; }
	int GetRate(){ return m_Rate; }

protected:

	// reserved for internal use.
	int m_Volume;
	int m_Handle;
	int m_State;
	int m_FadeType;
	String m_Format;
	unsigned long m_Playmode;
	int m_PrevSong;
    int m_Duration;
    int m_PlayerType;
    int m_DiscDrive;
	int m_Rate;

	// thread specific
	bool m_CrossfadeActive;
	bool m_AbortCrossfade;
	bool m_Exiting;

	// musikCore related objects
	Crossfader* m_Crossfader;
	Functor* m_Functor;
	Library* m_Library;
	Playlist* m_Playlist;
	EQSettings m_EQSettings;
    SongInfoArray m_CDInfo;

	// status flags
	bool m_IsPlaying;
	bool m_IsPaused;
	bool m_IsEQActive;
	bool m_IsCrossfaderReady;
	bool m_CrossfaderEnabled;
    bool m_UseGlobalEQSettings;
    bool m_IsCD;
	
	void StopSound();
  	int  StartSound( int device, int driver, int rate );

	// active streams and channels
	void CleanOldStreams( bool kill_primary = false );
	std::vector<Stream*> m_ActiveStreams;
	Stream* GetCurrStream();

	// equalizer and crossfader stuff
	void InitEQSettings();
	void InitCrossfader();
	void DeinitCrossfader();
	void DeinitPlaylist();
	void FlagCrossfade()		{ m_IsCrossfaderReady = true; }
	void UnflagCrossfade()		{ m_IsCrossfaderReady = false; }
	void SaveSongEq();

	// various other private functions
	void StopWait();
	void UpdateSongHistory( bool song_begin );

	// the following are reserved for the thread 
	bool IsCrossfadeActive(){ return m_CrossfadeActive; }
	void AbortCrossfade( bool wait = true );
	bool IsFinished(){ return m_Finished; }
	void FinalizePause();
	void FinalizeResume();
	void FinalizeNewSong();
	void FinalizeStop();
	void FinalizeExit();
	void FinalizeNetstream();
    

	//
	//
	// all inherited classes need to implement these.
	//
	//

	virtual int InitSoundSystem( int device, int driver, int rate ) { return 0; }     
	virtual void DeinitSoundSystem() {}
	
	virtual bool PlayStream( Stream* ptrStream, int offset ) { return false; }
    virtual Stream* LoadStream( const String& filename ) { return NULL; }
    virtual bool SetStreamVolume( Stream* ptrStream, int volume ) { return false; }
    virtual bool SetStreamPos( Stream* ptrStream, int offset ) { return false; }
    virtual int GetStreamPos( Stream* ptrStream ) { return 0; }
    virtual int GetStreamDuration( Stream* ptrStream ){ return 0; }
    virtual bool StreamPause( Stream* ptrStream ) { return false; }
    virtual bool StreamResume( Stream* ptrStream ) { return false; }
    virtual void OnDeleteStream( Stream* ptrStream ) {}
    virtual int GetStreamVolume( Stream* ptrStream ) { return 0; }
    virtual Stream* LoadNetStream(){ return 0; }
    virtual void UpdateNetstreamStatus() {}
    virtual Stream* LoadCDTrack( int track ){ return NULL; }
    virtual bool IsStreamActive( Stream* pStream ){ return false; }

	virtual void OnEQEnable(){}
	virtual void OnEQDisable(){}
	virtual void OnEQUpdate(){}

private:

    // called when the next stream has been loaded
    // and is ready to begin.
    void OnNextTrack( Stream* pNewStream, int fade_type, int start_pos ); 

    // checks for a netstream. if a netstream
    // is waiting, it will start start it up.
    void thread_CheckNetStream();

    // determines what to do if its time to
    // trigger a song event. 
    void thread_CheckNoCrossfadeStatus();
    void thread_CheckCrossfadeStatus();

    void thread_Crossfade();

    // thread variables
    int t_TimeElapsed;
    int t_TimeRemain;
    int t_TimeFade;
    bool t_Exit;

    OpenThreads::Mutex m_ThreadMutex;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
