///////////////////////////////////////////////////
// 
// Info:
//
//	...
//
// Credits:
//
//	musikCube by Casey Langen
//	Plugin by Martin Dittus
//
///////////////////////////////////////////////////

#ifndef __C_MUSIKPLUGINPROPS_H__
#define __C_MUSIKPLUGINPROPS_H__


///////////////////////////////////////////////////

// plugin info

// name of plugin
#ifdef _DEBUG
#define STR_PLUGIN_NAME _T( "miniPlayer (Debug build)" )
#else
#define STR_PLUGIN_NAME _T( "miniPlayer" )
#endif

// plugin version
#define STR_PLUGIN_VERSION _T( "0.1.7" )


///////////////////////////////////////////////////

// etc

#define STR_ABOUT_CAPTION _T( "About " ) STR_PLUGIN_NAME
#define STR_CONFIG_CAPTION _T( "Configure " ) STR_PLUGIN_NAME
#define STR_MAIN_CAPTION STR_PLUGIN_NAME

// website url
#define STR_PLUGIN_WEBSITE _T( "http://dekstop.de/musikcube/" )


#endif
