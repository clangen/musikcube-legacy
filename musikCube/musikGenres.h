///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
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

#ifndef __C_MUSIK_GENRES_H__
#define __C_MUSIK_GENRES_H__

///////////////////////////////////////////////////

// borrowed from id3lib

///////////////////////////////////////////////////

#define ID3_NR_OF_V1_GENRES 148

///////////////////////////////////////////////////

static musikCore::String ID3_v1_genre_description[ID3_NR_OF_V1_GENRES] =
{
  _T( "Blues" ),             //0
  _T( "Classic Rock" ),      //1
  _T( "Country" ),           //2
  _T( "Dance" ),             //3
  _T( "Disco" ),             //4
  _T( "Funk" ),              //5
  _T( "Grunge" ),            //6
  _T( "Hip-Hop" ),           //7
  _T( "Jazz" ),              //8
  _T( "Metal" ),             //9
  _T( "New Age" ),           //10
  _T( "Oldies" ),            //11
  _T( "Other" ),             //12
  _T( "Pop" ),               //13
  _T( "R&B" ),               //14
  _T( "Rap" ),               //15
  _T( "Reggae" ),            //16
  _T( "Rock" ),              //17
  _T( "Techno" ),            //18
  _T( "Industrial" ),        //19
  _T( "Alternative" ),       //20
  _T( "Ska" ),               //21
  _T( "Death Metal" ),       //22
  _T( "Pranks" ),            //23
  _T( "Soundtrack" ),        //24
  _T( "Euro-Techno" ),       //25
  _T( "Ambient" ),           //26
  _T( "Trip-Hop" ),          //27
  _T( "Vocal" ),             //28
  _T( "Jazz+Funk" ),         //29
  _T( "Fusion" ),            //30
  _T( "Trance" ),            //31
  _T( "Classical" ),         //32
  _T( "Instrumental" ),      //33
  _T( "Acid" ),              //34
  _T( "House" ),             //35
  _T( "Game" ),              //36
  _T( "Sound Clip" ),        //37
  _T( "Gospel" ),            //38
  _T( "Noise" ),             //39
  _T( "AlternRock" ),        //40
  _T( "Bass" ),              //41
  _T( "Soul" ),              //42
  _T( "Punk" ),              //43
  _T( "Space" ),             //44
  _T( "Meditative" ),        //45
  _T( "Instrumental Pop" ),  //46
  _T( "Instrumental Rock" ), //47
  _T( "Ethnic" ),            //48
  _T( "Gothic" ),            //49
  _T( "Darkwave" ),          //50
  _T( "Techno-Industrial" ), //51
  _T( "Electronic" ),        //52
  _T( "Pop-Folk" ),          //53
  _T( "Eurodance" ),         //54
  _T( "Dream" ),             //55
  _T( "Southern Rock" ),     //56
  _T( "Comedy" ),            //57
  _T( "Cult" ),              //58
  _T( "Gangsta" ),           //59
  _T( "Top 40" ),            //60
  _T( "Christian Rap" ),     //61
  _T( "Pop/Funk" ),          //62
  _T( "Jungle" ),            //63
  _T( "Native American" ),   //64
  _T( "Cabaret" ),           //65
  _T( "New Wave" ),          //66
  _T( "Psychadelic" ),       //67
  _T( "Rave" ),              //68
  _T( "Showtunes" ),         //69
  _T( "Trailer" ),           //70
  _T( "Lo-Fi" ),             //71
  _T( "Tribal" ),            //72
  _T( "Acid Punk" ),         //73
  _T( "Acid Jazz" ),         //74
  _T( "Polka" ),             //75
  _T( "Retro" ),             //76
  _T( "Musical" ),           //77
  _T( "Rock & Roll" ),       //78
  _T( "Hard Rock" ),         //79
//_T(  following are winamp extentions
  _T( "Folk" ),                  //80
  _T( "Folk-Rock" ),             //81
  _T( "National Folk" ),         //82
  _T( "Swing" ),                 //83
  _T( "Fast Fusion" ),           //84
  _T( "Bebob" ),                 //85
  _T( "Latin" ),                 //86
  _T( "Revival" ),               //87
  _T( "Celtic" ),                //88
  _T( "Bluegrass" ),             //89
  _T( "Avantgarde" ),            //90
  _T( "Gothic Rock" ),           //91
  _T( "Progressive Rock" ),      //92
  _T( "Psychedelic Rock" ),      //93
  _T( "Symphonic Rock" ),        //94
  _T( "Slow Rock" ),             //95
  _T( "Big Band" ),              //96
  _T( "Chorus" ),                //97
  _T( "Easy Listening" ),        //98
  _T( "Acoustic" ),              //99
  _T( "Humour" ),                //100
  _T( "Speech" ),                //101
  _T( "Chanson" ),               //102
  _T( "Opera" ),                 //103
  _T( "Chamber Music" ),         //104
  _T( "Sonata" ),                //105
  _T( "Symphony" ),              //106
  _T( "Booty Bass" ),            //107
  _T( "Primus" ),                //108
  _T( "Porn Groove" ),           //109
  _T( "Satire" ),                //110
  _T( "Slow Jam" ),              //111
  _T( "Club" ),                  //112
  _T( "Tango" ),                 //113
  _T( "Samba" ),                 //114
  _T( "Folklore" ),              //115
  _T( "Ballad" ),                //116
  _T( "Power Ballad" ),          //117
  _T( "Rhythmic Soul" ),         //118
  _T( "Freestyle" ),             //119
  _T( "Duet" ),                  //120
  _T( "Punk Rock" ),             //121
  _T( "Drum Solo" ),             //122
  _T( "A capella" ),             //123
  _T( "Euro-House" ),            //124
  _T( "Dance Hall" ),            //125
  _T( "Goa" ),                   //126
  _T( "Drum & Bass" ),           //127
  _T( "Club-House" ),            //128
  _T( "Hardcore" ),              //129
  _T( "Terror" ),                //130
  _T( "Indie" ),                 //131
  _T( "Britpop" ),               //132
  _T( "Negerpunk" ),             //133
  _T( "Polsk Punk" ),            //134
  _T( "Beat" ),                  //135
  _T( "Christian Gangsta Rap" ), //136
  _T( "Heavy Metal" ),           //137
  _T( "Black Metal" ),           //138
  _T( "Crossover" ),             //139
  _T( "Contemporary Christian" ),//140
  _T( "Christian Rock " ),       //141
  _T( "Merengue" ),              //142
  _T( "Salsa" ),                 //143
  _T( "Trash Metal" ),           //144
  _T( "Anime" ),                 //145
  _T( "JPop" ),                  //146
  _T( "Synthpop" )               //147
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////