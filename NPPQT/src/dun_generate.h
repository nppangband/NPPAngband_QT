#ifndef DUN_GENERATE_H
#define DUN_GENERATE_H

/*
 * Dungeon generation values
 */


#define DUN_ROOMS	50
#define DUN_UNUSUAL_MORIA	300	/* Level/chance of unusual room */
#define DUN_UNUSUAL	180	/* Level/chance of unusual room */
#define DUN_DEST	35	/* 1/chance of having a destroyed level */
#define DUN_FRACTAL	25	/* 1/chance of having a fractal level */
#define SMALL_LEVEL	10	/* 1/chance of smaller size */
#define THEMED_LEVEL_CHANCE	75	/* 1/chance of being a themed level */
#define WILDERNESS_LEVEL_CHANCE	75 /* 1/chance of being a pseudo-wilderness level */
#define LABYRINTH_LEVEL_CHANCE	75 /* 1/chance of being a labrynth level */
#define GREATER_VAULT_LEVEL_CHANCE	150 /* 1/chance of being a greater vault level */

#define DUN_MAX_LAKES	3	/* Maximum number of lakes/rivers */
#define DUN_FEAT_RNG	2	/* Width of lake */

/* Maximum size of a fractal map */
#define MAX_FRACTAL_SIZE 65

/*
 * Dungeon tunnel generation values
 */
#define DUN_TUN_RND	10	/* Chance of random direction */
#define DUN_TUN_CHG	30	/* Chance of changing direction */
#define DUN_TUN_CON	15	/* Chance of extra tunneling */
#define DUN_TUN_PEN	25	/* Chance of doors at room entrances */
#define DUN_TUN_JCT	90	/* Chance of doors at tunnel junctions */

/*
 * Dungeon streamer generation values
 */
#define DUN_STR_DEN	5	/* Density of streamers */
#define DUN_STR_RNG	2	/* Width of streamers */
#define DUN_STR_MAG	3	/* Number of magma streamers */
#define DUN_STR_MC	90	/* 1/chance of treasure per magma */
#define DUN_STR_QUA	2	/* Number of quartz streamers */
#define DUN_STR_QC	40	/* 1/chance of treasure per quartz */
#define DUN_STR_SAN	2	/* Number of sandstone streamers */
#define DUN_STR_SLV	40	/* Deepest level sandstone occurs instead of magma */
#define DUN_STR_GOL	20	/* 1/chance of rich mineral vein */
#define DUN_STR_GC	2	/* 1/chance of treasure per rich mineral vein */
#define DUN_STR_CRA	8	/* 1/chance of cracks through dungeon */
#define DUN_STR_CC	0	/* 1/chance of treasure per crack */

/*
 * Dungeon treausre allocation values
 */
#define DUN_AMT_ROOM	9	/* Amount of objects for rooms */
#define DUN_AMT_ITEM	3	/* Amount of objects for rooms/corridors */
#define DUN_AMT_GOLD	3	/* Amount of treasure for rooms/corridors */

#define DUN_AMT_ROOM_MORIA	7	/* Amount of objects for rooms */
#define DUN_AMT_ITEM_MORIA	2	/* Amount of objects for rooms/corridors */
#define DUN_AMT_GOLD_MORIA	2	/* Amount of treasure for rooms/corridors */

/*
 * Hack -- Dungeon allocation "places"
 */
#define ALLOC_SET_CORR		1	/* Hallway */
#define ALLOC_SET_ROOM		2	/* Room */
#define ALLOC_SET_BOTH		3	/* Anywhere */

/*
 * Hack -- Dungeon allocation "types"
 */
#define ALLOC_TYP_RUBBLE	1	/* Rubble */
#define ALLOC_TYP_TRAP		3	/* Trap */
#define ALLOC_TYP_GOLD		4	/* Gold */
#define ALLOC_TYP_OBJECT	5	/* Object */
#define ALLOC_TYP_CHEST		6	/* Object */
#define ALLOC_TYP_PARCHMENT 7	/* Special parchment, for collection quests */


/*
 * Maximum numbers of rooms along each axis (currently 6x18)
 */

#define MAX_ROOMS_ROW	(MAX_DUNGEON_HGT / BLOCK_HGT)
#define MAX_ROOMS_COL	(MAX_DUNGEON_WID / BLOCK_WID)

/*
 * Bounds on some arrays used in the "dun_data" structure.
 * These bounds are checked, though usually this is a formality.
 */
#define CENT_MAX	110
#define DOOR_MAX	200
#define WALL_MAX	500
#define TUNN_MAX	900


/*
 * These flags control the construction of starburst rooms and lakes
 */
#define STAR_BURST_ROOM		0x00000001	/* Mark grids with CAVE_ROOM */
#define STAR_BURST_LIGHT	0x00000002	/* Mark grids with CAVE_GLOW */
#define STAR_BURST_CLOVER	0x00000004	/* Allow cloverleaf rooms */
#define STAR_BURST_RAW_FLOOR	0x00000008	/* Floor overwrites dungeon */
#define STAR_BURST_RAW_EDGE	0x00000010	/* Edge overwrites dungeon */


extern bool allow_uniques;


/*
 * Maximal number of room types
 */
#define ROOM_MAX	15
#define ROOM_MIN	2

/*
 * Maximum distance between rooms
 */
#define MAX_RANGE_TO_ROOM 15




/*
 * Room type information
 */

typedef struct room_data room_data;

struct room_data
{
    /* Required size in blocks */
    s16b dy1, dy2, dx1, dx2;

    /* Hack -- minimum level */
    s16b level;
};


/*
 * Structure to hold all "dungeon generation" data
 */

typedef struct dun_data dun_data;

struct dun_data
{
    /* Array of centers of rooms */
    int cent_n;
    coord cent[CENT_MAX];

    /* Array of possible door locations */
    int door_n;
    coord door[DOOR_MAX];

    /* Array of wall piercing locations */
    int wall_n;
    coord wall[WALL_MAX];

    /* Array of tunnel grids */
    int tunn_n;
    coord tunn[TUNN_MAX];

    /* Number of blocks along each axis */
    int row_rooms;
    int col_rooms;

    /* Array of which blocks are used */
    bool room_map[MAX_ROOMS_ROW][MAX_ROOMS_COL];

    /* Hack -- there is a pit/nest on this level */
    bool crowded;
};


/*
 * Dungeon generation data -- see "cave_gen()"
 */
extern dun_data *dun;


/*
 * Array of room types (assumes 11x11 blocks)
 */
static const room_data room[ROOM_MAX] =
{
    { 0, 0, 0, 0, 0 },		/* 0 = Nothing */
    { 0, 0, -1, 1, 1 },		/* 1 = Simple (33x11) */
    { 0, 0, -1, 1, 1 },		/* 2 = Overlapping (33x11) */
    { 0, 0, -1, 1, 3 },		/* 3 = Crossed (33x11) */
    { 0, 0, -1, 1, 3 },		/* 4 = Large (33x11) */
    { 0, 0, -1, 1, 5 },		/* 5 = Monster nest (33x11) */
    { 0, 0, -1, 1, 5 },		/* 6 = Monster pit (33x11) */
    { 0, 1, -1, 1, 5 },		/* 7 = Lesser vault (33x22) */
    { -1, 2, -2, 3, 10 },	/* 8 = Greater vault (66x44) */
    { 0, 1, -1, 1, 0 },		/* 9 = Quest vault (44x22) */
    { 0, 1, -1, 1, 0},		/* 10 = Starburst (33x22) */
    { -1, 2, -2, 3, 0},		/* 11 = Great Starburst (66x44) */

    /*
     * Hack - if the dimensions for starburst rooms or great starburst
     * rooms change, these lines in build_type_starburst need changing:
     if (giant_room)
     {
        dy = 19;
        dx = 30;
     }
     33x22
     else
     {
        dy = 9;
        dx = 14;
     }

     -JG
     */

    {0, 1, -1, 1, 0},		/* 12 = Small Fractal (33x22) */
    {-1, 1, -1, 1, 0},		/* 13 = Medium Fractal (33x33) */
    {-1, 1, -2, 3, 0}		/* 14 = Big Fractal (66x33) */
};






/*
 * A feature pair (wall + floor) that contains also a chance value
 * and a radius
 */
typedef struct
{
    u32b level_flag;
    u16b wall;
    u16b floor;
    u16b chance;
    byte rad;
} feature_selector_item_type;

#define CUR_NUM_THEME_LEVEL_FLAGS 8
typedef struct
{
    byte theme;
    u32b flags;		/* A combination of the LF1_* flags */
} flags_themed_levels;
extern flags_themed_levels themed_level_flags[CUR_NUM_THEME_LEVEL_FLAGS];


/* Available fractal map types */
enum
{
    FRACTAL_TYPE_17x33 = 0,
    FRACTAL_TYPE_33x65,
    FRACTAL_TYPE_9x9,
    FRACTAL_TYPE_17x17,
    FRACTAL_TYPE_33x33,
    MAX_FRACTAL_TYPES
};

// dun_generate functions
extern int next_to_walls(int y, int x);
extern bool new_player_spot_old(void);
extern void basic_granite(void);
extern void set_perm_boundry(void);
extern bool scramble_and_connect_rooms_stairs(void);
extern bool place_traps_rubble_player(void);
extern void build_tunnel(int row1, int col1, int row2, int col2);
extern bool alloc_stairs(u16b feat, int num);
extern bool place_monsters_objects(void);
extern void alloc_object(int set, int typ, int num);


// dun_gen_rooms functions
extern void mark_g_vault(int y0, int x0, int hgt, int wid);
extern void build_vault(int y0, int x0, const vault_type *v_ptr);
extern bool room_build(int by0, int bx0, int typ);
extern void generate_fill(int y1, int x1, int y2, int x2, u16b feat);

// dun_gen_features functions
extern bool generate_starburst_room(int y1, int x1, int y2, int x2,
                                           u16b feat, u16b edge, u32b flag);

extern bool build_type_fractal(int y0, int x0, byte type);
extern void build_type_starburst(int y0, int x0, bool giant_room);
extern void build_nature(void);
extern void build_misc_features(void);

extern bool build_wilderness_level(void);


//dun_gen_quest_levels.cpp
extern bool monster_wilderness_labrynth_okay(int r_idx);
extern bool build_themed_level(void);
extern bool build_arena_level(void);
extern bool build_labyrinth_level(void);
extern bool build_greater_vault_level(void);

/* These ones are the valid values for the map grids */
#define FRACTAL_NONE	0	/* Used only at construction time */
#define FRACTAL_WALL	1	/* Wall grid */
#define FRACTAL_EDGE	2	/* Wall grid adjacent to floor (outer wall) */
#define FRACTAL_FLOOR	3	/* Floor grid */
#define FRACTAL_POOL_1	4	/* Pool grid */
#define FRACTAL_POOL_2	5	/* Pool grid */
#define FRACTAL_POOL_3	6	/* Pool grid */

#endif // DUN_GENERATE_H
