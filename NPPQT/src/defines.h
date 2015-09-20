#ifndef DEFINES_H
#define DEFINES_H

#include <QtGlobal>


/* File: defines.h */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *                    Jeff Greene, Diego Gonzalez
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 3, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */


/*
 * Do not edit this file unless you know *exactly* what you are doing.
 *
 * Some of the values in this file were chosen to preserve game balance,
 * while others are hard-coded based on the format of old save-files, the
 * definition of arrays in various places, mathematical properties, fast
 * computation, storage limits, or the format of external text files.
 *
 * Changing some of these values will induce crashes or memory errors or
 * savefile mis-reads.  Most of the comments in this file are meant as
 * reminders, not complete descriptions, and even a complete knowledge
 * of the source may not be sufficient to fully understand the effects
 * of changing certain definitions.
 *
 * Lastly, note that the code does not always use the symbolic constants
 * below, and sometimes uses various hard-coded values that may not even
 * be defined in this file, but which may be related to definitions here.
 * This is of course bad programming practice, but nobody is perfect.
 *
 *
 * You have been warned.
 */


/*
 * Name of the version/variant
 */
#define VERSION_NAME "NPPAngband"

#define VERSION_MODE_NAME (game_mode == GAME_NPPMORIA ? "NPPMoria" : "NPPAngband")

/*
 * Current version string
 */
#define VERSION_STRING	"8.0.0"


/*
 * Current version numbers
 */
#define VERSION_MAJOR	7
#define VERSION_MINOR	1
#define VERSION_PATCH	0
#define VERSION_EXTRA	0


/*
 * Oldest version number that can still be imported
 */
#define OLD_VERSION_MAJOR	7
#define OLD_VERSION_MINOR	1
#define OLD_VERSION_PATCH	0

/*
 * OPTION: Person to contact if something goes wrong.
 */
#define MAINTAINER	"nppangbanddev@verizon.net"


// Keeping track of which game we are playing.
enum
{
    GAME_MODE_UNDEFINED,
    GAME_NPPANGBAND,
    GAME_NPPMORIA
};

/*
 * Number of grids in each block (vertically)
 * Probably hard-coded to 11, see "generate.c"
 */
#define BLOCK_HGT	11

/*
 * Number of grids in each block (horizontally)
 * Probably hard-coded to 11, see "generate.c"
 */
#define BLOCK_WID	11

/*
 * Number of grids in each panel (vertically)
 * Must be a multiple of BLOCK_HGT
 */
#define PANEL_HGT	11

/*
 * Number of grids in each panel (horizontally)
 * Must be a multiple of BLOCK_WID
 */
#define PANEL_WID	33


//In some of the QT files, this is defined.  In some it isn't.
#ifndef     TRUE
#define     TRUE    true
#endif // TRUE
#ifndef     FALSE
#define     FALSE   false
#endif // FALSE



#define SIDEBAR_MONSTER_MAX	15

/*
 * There is a 1/160 chance per round of creating a new monster
 */
#define MAX_M_ALLOC_CHANCE	160

/*
 * Normal levels get at least 14 monsters
 */
#define MIN_M_ALLOC_LEVEL	14

/*
 * The town starts out with 4 residents during the day
 */
#define MIN_M_ALLOC_TD		4

/*
 * The town starts out with 8 residents during the night
 */
#define MIN_M_ALLOC_TN		8

/*
 * Number of grids in each dungeon (horizontally)
 * Must be a multiple of PANEL_HGT and an even number
 * Must be less or equal to 256
 */
#define MAX_DUNGEON_HGT		66

/*
 * Number of grids in each dungeon (vertically)
 * Must be a multiple of PANEL_WID and an even number
 * Must be less or equal to 256
 */
#define MAX_DUNGEON_WID		198

/*
 * Total number of grids in the dungeon
 */
#define MAX_DUNGEON_AREA	(MAX_DUNGEON_HGT * MAX_DUNGEON_WID)

/*
 * Maximum dungeon level.  The player can never reach this level
 * in the dungeon, and this value is used for various calculations
 * involving object and monster creation.  In Angband, it must be at least 100.
 * In Moria, it must be above 50
 * Setting it below 128 may prevent the creation of some objects.
 */
#define MAX_DEPTH_ALL	128
#define MAX_DEPTH  		((game_mode == GAME_NPPMORIA) ? 64: 128)
#define MORIA_QUEST_DEPTH	50


#define PY_MAX_LEVEL_MORIA 40

/*
 * Maximum size of the "view" array (see "cave.c")
 * Note that the "view radius" will NEVER exceed 20, and even if the "view"
 * was octagonal, we would never require more than 1520 entries in the array.
 */
#define VIEW_MAX 1536

/*
 * Maximum size of the "temp" array (see "cave.c")
 * Note that we must be as large as "VIEW_MAX" for proper functioning
 * of the "update_view()" function, and we must also be as large as the
 * largest illuminatable room, but no room is larger than 800 grids.  We
 * must also be large enough to allow "good enough" use as a circular queue,
 * to calculate monster flow, but note that the flow code is "paranoid".
 */
#define TEMP_MAX 1536

/*
 * Maximum number of player "sex" types (see "table.c", etc)
 */
#define MAX_SEXES		3

/*
 * Player sex constants (hard-coded by save-files, arrays, etc)
 */
#define SEX_MALE		0
#define SEX_FEMALE		1
#define SEX_NEUTRAL     2


// Ensure the variable sizes are consistent across all platforms.
typedef quint8          byte;
typedef quint16         u16b;
typedef qint16          s16b;
typedef quint32         u32b;
typedef qint32          s32b;

#define MAX_BYTE    255
#define MIN_BYTE    0

#define MAX_S16B    32767
#define MIN_S16B    -32768

#define MAX_U16B    0
#define MIN_U16B    65535

#define MAX_S32B    -2147483648
#define MIN_S32B    2147483647

#define MAX_U32B    0
#define MIN_U32B    4294967295

/*
 * An array of 256 byte's
 */
typedef byte byte_256[256];

/*
 * An array of 256 u16b's
 */
typedef u16b u16b_256[256];

/*
 * An array of MAX_DUNGEON_WID byte's
 */
typedef byte byte_wid[MAX_DUNGEON_WID];

/*
 * An array of MAX_DUNGEON_WID s16b's
 */
typedef s16b s16b_wid[MAX_DUNGEON_WID];

/*
 * A matrix of MAX_DUNGEON_HGT * MAX_DUNGEON_WID u16b's
 */
typedef u16b u16b_dungeon[MAX_DUNGEON_HGT][MAX_DUNGEON_WID];


/*
 * Maximum size of the "dyna_g" array.
 */
#define DYNA_MAX 1536

/* Flags for the "dynamic_grid_type" data type */
#define DF1_OCCUPIED 0x01	/* The entry is being used */
#define DF1_NEW_BORN 0x02	/* The dynamic feature was created recently */


#define ENERGY_TO_MOVE		200
#define BASE_ENERGY_MOVE	100
#define BASE_ENERGY_FLYING	BASE_ENERGY_MOVE

/*
 * Minimum noise you can make just walking around, given
 * perfect stealth.  Maximum noise is this *100
 */
#define WAKEUP_MIN      20
#define WAKEUP_MAX      (WAKEUP_MIN * 100)

#define SEARCH_CHANCE_MAX   50


/*
 * More maximum values
 */
#define MAX_SIGHT	20	/* Maximum view distance */
#define MAX_RANGE	20	/* Maximum range (spells, etc) */




/*
 * Misc constants
 */
#define TOWN_DAWN		10000	/* Number of turns from dawn to dawn XXX */
#define BREAK_GLYPH		400		/* Rune of protection resistance */
#define BTH_PLUS_ADJ	3		/* Adjust BTH per plus-to-hit */
#define MON_MULT_ADJ	8		/* High value slows multiplication */
#define QUEST_TURNS		1200	/* Number of turns between quest failure checks */
#define MON_DRAIN_LIFE	2		/* Percent of player exp drained per hit */
#define USE_DEVICE		3		/* x> Harder devices x< Easier devices */
#define INTEREST_OFFSET	10		/* Amount of levels OOD before a history is interesting */



/*
 * Spell types used by project(), and related functions.
 */
#define GF_XXX1			1
#define GF_ARROW		2
#define GF_MISSILE		3
#define GF_MANA			4
#define GF_HOLY_ORB		5
#define GF_LIGHT_WEAK	6
#define GF_DARK_WEAK	7
#define GF_WATER		8
#define GF_PLASMA		9
#define GF_METEOR		10
#define GF_ICE			11
#define GF_GRAVITY		12
#define GF_INERTIA_NPP		13
#define GF_FORCE		14
#define GF_TIME			15
#define GF_ACID			16
#define GF_ELEC			17
#define GF_FIRE			18
#define GF_COLD			19
#define GF_POIS			20
#define GF_STATIC		21
#define GF_LIGHT		22
#define GF_DARK			23
#define GF_EXTINGUISH	24
#define GF_CONFUSION	25
#define GF_SOUND		26
#define GF_SHARD		27
#define GF_NEXUS		28
#define GF_NETHER		29
#define GF_CHAOS		30
#define GF_DISENCHANT	31
#define GF_CLEAR_AIR	32
#define GF_KILL_WALL	33
#define GF_KILL_DOOR	34
#define GF_KILL_TRAP	35
#define GF_MAKE_WALL	36
#define GF_MAKE_DOOR	37
#define GF_MAKE_TRAP	38
#define GF_STERILIZE	39
#define GF_SAND			40
#define GF_AWAY_UNDEAD	41
#define GF_AWAY_EVIL	42
#define GF_AWAY_ALL		43
#define GF_TURN_UNDEAD	44
#define GF_TURN_EVIL	45
#define GF_TURN_ALL		46
#define GF_DISP_UNDEAD	47
#define GF_DISP_EVIL	48
#define GF_DISP_ALL		49
#define GF_MAKE_WARY	50

#define GF_OLD_CLONE	51
#define GF_OLD_POLY		52
#define GF_OLD_HEAL		53
#define GF_OLD_SPEED	54
#define GF_OLD_SLOW		55
#define GF_OLD_CONF		56
#define GF_OLD_SLEEP	57
#define GF_LIFE_DRAIN	58
#define GF_SPORE		59
#define GF_MASS_IDENTIFY 60
#define GF_BRIDGE		61
#define GF_LAVA			62
#define GF_BWATER		63
#define GF_BMUD			64
#define GF_LOCK_DOOR	92

#define GF_FEATURE		95
#define GF_VAPOUR		96
#define GF_STEAM		97
#define GF_SMOKE		98
#define GF_FOG			99
#define GF_ELEC_BURST  100

/*
 * Some constants for the "learn" code.  These generalized from the
 * old DRS constants.
 */
#define LRN_FREE_SAVE	14
#define LRN_MANA		15
#define LRN_ACID		16
#define LRN_ELEC		17
#define LRN_FIRE		18
#define LRN_COLD		19
#define LRN_POIS		20
#define LRN_FEAR_SAVE	21
#define LRN_LIGHT		22
#define LRN_DARK		23
#define LRN_BLIND		24
#define LRN_CONFU		25
#define LRN_SOUND		26
#define LRN_SHARD		27
#define LRN_NEXUS		28
#define LRN_NETHR		29
#define LRN_CHAOS		30
#define LRN_DISEN		31
#define LRN_SAVE		39
#define LRN_ARCH		40
#define LRN_PARCH		41
#define LRN_ICE			42
#define LRN_PLAS		43
#define LRN_SOUND2		44 /* attacks which aren't resisted, but res sound prevent stun */
#define LRN_STORM		45
#define LRN_WATER		46
#define LRN_NEXUS_SAVE	47 /* Both resist Nexus and Saves apply */
#define LRN_BLIND_SAVE	48 /* Both resist Blind and Saves apply */
#define LRN_CONFU_SAVE	49 /* Both resist Confusion and Saves apply */
#define LRN_DARK_SAVE	50
#define LRN_HOLY_SAVE	51
#define LRN_LAVA  		62

#define LRN_MAX			63



/*** Function flags ***/

#define PROJECT_NO			0
#define PROJECT_NOT_CLEAR	1
#define PROJECT_CLEAR		2


/*
 * Bit flags for the "project()", the "projectable()", and/or the
 * "project_path()" functions.
 *
 *   BEAM:  Work as a beam weapon (affect every grid passed through)
 *   ARC:   Act as an arc spell (a portion of a caster-centered ball)
 *   STAR:  Act as a starburst - a randomized ball
 *   BOOM:  Explode in some way
 *   WALL:  Affect one layer of any wall, even if not passable
 *   PASS:  Ignore walls entirely
 *   GRID:  Affect each grid in the "blast area" in some way
 *   ITEM:  Affect each object in the "blast area" in some way
 *   KILL:  Affect each monster in the "blast area" in some way
 *   PLAY:  Explicitly affect the player
 *   EFFECT Set Effects based on this projection
 *   HIDE:  Hack -- disable graphical effects of projection
 *   STOP:  Stop as soon as we hit a monster
 *   JUMP:  Jump directly to the target location
 *   THRU:  Continue "through" the target (used for projectiles)
 *   CHCK:  Note occupied grids, but do not stop at them
 *   ORTH:  Projection cannot move diagonally (used for wall spells)
 */

/* Projection types */
#define PROJECT_NONE		0x00000000
#define PROJECT_BEAM		0x00000001
#define PROJECT_ARC			0x00000002
#define PROJECT_STAR		0x00000004
#define PROJECT_ROCK		0x00000008 /* A boulder is being thrown, use rock graphic (affects visuals only) */
#define PROJECT_SHOT		0x00000010 /* A rock is being thrown/fired, use shot graphic (affects visuals only) */
#define PROJECT_AMMO		0x00000020 /* A bolt/arrow is being thrown/fired, use shot graphic (affects visuals only) */

/* What projections do */
#define PROJECT_BOOM		0x00000040
#define PROJECT_WALL		0x00000080
#define PROJECT_PASS		0x00000100  /*Ignore walls*/
#define PROJECT_ROOM		0x00000200  /* Create a room as you kill_wall */
#define PROJECT_SAME		0x00000400  /* Don't damage similar monsters */

/* What projections affect */
#define PROJECT_GRID		0x00000800
#define PROJECT_ITEM		0x00001000
#define PROJECT_KILL		0x00002000 /* Hurt the monsters*/
#define PROJECT_PLAY		0x00004000 /* Hurt the player*/
#define PROJECT_EFCT		0x00008000 /* Use Effects*/
#define PROJECT_CLOUD		0x00010000 /* Always set the effect regardless of damage*/
#define PROJECT_NO_EFCT		0x00020000 /* Cancel effects */

/* Graphics */
#define PROJECT_HIDE		0x00040000
#define PROJECT_NO_REDRAW	0x00080000
#define PROJECT_XXX9		0x00100000

/* How projections travel */
#define PROJECT_STOP		0x00200000
#define PROJECT_JUMP		0x00400000
#define PROJECT_THRU		0x00800000
#define PROJECT_CHCK		0x01000000
#define PROJECT_ORTH		0x02000000 /*(unused)*/
#define PROJECT_XX10		0x04000000

/* Projection blockage indicators */
#define PATH_G_FULL			0
#define PATH_G_BLCK			1
#define PATH_G_WALL			2
#define PATH_G_NONE			100

#define PATH_SIZE			512

/*Who caused the projection? */
#define SOURCE_PLAYER			-1	/*player is the source of projection*/
#define SOURCE_TRAP				-2	/*Trap*/
#define SOURCE_EFFECT			-3	/*Effect*/
#define SOURCE_OTHER			-4	/*Terrain, something other than player or monster*/
#define SOURCE_MONSTER_START	 0	/*Greater than 0 monster is the source*/


/*Mode whether to describe traps/spells or set off/cast them*/
#define MODE_DESCRIBE	1
#define MODE_ACTION		2
#define MODE_FLAGS		3


/*
 * An arc with a width (in degrees) less than this value will lose less
 * power over distance.
 */
#define ARC_STANDARD_WIDTH     90


/*
 * Bit flags for the "target_set" function
 *
 *	KILL: Target monsters
 *	LOOK: Describe grid fully
 *	XTRA: Currently unused flag
 *	GRID: Select from all grids
 *	PROBE: - Terrain can be targeted as well as a monster (for probing)
 */
#define TARGET_KILL		0x01
#define TARGET_LOOK		0x02
#define TARGET_XTRA		0x04
#define TARGET_GRID		0x08
#define TARGET_QUIET	0x08
#define TARGET_TRAP		0x20
#define TARGET_PROBE	0x40


#define LEV_THEME_HEAD				11
#define LEV_THEME_CREEPING_COIN		0	/*creeping coins*/
#define LEV_THEME_ORC				1	/*orc*/
#define LEV_THEME_TROLL				2	/*troll*/
#define LEV_THEME_OGRE				3	/*ogre*/
#define LEV_THEME_HOUND				4	/*hound*/
#define LEV_THEME_GIANT				5	/*giant*/
#define LEV_THEME_DRAGON_YOUNG		6	/*young dragon*/
#define LEV_THEME_DRAGON_ACID		7	/*acid dragon*/
#define LEV_THEME_DRAGON_FIRE		8	/*fire dragon*/
#define LEV_THEME_DRAGON_ELEC		9	/*electric dragon*/
#define LEV_THEME_DRAGON_COLD		10	/*cold dragon*/
#define LEV_THEME_DRAGON_POIS		11	/*poison dragon*/
#define LEV_THEME_DRAGON_CHROMATIC	12	/*chromatic dragon*/
#define LEV_THEME_DRAGON_MISC		13	/*dragon*/
#define LEV_THEME_DRAGON_ANCIENT	14	/*ancient dragon*/
#define LEV_THEME_JELLY				15	/*jelly*/
#define LEV_THEME_ORC_NAGA_YEEK_KOBOLD	16 /*kobold, yeek, orc, and naga*/
#define LEV_THEME_ANIMAL			17	/*animal*/
#define LEV_THEME_HUMANOID			18	/*humanoid*/
#define LEV_THEME_DEMON_MINOR		19	/*minor demon*/
#define LEV_THEME_DEMON_ALL			20	/*demon*/
#define LEV_THEME_DEMON_MAJOR		21	/*major demon*/
#define LEV_THEME_CAVE_DWELLER		22	/*cave dweller*/
#define LEV_THEME_UNDEAD			23	/*undead*/
#define LEV_THEME_DRAGON_ELEMENTAL	24	/*Elemental Dragons*/
#define LEV_THEME_VALAR_SERVANTS	25	/* Servants of the Valar*/
#define LEV_THEME_TAIL				26

#define PIT_LEVEL_BOOST				6
#define NEST_LEVEL_BOOST			5
#define PIT_NEST_QUEST_BOOST		8

#define THEMED_LEVEL_NO_QUEST_BOOST	7
#define THEMED_LEVEL_QUEST_BOOST	10



/*** Option Definitions ***/
/*
 * Option indexes (normal)
 */
enum
{
    OPT_carry_query_flag = 0,
    OPT_use_old_target,
    OPT_always_pickup,
    OPT_floor_query_flag,
    OPT_stack_force_notes,
    OPT_stack_force_costs,
    OPT_show_flavors,
    OPT_disturb_move,
    OPT_disturb_near,
    OPT_disturb_state,
    OPT_disturb_detect,
    OPT_verify_destroy,
    OPT_verify_leave_quest,
    OPT_UNUSED_1,
    OPT_UNUSED_2,
    OPT_UNUSED_3,
    OPT_auto_scum,
    OPT_allow_themed_levels,
    OPT_view_perma_grids,
    OPT_view_torch_grids,
    OPT_view_yellow_light,
    OPT_view_bright_light,
    OPT_view_granite_light,
    OPT_view_special_light,
    OPT_UNUSED_4,
    OPT_UNUSED_5,
    OPT_UNUSED_6,
    OPT_smart_cheat,
    OPT_UNUSED_6x1,
    OPT_UNUSED_6x2,
    OPT_hilight_player,
    OPT_easy_open,
    OPT_easy_alter,
    OPT_show_piles,
    OPT_center_player,
    OPT_animate_flicker,
    OPT_hp_changes_color,
    OPT_mark_squelch_items,
    OPT_UNUSED_6x3,
    OPT_UNUSED_6x4,
    OPT_UNUSED_7,
    OPT_UNUSED_8,
    OPT_UNUSED_9,
    OPT_UNUSED_10,
    OPT_UNUSED_11,
    OPT_UNUSED_12,
    OPT_birth_no_selling,
    OPT_birth_maximize,
    OPT_birth_preserve,
    OPT_birth_ironman,
    OPT_birth_no_stores,
    OPT_birth_no_artifacts,
    OPT_birth_rand_artifacts,
    OPT_birth_no_stacking,
    OPT_birth_force_small_lev,
    OPT_birth_connected_stairs,
    OPT_birth_no_quests,
    OPT_birth_no_player_ghosts,
    OPT_birth_no_store_services,
    OPT_birth_no_xtra_artifacts,
    OPT_birth_money,
    OPT_birth_classic_dungeons,
    OPT_birth_swap_weapons,
    OPT_birth_point_based,
    OPT_UNUSED_14,
    OPT_UNUSED_15,
    OPT_UNUSED_16,
    OPT_UNUSED_18,
    OPT_UNUSED_19,
    OPT_UNUSED_20,
    OPT_UNUSED_21,
    OPT_UNUSED_22,
    OPT_UNUSED_23,
    OPT_UNUSED_24,
    OPT_UNUSED_25,
    OPT_cheat_peek,
    OPT_cheat_hear,
    OPT_cheat_room,
    OPT_cheat_xtra,
    OPT_cheat_know,
    OPT_cheat_live,
    OPT_UNUSED_26,
    OPT_UNUSED_27,
    OPT_UNUSED_28,
    OPT_MAX,
};


#define OPT_BIRTH_HEAD  OPT_birth_no_selling
#define OPT_BIRTH_TAIL  OPT_UNUSED_19
#define OPT_CHEAT_HEAD  OPT_cheat_peek
#define OPT_CHEAT_TAIL  OPT_UNUSED_28
#define OPT_NONE        (OPT_MAX + 1)

/*
 * Hack -- Option symbols
 */
#define carry_query_flag		op_ptr->opt[OPT_carry_query_flag]
#define use_old_target			op_ptr->opt[OPT_use_old_target]
#define always_pickup			op_ptr->opt[OPT_always_pickup]
#define floor_query_flag		op_ptr->opt[OPT_floor_query_flag]
#define stack_force_notes		op_ptr->opt[OPT_stack_force_notes]
#define stack_force_costs		op_ptr->opt[OPT_stack_force_costs]
#define show_flavors			op_ptr->opt[OPT_show_flavors]
#define disturb_move			op_ptr->opt[OPT_disturb_move]
#define disturb_near			op_ptr->opt[OPT_disturb_near]
#define disturb_state			op_ptr->opt[OPT_disturb_state]
#define disturb_detect			op_ptr->opt[OPT_disturb_detect]
#define verify_destroy			op_ptr->opt[OPT_verify_destroy]
#define verify_leave_quest		op_ptr->opt[OPT_verify_leave_quest]
/* xxx */
/* xxx */
/* xxx */
#define auto_scum				op_ptr->opt[OPT_auto_scum]
#define allow_themed_levels		op_ptr->opt[OPT_allow_themed_levels]
#define view_perma_grids		op_ptr->opt[OPT_view_perma_grids]
#define view_torch_grids		op_ptr->opt[OPT_view_torch_grids]
#define view_yellow_light		op_ptr->opt[OPT_view_yellow_light]
#define view_bright_light		op_ptr->opt[OPT_view_bright_light]
#define view_granite_light		op_ptr->opt[OPT_view_granite_light]
#define view_special_light		op_ptr->opt[OPT_view_special_light]
/* xxx */
/* xxx */
/* xxx */
#define smart_cheat				op_ptr->opt[OPT_smart_cheat]
/* xxx */
/* xxx */
#define hilight_player			op_ptr->opt[OPT_hilight_player]
#define easy_open				op_ptr->opt[OPT_easy_open]
#define easy_alter				op_ptr->opt[OPT_easy_alter]
#define show_piles				op_ptr->opt[OPT_show_piles]
#define center_player			op_ptr->opt[OPT_center_player]
#define animate_flicker 		op_ptr->opt[OPT_animate_flicker]
#define hp_changes_color		op_ptr->opt[OPT_hp_changes_color]
#define mark_squelch_items		op_ptr->opt[OPT_mark_squelch_items]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define birth_no_selling		op_ptr->opt[OPT_birth_no_selling]
#define birth_maximize			op_ptr->opt[OPT_birth_maximize]
#define birth_preserve			op_ptr->opt[OPT_birth_preserve]
#define birth_ironman			op_ptr->opt[OPT_birth_ironman]
#define birth_no_stores			op_ptr->opt[OPT_birth_no_stores]
#define birth_no_artifacts		op_ptr->opt[OPT_birth_no_artifacts]
#define birth_rand_artifacts	op_ptr->opt[OPT_birth_rand_artifacts]
#define birth_no_stacking		op_ptr->opt[OPT_birth_no_stacking]
#define	birth_force_small_lev	op_ptr->opt[OPT_birth_force_small_lev]
#define	birth_connected_stairs	op_ptr->opt[OPT_birth_connected_stairs]
#define birth_no_quests			op_ptr->opt[OPT_birth_no_quests]
#define birth_no_player_ghosts	op_ptr->opt[OPT_birth_no_player_ghosts]
#define birth_no_store_services	op_ptr->opt[OPT_birth_no_store_services]
#define birth_no_xtra_artifacts	op_ptr->opt[OPT_birth_no_xtra_artifacts]
#define birth_money				op_ptr->opt[OPT_birth_money]
#define birth_classic_dungeons	op_ptr->opt[OPT_birth_classic_dungeons]
#define birth_swap_weapons		op_ptr->opt[OPT_birth_swap_weapons]
#define birth_point_based       op_ptr->opt[OPT_birth_point_based]
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
/* xxx */
#define cheat_peek				op_ptr->opt[OPT_cheat_peek]
#define cheat_hear				op_ptr->opt[OPT_cheat_hear]
#define cheat_room				op_ptr->opt[OPT_cheat_room]
#define cheat_xtra				op_ptr->opt[OPT_cheat_xtra]
#define cheat_know				op_ptr->opt[OPT_cheat_know]
#define cheat_live				op_ptr->opt[OPT_cheat_live]
/* xxx xxx */
/* xxx xxx */
/* xxx xxx */


/*
 * Convert a "location" (Y,X) into a "grid" (G)
 */
#define GRID(Y,X) \
    (256 * (Y) + (X))

/*
 * Convert a "grid" (G) into a "location" (Y)
 */
#define GRID_Y(G) \
    ((int)((G) / 256U))

/*
 * Convert a "grid" (G) into a "location" (X)
 */
#define GRID_X(G) \
    ((int)((G) % 256U))


/*
 * Determines if a map location is "meaningful"
 */
#define in_bounds(Y,X) \
    (((unsigned)(Y) < (unsigned)(p_ptr->cur_map_hgt)) && \
     ((unsigned)(X) < (unsigned)(p_ptr->cur_map_wid)))

/*
 * Determines if a map location is fully inside the outer walls
 * This is more than twice as expensive as "in_bounds()", but
 * often we need to exclude the outer walls from calculations.
 */
#define in_bounds_fully(Y,X) \
    (((Y) > 0) && ((Y) < p_ptr->cur_map_hgt-1) && \
     ((X) > 0) && ((X) < p_ptr->cur_map_wid-1))


/*
 * Determine if a "legal" grid is within "los" of the player
 *
 * Note the use of comparison to zero to force a "boolean" result
 */
#define player_has_los_bold(Y,X) \
    ((dungeon_info[Y][X].cave_info & (CAVE_VIEW)) != 0)

/*
 * Determine if the player has a clear enough head to observe things
 */

#define player_can_observe() \
    ((!p_ptr->timed[TMD_BLIND]) && (!p_ptr->timed[TMD_CONFUSED]) && (!p_ptr->timed[TMD_IMAGE]) && (!p_ptr->timed[TMD_PARALYZED]))


/*
 * Determine if a "legal" grid can be "seen" by the player
 *
 * Note the use of comparison to zero to force a "boolean" result
 */
#define player_can_see_bold(Y,X) \
    ((dungeon_info[Y][X].cave_info & (CAVE_SEEN)) != 0)

/*
 * Determine if a "legal" grid is within "line of fire" of the player
 *
 * Note the use of comparison to zero to force a "boolean" result
 */
#define player_can_fire_bold(Y,X) \
    ((dungeon_info[Y][X].cave_info & (CAVE_FIRE)) != 0)

/*
 * Available graphic modes
 */
enum
{
    GRAPHICS_NONE = 0,
    GRAPHICS_ORIGINAL,
    GRAPHICS_DAVID_GERVAIS,
    GRAPHICS_RAYMOND_GAUSTADNES,
    GRAPHICS_PSEUDO,
};

/*
 * Available graphic modes
 */
enum
{
    KEYSET_NEW = 0,
    KEYSET_ANGBAND,
    KEYSET_ROGUE,
};

/*
 * Information for "do_cmd_options()".
 */
enum
{
    OPT_PAGE_INTERFACE = 0,
    OPT_PAGE_DISPLAY,
    OPT_PAGE_WARNING,
    OPT_PAGE_BIRTH,
    OPT_PAGE_CHEAT,
    OPT_PAGE_MAX,
};

#define OPT_PAGE_PER			20


#define DUNGEON_TYPE_DEFAULT		1	/* Vanilla-like levels */
#define DUNGEON_TYPE_TOWN			2
#define DUNGEON_TYPE_THEMED_LEVEL	3
#define DUNGEON_TYPE_WILDERNESS		4
#define DUNGEON_TYPE_ARENA			5
#define DUNGEON_TYPE_LABYRINTH		6
#define DUNGEON_TYPE_GREATER_VAULT	7

/** Constants for the various patterns of pits */
#define MAX_PIT_PATTERNS	3
#define PIT_WIDTH			19
#define PIT_HEIGHT			5




//Needs to correspond with the colors table list in tables.c
enum
{
    // darkest colors
    TERM_DARK,
    TERM_L_DARK,
    TERM_SLATE,
    TERM_SLATE_GRAY,
    TERM_TAUPE,

    // White
    TERM_WHITE,
    TERM_LIGHT_GRAY,
    TERM_SNOW_WHITE,
    TERM_IVORY,

    //Red
    TERM_RED,
    TERM_L_RED,
    TERM_RED_LAVA,
    TERM_RASPBERRY,
    TERM_RED_RUST,
    TERM_PINK,

    //Orange
    TERM_ORANGE,
    TERM_ORANGE_PEEL,
    TERM_MAHAGONY,

    //Green
    TERM_GREEN,
    TERM_L_GREEN,
    TERM_JUNGLE_GREEN,
    TERM_LIME_GREEN,

    //Blue
    TERM_BLUE,
    TERM_L_BLUE,
    TERM_NAVY_BLUE,
    TERM_SKY_BLUE,

    //Brown
    TERM_UMBER,
    TERM_L_UMBER,
    TERM_AUBURN,
    TERM_L_BROWN,

    //Yellow
    TERM_YELLOW,
    TERM_MAIZE,
    TERM_EARTH_YELLOW,

    // Purple
    TERM_VIOLET,
    TERM_PURPLE,

    // Other
    TERM_GOLD,
    TERM_SILVER,
    TERM_COPPER,
    TERM_BRONZE,

    MAX_COLORS,
    COLOR_CUSTOM  //
};


// Same colors in tables.c, usable in HTML format
#define    COLOR24BIT_DARK          #000000
#define    COLOR24BIT_L_DARK        #606060
#define    COLOR24BIT_SLATE         #808080
#define    COLOR24BIT_SLATE_GRAY    #708090
#define    COLOR24BIT_TAUPE         #8B8589
#define    COLOR24BIT_WHITE         #FFFFFF
#define    COLOR24BIT_LIGHT_GRAY    #C0C0C0
#define    COLOR24BIT_SNOW_WHITE    #FFFAFA
#define    COLOR24BIT_IVORY         #FFFFF0
#define    COLOR24BIT_RED           #C00000
#define    COLOR24BIT_L_RED         #FF4040
#define    COLOR24BIT_RED_LAVA      #CF1020
#define    COLOR24BIT_RASPBERRY     #E30B5C
#define    COLOR24BIT_RED_RUST      #B7410E
#define    COLOR24BIT_PINK          #FF1493
#define    COLOR24BIT_ORANGE        #FF8000
#define    COLOR24BIT_ORANGE_PEEL   #FF9F00
#define    COLOR24BIT_MAHAGONY      #C04000
#define    COLOR24BIT_GREEN         #008040
#define    COLOR24BIT_L_GREEN       #00FF00
#define    COLOR24BIT_JUNGLE_GREEN  #29AB87
#define    COLOR24BIT_LIME_GREEN    #BFFF00
#define    COLOR24BIT_BLUE          #0040FF
#define    COLOR24BIT_L_BLUE        #00FFFF
#define    COLOR24BIT_NAVY_BLUE     #4C4CA6
#define    COLOR24BIT_SKY_BLUE      #00BFFF
#define    COLOR24BIT_UMBER         #804000
#define    COLOR24BIT_L_UMBER       #C08040
#define    COLOR24BIT_AUBURN        #6D351A
#define    COLOR24BIT_L_BROWN       #C19A6B
#define    COLOR24BIT_YELLOW        #FFFF00
#define    COLOR24BIT_EARTH_YELLOW  #E1A95F
#define    COLOR24BIT_MAIZE         #FBEC5D
#define    COLOR24BIT_VIOLET        #FF00FF
#define    COLOR24BIT_PURPLE        #A500FF
#define    COLOR24BIT_GOLD          #FFD700
#define    COLOR24BIT_SILVER        #C0C0C0
#define    COLOR24BIT_COPPER        #B87333
#define    COLOR24BIT_BRONZE        #CD7F32






#define CHECK_DISTURB(stop_search) \
(p_ptr->command_current || ((stop_search) && p_ptr->searching))


/*
 * Minimum values for panel change offsets
 */
#define PANEL_CHANGE_OFFSET_Y 2
#define PANEL_CHANGE_OFFSET_X 4

enum
{
    MSG_GENERIC       = 0,
    MSG_HIT           = 1,
    MSG_MISS          = 2,
    MSG_FLEE          = 3,
    MSG_DROP          = 4,
    MSG_KILL          = 5,
    MSG_LEVEL         = 6,
    MSG_DEATH         = 7,
    MSG_STUDY         = 8,
    MSG_TELEPORT      = 9,
    MSG_SHOOT         = 10,
    MSG_QUAFF         = 11,
    MSG_ZAP_ROD       = 12,
    MSG_WALK          = 13,
    MSG_TPOTHER       = 14,
    MSG_HITWALL       = 15,
    MSG_EAT           = 16,
    MSG_STORE1        = 17,
    MSG_STORE2        = 18,
    MSG_STORE3        = 19,
    MSG_STORE4        = 20,
    MSG_DIG           = 21,
    MSG_OPENDOOR      = 22,
    MSG_SHUTDOOR      = 23,
    MSG_TPLEVEL       = 24,
    MSG_BELL          = 25,
    MSG_NOTHING_TO_OPEN = 26,
    MSG_LOCKPICK_FAIL = 27,
    MSG_STAIRS_DOWN   = 28,
    MSG_HITPOINT_WARN = 29,
    MSG_ACT_ARTIFACT  = 30,
    MSG_USE_STAFF     = 31,
    MSG_DESTROY       = 32,
    MSG_MON_HIT       = 33,
    MSG_MON_TOUCH     = 34,
    MSG_MON_PUNCH     = 35,
    MSG_MON_KICK      = 36,
    MSG_MON_CLAW      = 37,
    MSG_MON_BITE      = 38,
    MSG_MON_STING     = 39,
    MSG_MON_BUTT      = 40,
    MSG_MON_CRUSH     = 41,
    MSG_MON_ENGULF    = 42,
    MSG_MON_CRAWL     = 43,
    MSG_MON_DROOL     = 44,
    MSG_MON_SPIT      = 45,
    MSG_MON_GAZE      = 46,
    MSG_MON_WAIL      = 47,
    MSG_MON_SPORE     = 48,
    MSG_MON_BEG       = 49,
    MSG_MON_INSULT    = 50,
    MSG_MON_MOAN      = 51,
    MSG_RECOVER       = 52,
    MSG_BLIND         = 53,
    MSG_CONFUSED      = 54,
    MSG_POISONED      = 55,
    MSG_AFRAID        = 56,
    MSG_PARALYZED     = 57,
    MSG_DRUGGED       = 58,
    MSG_SPEED         = 59,
    MSG_SLOW          = 60,
    MSG_SHIELD        = 61,
    MSG_BLESSED       = 62,
    MSG_HERO          = 63,
    MSG_BERSERK       = 64,
    MSG_PROT_EVIL     = 65,
    MSG_INVULN        = 66,
    MSG_SEE_INVIS     = 67,
    MSG_INFRARED      = 68,
    MSG_RES_ACID      = 69,
    MSG_RES_ELEC      = 70,
    MSG_RES_FIRE      = 71,
    MSG_RES_COLD      = 72,
    MSG_RES_POIS      = 73,
    MSG_STUN          = 74,
    MSG_CUT           = 75,
    MSG_STAIRS_UP     = 76,
    MSG_STORE_ENTER   = 77,
    MSG_STORE_LEAVE   = 78,
    MSG_STORE_HOME    = 79,
    MSG_MONEY1        = 80,
    MSG_MONEY2        = 81,
    MSG_MONEY3        = 82,
    MSG_SHOOT_HIT     = 83,
    MSG_STORE5        = 84,
    MSG_LOCKPICK      = 85,
    MSG_DISARM        = 86,
    MSG_IDENT_BAD     = 87,
    MSG_IDENT_EGO     = 88,
    MSG_IDENT_ART     = 89,
    MSG_BR_ELEMENTS   = 90,
    MSG_BR_FROST      = 91,
    MSG_BR_ELEC       = 92,
    MSG_BR_ACID       = 93,
    MSG_BR_GAS        = 94,
    MSG_BR_FIRE       = 95,
    MSG_BR_CONF       = 96,
    MSG_BR_DISENCHANT = 97,
    MSG_BR_CHAOS      = 98,
    MSG_BR_SHARDS     = 99,
    MSG_BR_SOUND      = 100,
    MSG_BR_LIGHT      = 101,
    MSG_BR_DARK       = 102,
    MSG_BR_NETHER     = 103,
    MSG_BR_NEXUS      = 104,
    MSG_BR_TIME       = 105,
    MSG_BR_INERTIA    = 106,
    MSG_BR_GRAVITY    = 107,
    MSG_BR_PLASMA     = 108,
    MSG_BR_FORCE      = 109,
    MSG_SUM_MONSTER   = 110,
    MSG_SUM_AINU     = 111,
    MSG_SUM_UNDEAD    = 112,
    MSG_SUM_ANIMAL    = 113,
    MSG_SUM_SPIDER    = 114,
    MSG_SUM_HOUND     = 115,
    MSG_SUM_HYDRA     = 116,
    MSG_SUM_DEMON     = 117,
    MSG_SUM_DRAGON    = 118,
    MSG_SUM_HI_UNDEAD = 119,
    MSG_SUM_HI_DRAGON = 120,
    MSG_SUM_HI_DEMON  = 121,
    MSG_SUM_WRAITH    = 122,
    MSG_SUM_UNIQUE    = 123,
    MSG_WIELD         = 124,
    MSG_CURSED        = 125,
    MSG_PSEUDOID      = 126,
    MSG_HUNGRY        = 127,
    MSG_NOTICE        = 128,
    MSG_AMBIENT_DAY   = 129,
    MSG_AMBIENT_NITE  = 130,
    MSG_AMBIENT_DNG1  = 131,
    MSG_AMBIENT_DNG2  = 132,
    MSG_AMBIENT_DNG3  = 133,
    MSG_AMBIENT_DNG4  = 134,
    MSG_AMBIENT_DNG5  = 135,
    MSG_CREATE_TRAP   = 136,
    MSG_SHRIEK        = 137,
    MSG_CAST_FEAR     = 138,
    MSG_HIT_GOOD      = 139,
    MSG_HIT_GREAT     = 140,
    MSG_HIT_SUPERB    = 141,
    MSG_HIT_HI_GREAT  = 142,
    MSG_HIT_HI_SUPERB = 143,
    MSG_SPELL         = 144,
    MSG_PRAYER        = 145,
    MSG_KILL_UNIQUE   = 146,
    MSG_KILL_KING     = 147,
    MSG_DRAIN_STAT    = 148,
    MSG_MULTIPLY      = 149,
    MSG_LOSING_NATIVITY = 150,
    MSG_LOSING_FLYING	= 151,
    MSG_HIDE_UNHIDE	= 152,

    MSG_MAX           = 153,
    SOUND_MAX         = MSG_MAX
};

/*
 *  Keeping track of what type of
 *  information we are displaying.
 */
enum
{
    DISPLAY_INFO_FEATURE,
    DISPLAY_INFO_MONSTER,
    DISPLAY_INFO_OBJECT
};




#endif // DEFINES_H
