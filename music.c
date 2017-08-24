/**
 *		@file music.c
 *		@brief This defines all functionality related loading music.
 *
 *		@author Team Drop Tables
 *
 *		@date April 26, 2017
 */

#include "open_interface.h"
#include "music.h"

/// Loads two songs onto the iRobot
/**
 * Loads two songs onto the iRobot. They must be pre-defined to operate.
 * Before calling this method, be sure to initialize the open interface by calling:
 *
 *   oi_t* sensor = oi_alloc();
 *   oi_init(sensor); 
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
void load_songs() {

	unsigned char meleeThemeNumNotes = 32;
	unsigned char meleeThemeNotes[32]     = {71, 66, 71, 78, 71, 78, 81, 80, 76, 71, 0, 71, 73, 74, 71, 74, 78, 76, 71, 66, 71, 78, 71, 78, 81, 80, 76, 71, 0, 71, 73, 74};
	unsigned char meleeThemeDurations[32] = {12, 6, 6, 36, 12, 12, 12, 12, 12, 48, 12, 6, 6, 60, 12, 12, 12, 96, 12, 6, 6, 36, 12, 12, 12, 12, 12, 48, 12, 6, 6, 96};
	oi_loadSong(MELEE_THEME, meleeThemeNumNotes, meleeThemeNotes, meleeThemeDurations);

	unsigned char gamecubeNumNotes = 27; //E   B    E  A   D   A   C#  G#  D#  A   F#  C#  G#  C#  D   G | C   F   A#   F   C   A   E   B  G#  Rest  E
	unsigned char gamecubeNotes[27]     = {64, 71, 76, 81, 86, 81, 85, 80, 75, 81, 78, 73, 68, 73, 74, 79, 84, 89, 94, 89, 84, 93, 88, 83, 92,0, 64};
	unsigned char gamecubeDurations[27] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 18, 8, 18};
	oi_loadSong(GAMECUBE_THEME, gamecubeNumNotes, gamecubeNotes, gamecubeDurations);
}
