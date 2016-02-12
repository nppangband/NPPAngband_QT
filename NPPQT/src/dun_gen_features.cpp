
/* File: gen_capabilities.cpp */

/*
 * Copyright (c) 2010 Jeff Greene, Diego Gonzalez
 * Please see copyright.txt for complete copyright and licensing restrictions.
 */

#include "src/npp.h"
#include "src/dun_generate.h"


/*
 * Table of "magic" values used to ponder the size of the dungeon
 * 1 means "tiny": 2x2, 2x3, 2x4, 3x2, 3x3, 4x2
 * 2 means "small rectangle": 2x5, 2x6, 3x4, 4x3, 5x2, 6x2
 * 3 means "medium rectangle": 3x5, 3x6, 4x4, 5x3, 6x3
 * 4 means "medium": 4x5, 4x6, 5x4, 6x4
 * 5 means "large": 5x5, 5x6, 6x5
 * 6 means "largest": 6x6
 */
static byte dungeon_size_tab[7][7] =
{
    /*	0	1	2	3	4	5	6	*/
    {	0,	0,	0,	0,	0,	0,	0	},	/* 0 */
    {	0,	0,	0,	0,	0,	0,	0	},	/* 1 */
    {	0,	0,	1,	1,	1,	2,	2	},	/* 2 */
    {	0,	0,	1,	1,	2,	3,	3	},	/* 3 */
    {	0,	0,	1,	2,	3,	4,	4	},	/* 4 */
    {	0,	0,	2,	3,	4,	5,	5	},	/* 5 */
    {	0,	0,	2,	3,	4,	5,	6	},	/* 6 */
};


/*
 * Return a number between 1 and 6 that describes the size of the dungeon
 */
static byte ponder_dungeon_size(void)
{
    /* Get real dungeon size in panels */
    int hgt = (p_ptr->cur_map_hgt / PANEL_HGT);
    int wid = (p_ptr->cur_map_wid / PANEL_WID);

    /* Paranoia. Check limits */
    if (hgt > 6) hgt = 6;
    if (hgt < 2) hgt = 2;

    if (wid > 6) wid = 6;
    if (wid < 2) wid = 2;

    /* Ponder size */
    return (dungeon_size_tab[hgt][wid]);
}


/*
 * Return TRUE if the given feature is suitable to make a lake or river
 * on the current level
 */
static bool cave_feat_lake(int f_idx)
{
    /* Get the feature */
    feature_type *f_ptr = &f_info[f_idx];

    /* If this is TRUE we ignore the feature if certain elemental flags already exist in the level */
    bool reject_elements = FALSE;

    /* Require lake or river */
    if (!_feat_ff2_match(f_ptr, FF2_LAKE | FF2_RIVER)) return (FALSE);

    /* Special case. True lava changes all deep features */
    /*if ((current_flags & (LF1_LAVA)) && _feat_ff2_match(f_ptr, FF2_DEEP)) return (TRUE);*/

    /* Analyze the elemental flags */
    switch (_feat_ff3_match(f_ptr, TERRAIN_MASK))
    {
        /* "Boring" features */
        case 0:
        case ELEMENT_SAND:
        case ELEMENT_MUD:
        case ELEMENT_FOREST:
        {
            /* Don't mess with lava levels */
            if (level_flag & (LF1_LAVA)) return (FALSE);

            break;
        }

        case ELEMENT_LAVA:
        {
            /* True lava needs true lava or an empty dungeon */
            if (!(level_flag & (LF1_LAVA))) reject_elements = TRUE;

            break;
        }

        case ELEMENT_BMUD:
        case ELEMENT_BWATER:
        {
            /* These ones need lava, bmud, bwater or an empty dungeon */
            if (!(level_flag & (LF1_LAVA | LF1_BMUD | LF1_BWATER))) reject_elements = TRUE;

            break;
        }

        case ELEMENT_FIRE:
        case ELEMENT_OIL:
        {
            /* These two and true lava are compatible */
            if (!(level_flag & (LF1_OIL | LF1_FIRE | LF1_LAVA))) reject_elements = TRUE;

            break;
        }

        case ELEMENT_ACID:
        {
            /* Acid needs acid or an empty dungeon */
            if (!(level_flag & (LF1_ACID))) reject_elements = TRUE;

            break;
        }

        case ELEMENT_ICE:
        {
            /* Ice needs ice or an empty dungeon */
            if (!(level_flag & (LF1_ICE))) reject_elements = TRUE;

            break;
        }

        case ELEMENT_WATER:
        {
            /* Don't mess with lava levels */
            if (level_flag & (LF1_LAVA)) return (FALSE);

            /* Water needs water, boiling water or ice */
            /* Water is also compatible with acid (flavor) */
            if (!(level_flag & (LF1_ACID | LF1_BWATER | LF1_WATER | LF1_ICE)))
            {
                reject_elements = TRUE;
            }

            break;
        }
    }

    /* Test the presence of certaine flags in the level if necessary */
    if (reject_elements && (level_flag &
        (LF1_LAVA | LF1_FIRE | LF1_OIL | LF1_ACID | LF1_WATER | LF1_ICE | LF1_BMUD | LF1_BWATER)))
    {
        /* Failure */
        return (FALSE);
    }

    /* Success */
    return (TRUE);
}


/*
 * Note that the order we generate the dungeon is terrain features, then
 * rooms, then corridors, then streamers. This is important, because
 * (currently) we ensure that deep or hostile terrain is bridged by safe
 * terrain, and rooms (and vaults) alway have their respective walls intact.
 *
 * Note that rooms can be generated inside 'big' lakes, but not on regular
 * lakes. We take a risk here that 'big' lakes are less likely to have
 * areas rendered inaccessible by having a room block them.
 *
 * XXX XXX XXX Currently both types of lakes can have areas that are completely
 * blocked because of the 20% chance of filling a lake centre location with
 * a lake edge location. We should always guarantee that all areas are connected.
 *
 * XXX XXX These huge case statements should be cut down by using WALL, FLOOR,
 * etc. flags to take out the common cases and ensuring we never overwrite a
 * dun square with an edge square. But the resulting code might be less
 * efficient.
 */

/*
 * Places a terrain on another terrain
 */

void build_terrain(int y, int x, int feat)
{
    int oldfeat, newfeat;
    int k;
    int effect_rock = 0;

    feature_type *f_ptr;
    feature_type *f2_ptr;

    /* Get the feature */
    oldfeat = dungeon_info[y][x].feature_idx;
    f_ptr = &f_info[oldfeat];

    /* Set the new feature */
    newfeat = oldfeat;
    f2_ptr = &f_info[feat];

    /* Paranoia */
    if (!oldfeat)
    {
        newfeat = feat;
    }
    /* Put the feature quickly if we are overriding boring walls */
    else if (_feat_ff1_match(f_ptr, FF1_WALL) &&
        !_feat_ff3_match(f_ptr, TERRAIN_MASK))
    {
        newfeat = feat;
    }
    /* Tunnel the old feature */
    else if (_feat_ff1_match(f2_ptr, FF1_FLOOR) &&
        _feat_ff1_match(f_ptr, FF1_CAN_TUNNEL))
    {
        newfeat = feat_state(oldfeat, FS_TUNNEL);
    }
    /* Chasm edge, stone bridge */
    else if (_feat_ff2_match(f2_ptr, FF2_BRIDGED))
    {
        newfeat = feat;
    }
    /*
     * EXPERIMENTAL. Leave some grids untouched when overlapping lakes.
     * Note that we check for a match of the LOS, PROJECT and MOVE flags to
     * support rivers and tree branches properly (this is a hack).
     */
    else if (_feat_ff3_match(f2_ptr, TERRAIN_MASK) &&
        _feat_ff3_match(f_ptr, TERRAIN_MASK) &&
        (_feat_ff1_match(f2_ptr, FF1_MOVE | FF1_LOS | FF1_PROJECT) ==
        _feat_ff1_match(f_ptr, FF1_MOVE | FF1_LOS | FF1_PROJECT)) &&
        one_in_(4))
    {
        newfeat = oldfeat;
    }
    /* Handle new lava */
    else if (_feat_ff3_match(f2_ptr, FF3_LAVA))
    {
        /* We are filling a hole in the dungeon */
        if (_feat_ff2_match(f_ptr, FF2_DEEP) &&
            !_feat_ff3_match(f_ptr, FF3_LAVA))
        {
            /* Heat the water */
            if (_feat_ff3_match(f_ptr, FF3_WATER))
            {
                newfeat = FEAT_FLOOR_WATER_BOILING;
            }
            /* Melt the old feature */
            else
            {
                newfeat = FEAT_FLOOR_MUD_BOILING;
            }
        }
        /* Burn old features */
        else if (_feat_ff2_match(f_ptr, FF2_HURT_FIRE))
        {
            newfeat = feat_state(oldfeat, FS_HURT_FIRE);
        }
        /* Lava overrides all */
        else
        {
            newfeat = feat;
        }
    }
    /* Handle old lava */
    else if (_feat_ff3_match(f_ptr, FF3_LAVA))
    {
        /* We are digging a hole in the lava */
        if (_feat_ff2_match(f2_ptr, FF2_DEEP))
        {
            /* Heat the water */
            if (_feat_ff3_match(f2_ptr, FF3_WATER))
            {
                newfeat = FEAT_FLOOR_WATER_BOILING;
            }
            /* Melt the new feature */
            else
            {
                newfeat = FEAT_FLOOR_MUD_BOILING;
            }
        }
    }

    /* Handle new fire */
    else if (_feat_ff3_match(f2_ptr, FF3_FIRE))
    {
        /* Burn the old feature */
        if (_feat_ff2_match(f_ptr, FF2_HURT_FIRE))
        {
            newfeat = feat_state(oldfeat, FS_HURT_FIRE);
        }
        /* Some features resist fire */
        else if (!_feat_ff3_match(f_ptr, FF3_ICE | FF3_WATER))
        {
            newfeat = feat;
        }
    }

    /* Handle new ice */
    else if (_feat_ff3_match(f2_ptr, FF3_ICE) &&
        _feat_ff2_match(f_ptr, FF2_HURT_COLD))
    {
        newfeat = feat_state(oldfeat, FS_HURT_COLD);
    }
    /* Handle new water */

    else if (_feat_ff3_match(f2_ptr, FF3_WATER))
    {
        if (!_feat_ff3_match(f_ptr, FF3_WATER))
        {
            newfeat = feat;
        }
        /* Note that old water is unnafected to avoid "wave lakes" */
        else if ((_feat_ff3_match(f_ptr, TERRAIN_MASK) != FF3_WATER) &&
            _feat_ff2_match(f_ptr, FF2_HURT_WATER))
        {
            newfeat = feat_state(oldfeat, FS_HURT_WATER);
        }
    }
    /* All special cases were ignored. Put the feature */
    else
    {
        newfeat = feat;
    }

    /* Hack -- no change */
    if (newfeat == oldfeat) return;

    /* Get the chance to replace a feature */
    k = randint(100);

    /* Replace some features with something else (sometimes) */
    switch (newfeat)
    {
        case FEAT_WALL_LIMESTONE:
        {
            if (k <= 40) newfeat = FEAT_FLOOR;

            else if (k <= 60) newfeat = FEAT_FLOOR_WATER;

            break;
        }

        case FEAT_FLOOR_MUD:
        {
            if (k <= 10) newfeat = FEAT_FLOOR_EARTH;

            else if (k <= 23) newfeat = FEAT_FLOOR_MUD;

            else if (k <= 24) newfeat = FEAT_TREE;

            break;
        }

        case FEAT_FLOOR_MUD_BOILING:
        {
            if (k <= 10) newfeat = FEAT_FLOOR_WATER_BOILING;

            break;
        }

        case FEAT_FLOOR_WATER_BOILING:
        {
            if (k <= 10) newfeat = FEAT_MAGMA_VEIN;

            else if (k <= 13) newfeat = FEAT_WALL_WATER_BOILING_GEYSER;

            break;
        }

        case FEAT_FOREST_SOIL:
        {
            if (k <= 10) newfeat = FEAT_TREE;

            else if (k <= 20) newfeat = FEAT_FOREST_SOIL;

            else if (k <= 30) newfeat = FEAT_FOREST_SOIL_DYNAMIC;

            else if (k <= 40) newfeat = FEAT_GRASS;

            else if (k <= 45) newfeat = FEAT_GRASS_DYNAMIC;

            break;
        }

        case FEAT_FLOOR_ICE:
        {
            if (k <= 5) newfeat = FEAT_WALL_ICE_CRACKED;

            break;
        }

        case FEAT_WALL_ICE:
        {
            if (k <= 25) newfeat = FEAT_WALL_ICE_CRACKED;

            else if (k <= 50) newfeat = FEAT_FLOOR_ICE;

            break;
        }

        case FEAT_WALL_ICE_CRACKED:
        {
            if (k <= 90) newfeat = FEAT_WALL_ICE;

            break;
        }

        case FEAT_FLOOR_ACID:
        {
            if (k <= 5)
            {
                newfeat = FEAT_CRACKED_WALL_OVER_ACID;
            }

            break;
        }

        case FEAT_WALL_COAL:
        {
            if (k <= 5) newfeat = FEAT_WALL_COAL_BURNING;

            else if (k <= 50) newfeat = FEAT_BURNT_SPOT;

            else if (k <= 60) newfeat = FEAT_FIRE;

            break;
        }

        case FEAT_WALL_SHALE:
        {
            if (k <= 5) newfeat = FEAT_WALL_COAL;

            break;
        }


        case FEAT_FLOOR_SAND:
        {
            if (k <= 5) newfeat = FEAT_WALL_SANDSTONE;

            else if (k <= 10) newfeat = FEAT_QUARTZ_VEIN;

            else if (k <= 20)
            {
                newfeat = FEAT_FLOOR_ROCK;
                effect_rock = FEAT_LOOSE_ROCK;
            }

            else if (k <= 25) newfeat = FEAT_FLOOR_ROCK;

            break;
        }

        case FEAT_THICKET:
        {
            if (k <= 5) newfeat = FEAT_BUSH;

            else if (k <= 15) newfeat = FEAT_THORNS;

            else if (k <= 20) newfeat = FEAT_BRAMBLES;

            else if (k <= 21) newfeat = FEAT_TREE;

            else if (k <= 25) newfeat = FEAT_FLOOR_PEBBLES;

            else if (k <= 30)
            {
                if (level_flag & (ELEMENT_WATER)) newfeat = FEAT_FLOOR_WATER;
                else newfeat = FEAT_FLOOR_MUD;
            }

            else if (k <= 40) newfeat = FEAT_FLOOR_MUD;

            else if (k <= 70) newfeat = FEAT_FLOOR_EARTH;

            break;
        }

        case FEAT_FLOOR_LAVA:
        {
            if (k <= 10) newfeat = FEAT_WALL_OF_FIRE;

            else if (k <= 15) newfeat = FEAT_SCORCHED_WALL;

            break;
        }

        case FEAT_FLOOR_EARTH:
        {
            if (k <= 5) newfeat = FEAT_FLOOR_ROCK;

            else if (k <= 10) newfeat = FEAT_FLOOR_PEBBLES;

            break;
        }

        case FEAT_FIRE:
        {
            if (k <= 25) newfeat = FEAT_BURNT_SPOT;

            break;
        }

    }

    /* Hack -- no change */
    if (newfeat == oldfeat) return;


    /* Set the new feature */
    cave_set_feat(y, x, newfeat);

    if (effect_rock) set_effect_rocks(effect_rock, y, x);

}


/*
 * Returns TRUE if f_idx is a valid pool feature
 */
static bool cave_feat_pool(int f_idx)
{
    feature_type *f_ptr = &f_info[f_idx];

    /* Hack -- Ignore ice pools on non-ice levels */
    if (!(level_flag & LF1_ICE) && _feat_ff3_match(f_ptr, FF3_ICE))
    {
        return (FALSE);
    }

    /* Hack -- Ignore solid features */
    if (!_feat_ff1_match(f_ptr, FF1_MOVE))
    {
        return (FALSE);
    }

    /* All remaining lake features will be fine */
    return (cave_feat_lake(f_idx));
}

/*
 * This table holds the default features used in *almost* all lakes and pools
 * of a themed level.
 * See "pick_proper_feature"
 */
static struct
{
    byte theme;
    u16b feature;
} themed_level_features[] =
{
    {LEV_THEME_DEMON_MINOR,	FEAT_FIRE},
    {LEV_THEME_DEMON_ALL,	FEAT_FIRE},
    {LEV_THEME_DEMON_MAJOR,	FEAT_FLOOR_LAVA},
    {LEV_THEME_DEMON_MAJOR,	FEAT_FIRE},
    {LEV_THEME_DRAGON_FIRE,	FEAT_FIRE},
    {LEV_THEME_DRAGON_ACID,	FEAT_FLOOR_ACID},
    {LEV_THEME_DRAGON_ELEC,	FEAT_FLOOR_WATER},
    {LEV_THEME_DRAGON_COLD,	FEAT_FLOOR_ICE},
    {LEV_THEME_TROLL,		FEAT_FOREST_SOIL},
    {LEV_THEME_OGRE,		FEAT_FOREST_SOIL},
    {LEV_THEME_OGRE,		FEAT_THICKET},
    /* Add entries for more themed levels if needed */
};


/*
 * Choose a terrain feature for the current level.
 * You can use a hook to ensure consistent terrain (lakes/pools).
 * This function handles themed levels as a special case. The "feeling"
 * global variable must be properly set to recognize the themed level. See
 * "build_themed_level".
 */
static u16b pick_proper_feature(bool (*feat_hook)(int f_idx))
{
    /* Default depth for the feature */
    int max_depth = p_ptr->depth;
    u16b feat;

    /* Special case - Themed levels with default features */
    /* Note that we have a small chance to ignore these features */
    if ((feeling >= LEV_THEME_HEAD) && (rand_int(100) < 75))
    {
        /* Get the theme */
        byte theme = feeling - LEV_THEME_HEAD;
        u16b features[10];
        u16b i, n;

        /* Find if we have to use default features for this level */
        for (i = n = 0; i < N_ELEMENTS(themed_level_features); i++)
        {
            /* Ignore mismatching themes */
            if (theme != themed_level_features[i].theme) continue;

            /* Get the feature */
            feat = themed_level_features[i].feature;

            /* Ignore features who are too deep for the player */
            if (f_info[feat].f_level > p_ptr->depth + 25) continue;

            /* IMPORTANT - Check consistency with the level */
            if (feat_hook && !feat_hook(feat)) continue;

            /* Feature is OK */
            features[n++] = feat;

            /* Paranoia */
            if (n >= N_ELEMENTS(features)) break;
        }

        /* Pick a default feature, if any */
        if (n > 0) return (features[rand_int(n)]);
    }

    /* Special case - Themed levels with random features */
    if (feeling >= LEV_THEME_HEAD)
    {
        /* Note that we have a boost to depth in themed levels */
        max_depth += 7;

        /* Quests have a bigger boost to depth */
        if (quest_check(p_ptr->depth) == QUEST_THEMED_LEVEL)
        {
            max_depth += 10;
        }

        /* Check bounds */
        max_depth = MIN(max_depth, MAX_DEPTH - 1);
    }

    /* Set the given hook, if any */
    get_feat_num_hook = feat_hook;

    get_feat_num_prep();

    /* Pick a feature */
    feat = get_feat_num(max_depth);

    /* Clear the hook */
    get_feat_num_hook = NULL;

    get_feat_num_prep();

    /* Return the feature */
    return (feat);
}


/*
 * A fractal map is a matrix (MAX_FRACTAL_SIZE * MAX_FRACTAL_SIZE) of
 * small numbers. Each number in the map should be replaced with a dungeon
 * feature. This way we can use fractal maps to perform different actions like
 * building rooms, placing pools, etc.
 *
 * We are going to store this matrix in dynamic memory so we have to define
 * a couple of new types:
 */

/*
 * A row of the fractal map
 */
typedef byte fractal_map_wid[MAX_FRACTAL_SIZE];

/*
 * FRACTAL MAP. An array of rows. It can be used as a two-dimensional array.
 */
typedef fractal_map_wid *fractal_map;

/*
 * VERY IMPORTANT: The map must be a square. Its size must be
 * "power of 2 plus 1". Valid values are 3, 5, 9, 17, 33, 65, 129, 257, etc.
 * The maximum supported size is 65.
 *
 * Aditional comments about the construction of the map:
 * Only nine grids of this square are processed at the beginning.
 * These grids are the four corners (who MUST be previously set to meaningful
 * values) and five inner grids.
 * The inner grids are the middle points of each side plus the grid
 * at the center.
 * These nine grids can be viewed as a "3x3" square.
 * Example:
 *
 * a*b
 * ***
 * c*d
 *
 * The algorithm supplies the values for the five inner grids by
 * randomly chosing one *adjacent* corner. Example:
 *
 * aab
 * cbd
 * ccd
 *
 * The next step is to consider this "3x3" square as a part of a larger
 * square:
 *
 * a*a*b
 * *****
 * c*b*d
 * *****
 * c*c*d
 *
 * Now we have four "3x3" squares. The process is repeated for each one of
 * them. The process is stopped when the initial square can't be "magnified"
 * any longer.
 */




typedef struct fractal_template fractal_template;

/* Initialization function for templates */
typedef void (*fractal_init_func)(fractal_map map, fractal_template *t_ptr);

/*
 * A fractal template is used to set the basic shape of the fractal.
 *
 * Templates must provide values for at least the four corners of the map.
 * See the examples.
 */
struct fractal_template
{
    /* The type of the fractal map (one of FRACTAL_TYPE_*) */
    byte type;

    /* The maximum size of the fractal map (3, 5, 9, 17, 33 or 65) */
    int size;

    /* The initialization function for this template */
    fractal_init_func init_func;
};


/* Verify that a point is inside a fractal */
#define IN_FRACTAL(template,y,x) \
    (((y) >= 0) && ((y) < (template)->size) && \
    ((x) >= 0) && ((x) < (template)->size))


/*
 * Places a line in a fractal map given its start and end points and a certain
 * grid type. To be used in template initialization routines.
 * Note: This is a very basic drawing routine. It works fine with vertical,
 * horizontal and the diagonal lines of a square. It doesn't support other
 * oblique lines well.
 */
static void fractal_draw_line(fractal_map map, fractal_template *t_ptr,
        int y1, int x1, int y2, int x2, byte content)
{
    int dx, dy;

    /* Get the proper increments to reach the end point */
    dy = ((y1 < y2) ? 1: (y1 > y2) ? -1: 0);
    dx = ((x1 < x2) ? 1: (x1 > x2) ? -1: 0);

    /* Draw the line */
    while (TRUE)
    {
        /* Stop at the first illegal grid */
        if (!IN_FRACTAL(t_ptr, y1, x1)) break;

        /* Set the new content of the grid */
        map[y1][x1] = content;

        /* We reached the end point? */
        if ((y1 == y2) && (x1 == x2)) break;

        /* Advance one position */
        y1 += dy;
        x1 += dx;
    }
}


/*
 * Places walls in the perimeter of a fractal map
 */
static void fractal_draw_borders(fractal_map map, fractal_template *t_ptr)
{
    int last = t_ptr->size - 1;

    /* Left */
    fractal_draw_line(map, t_ptr, 0, 0, last, 0, FRACTAL_WALL);
    /* Important: Leave some space for tunnels */
    fractal_draw_line(map, t_ptr, 0, 1, last, 1, FRACTAL_WALL);

    /* Right */
    fractal_draw_line(map, t_ptr, 0, last, last, last, FRACTAL_WALL);
    /* Important: Leave some space for tunnels */
    fractal_draw_line(map, t_ptr, 0, last - 1, last, last - 1, FRACTAL_WALL);

    /* Top */
    fractal_draw_line(map, t_ptr, 0, 1, 0, last - 1, FRACTAL_WALL);

    /* Bottom */
    fractal_draw_line(map, t_ptr, last, 1, last, last - 1, FRACTAL_WALL);
}


/*
 * Some fractal templates
 */

/* 17x33 template */
static void fractal1_init_func(fractal_map map, fractal_template *t_ptr)
{
    /* Borders */
    fractal_draw_borders(map, t_ptr);

    /*
     * Mega-hack -- place walls in the middle of the 33x33 map to generate
     * a 17x33 map
     */
    fractal_draw_line(map, t_ptr, 16, 1, 16, 32, FRACTAL_WALL);

    map[8][8] = (one_in_(15) ? FRACTAL_POOL_1: FRACTAL_FLOOR);
    map[8][16] = (one_in_(15) ? FRACTAL_POOL_2: FRACTAL_FLOOR);
    map[8][24] = (one_in_(15) ? FRACTAL_POOL_3: FRACTAL_FLOOR);
}


/* 33x65 template */
static void fractal2_init_func(fractal_map map, fractal_template *t_ptr)
{
    int k;

    /* Borders */
    fractal_draw_borders(map, t_ptr);

    /*
     * Mega-hack -- place walls in the middle of the 65x65 map to generate
     * a 33x65 map
     */
    fractal_draw_line(map, t_ptr, 32, 1, 32, 64, FRACTAL_WALL);

    k = rand_int(100);
    /* 1 in 5 chance to make a pool and 1 in 5 to leave the map untouched */
    if (k < 80) map[8][16] = ((k < 20) ? FRACTAL_POOL_2: FRACTAL_FLOOR);

    k = rand_int(100);
    /* 1 in 4 chance to make a pool and 1 in 4 to leave the map untouched */
    if (k < 75) map[8][32] = ((k < 25) ? FRACTAL_POOL_3: FRACTAL_FLOOR);

    k = rand_int(100);
    /* 1 in 5 chance to make a pool and 1 in 5 to leave the map untouched */
    if (k < 80) map[8][48] = ((k < 20) ? FRACTAL_POOL_1: FRACTAL_FLOOR);

    map[16][16] = (one_in_(4) ? FRACTAL_POOL_1: FRACTAL_FLOOR);
    map[16][32] = (one_in_(3) ? FRACTAL_POOL_2: FRACTAL_FLOOR);
    map[16][48] = (one_in_(4) ? FRACTAL_POOL_3: FRACTAL_FLOOR);

    k = rand_int(100);
    /* 1 in 5 chance to make a pool and 1 in 5 to leave the map untouched */
    if (k < 80) map[24][16] = ((k < 20) ? FRACTAL_POOL_3: FRACTAL_FLOOR);

    k = rand_int(100);
    /* 1 in 4 chance to make a pool and 1 in 4 to leave the map untouched */
    if (k < 75) map[24][32] = ((k < 25) ? FRACTAL_POOL_1: FRACTAL_FLOOR);

    k = rand_int(100);
    /* 1 in 5 chance to make a pool and 1 in 5 to leave the map untouched */
    if (k < 80) map[24][48] = ((k < 20) ? FRACTAL_POOL_2: FRACTAL_FLOOR);
}


/* 9x9 template for pools */
static void fractal3_init_func(fractal_map map, fractal_template *t_ptr)
{
    /*Unused*/
    (void)t_ptr;

    /* Walls in the corners */
    map[0][0] = FRACTAL_WALL;
    map[0][8] = FRACTAL_WALL;
    map[8][0] = FRACTAL_WALL;
    map[8][8] = FRACTAL_WALL;

    map[2][4] = FRACTAL_FLOOR;
    map[4][2] = FRACTAL_FLOOR;
    map[4][4] = FRACTAL_FLOOR;
    map[4][6] = FRACTAL_FLOOR;
    map[6][4] = FRACTAL_FLOOR;
}


/* 17x17 template for pools */
static void fractal4_init_func(fractal_map map, fractal_template *t_ptr)
{
    /*Unused*/
    (void)t_ptr;

    /* Walls in the corners */
    map[0][0] = FRACTAL_WALL;
    map[0][16] = FRACTAL_WALL;
    map[16][0] = FRACTAL_WALL;
    map[16][16] = FRACTAL_WALL;

    map[4][8] = FRACTAL_FLOOR;
    map[8][4] = FRACTAL_FLOOR;
    map[8][8] = FRACTAL_FLOOR;
    map[8][12] = FRACTAL_FLOOR;
    map[12][8] = FRACTAL_FLOOR;
}


/* 33x33 template */
static void fractal5_init_func(fractal_map map, fractal_template *t_ptr)
{
    bool flip_h = one_in_(2);

    /* Borders */
    fractal_draw_borders(map, t_ptr);

    if (one_in_(15)) map[8][flip_h ? 24: 8] = FRACTAL_FLOOR;

    map[16][16] = FRACTAL_FLOOR;

    if (one_in_(15)) map[24][flip_h ? 8: 24] = FRACTAL_FLOOR;
}


/*
 * A list of the available fractal templates.
 */
static fractal_template fractal_repository[] =
{
    {FRACTAL_TYPE_17x33, 33, fractal1_init_func},
    {FRACTAL_TYPE_33x65, 65, fractal2_init_func},
    {FRACTAL_TYPE_9x9, 9, fractal3_init_func},
    {FRACTAL_TYPE_17x17, 17, fractal4_init_func},
    {FRACTAL_TYPE_33x33, 33, fractal5_init_func},
};


/*
 * Wipes the contents of a fractal map and applies the given template.
 */
static void fractal_map_reset(fractal_map map, fractal_template *t_ptr)
{
    int x, y;

    /* Fill the map with FRACTAL_NONE */
    for (y = 0; y < t_ptr->size; y++)
    {
        for (x = 0; x < t_ptr->size; x++)
        {
            map[y][x] = FRACTAL_NONE;
        }
    }

    /* Call the initialization function to place some floors */
    if (t_ptr->init_func)
    {
        t_ptr->init_func(map, t_ptr);
    }
}


/*
 * Returns a *reset* fractal map allocated in dynamic memory.
 * You must deallocate the map with FREE when it isn't used anymore.
 */
static fractal_map fractal_map_create(fractal_template *t_ptr)
{
    /* The new map */
    fractal_map map;

    /* Allocate the map */
    map = C_ZNEW(t_ptr->size, fractal_map_wid);

    /* Reset the contents of the map */
    fractal_map_reset(map, t_ptr);

    /* Done */
    return (map);
}


/*#define DEBUG_FRACTAL_TEMPLATES 1*/

#ifdef DEBUG_FRACTAL_TEMPLATES

/* This table is used to convert the map grids to printable characters */
static char fractal_grid_to_char[] =
{
    ' ',	/* FRACTAL_NONE */
    '#',	/* FRACTAL_WALL */
    '&',	/* FRACTAL_EDGE */
    '.',	/* FRACTAL_FLOOR*/
    '1',	/* FRACTAL_POOL_1*/
    '2',	/* FRACTAL_POOL_2*/
    '3',	/* FRACTAL_POOL_3*/
};

/*
 * Prints a fractal map to stdout. "title" is optional (can be NULL).
 */
static void fractal_map_debug(fractal_map map, fractal_template *t_ptr,
    char *title)
{
    int x, y;
    FILE *fff;
    static bool do_create = TRUE;

    fff = my_fopen("fractal.txt", do_create ? "w": "a");

    do_create = FALSE;

    if (!fff) return;

    /* Show the optional title */
    if (title)
    {
        fputs(title, fff);

        putc('\n', fff);
    }

    /* Show the map */
    for (y = 0; y < t_ptr->size; y++)
    {
        for (x = 0; x < t_ptr->size; x++)
        {
            byte grid = map[y][x];
            char chr;

            /* Check for strange grids */
            if (grid >= N_ELEMENTS(fractal_grid_to_char))
            {
                chr = '?';
            }
            /* Translate to printable char */
            else
            {
                chr = fractal_grid_to_char[grid];
            }

            /* Show it */
            putc(chr, fff);
        }

        /* Jump to the next row */
        putc('\n', fff);
    }

    putc('\n', fff);

    /* Done */
    my_fclose(fff);
}

#endif /* DEBUG_FRACTAL_TEMPLATES */


/*
 * Completes a fractal map. The map must have been reset.
 */
static void fractal_map_complete(fractal_map map, fractal_template *t_ptr)
{
    int x, y, x1, y1, x2, y2, cx, cy;
    /*
     * Set the initial size of the squares. At first, we have only
     * one big square.
     */
    int cur_size = t_ptr->size - 1;

    /*
     * Construct the map using a variable number of iterations.
     * Each iteration adds more details to the map.
     * This algorithm is originally recursive but we made it iterative
     * for efficiency.
     */
    do
    {
        /* Get the vertical coordinates of the first square */
        y1 = 0;
        y2 = cur_size;

        /*
         * Process the whole map. Notice the step used: (cur_size / 2)
         * is the middle point of the current "3x3" square.
         */
        for (y = 0; y < t_ptr->size; y += (cur_size / 2))
        {
            /* Change to the next "3x3" square, if needed */
            if (y > y2)
            {
                /*
                 * The end of the previous square becomes the
                 * beginning of the new square
                 */
                y1 = y2;

                /* Get the end of the new square */
                y2 += cur_size;
            }

            /* Get the horizontal coordinates of the first square */
            x1 = 0;
            x2 = cur_size;

            /* Notice the step */
            for (x = 0; x < t_ptr->size; x += (cur_size / 2))
            {
                /* Change to the next "3x3" square, if needed */
                if (x > x2)
                {
                    /*
                     * The end of the previous square
                     * becomes the beginning of the new
                     * square
                     */
                    x1 = x2;

                    /* Get the end of the new square */
                    x2 += cur_size;
                }

                /* IMPORTANT: ignore already processed grids */
                if (map[y][x] != FRACTAL_NONE) continue;

                /*
                 * Determine if the vertical coordinate of
                 * this grid should be fixed
                 */
                if ((y == y1) || (y == y2)) cy = y;
                /* Pick one *adjacent* corner randomly */
                else cy = ((rand_int(100) < 50) ? y1: y2);

                /*
                 * Determine if the horizontal coordinate of
                 * this grid should be fixed
                 */
                if ((x == x1) || (x == x2)) cx = x;
                /* Pick one *adjacent* corner randomly */
                else cx = ((rand_int(100) < 50) ? x1: x2);

                /* Copy the value of the chosed corner */
                map[y][x] = map[cy][cx];
            }
        }

    /* Decrease the size of the squares for the next iteration */
    cur_size /= 2;

    /* We stop when the squares can't be divided anymore */
    } while (cur_size > 1);
}


/*
 * Verify if all floor grids in a completed fractal map are connected.
 */
static int fractal_map_is_connected(fractal_map map, fractal_template *t_ptr)
{
    int x, y, i, connected = TRUE;
    fractal_map_wid *visited;
    /* Queue of visited grids */
    QVector<coord> grid_queue;
    grid_queue.clear();

    /* Allocate a "visited" matrix */
    visited = C_ZNEW(t_ptr->size, fractal_map_wid);

    /* Find a floor grid */
    for (y = 0; (y < t_ptr->size) && !grid_queue.size(); y++)
    {
        for (x = 0; (x < t_ptr->size) && !grid_queue.size(); x++)
        {
            /* Found one */
            if (map[y][x] >= FRACTAL_FLOOR)
            {
                /* Put it on the queue */
                grid_queue.append(make_coords(y, x));

                /* Mark as visited */
                visited[y][x] = TRUE;
            }
        }
    }

    /* Paranoia. No floor grid was found */
    if (!grid_queue.size())
    {
        /* Done */
        return (!connected);
    }

    /* Process all reachable floor grids */
    while (grid_queue.size())
    {
        /* Get the coordinates of the grid in the head of the queue */
        y = grid_queue.at(0).y;
        x = grid_queue.at(0).x;

        /* Remove that grid from the queue */
        grid_queue.removeFirst();

        /* Scan all adjacent grids */
        for (i = 0; i < 8; i++)
        {
            /* Get coordinates */
            int yy = y + ddy_ddd[i];
            int xx = x + ddx_ddd[i];

            /* Check bounds */
            if (!IN_FRACTAL(t_ptr, yy, xx)) continue;

            /* Ignore already processed grids */
            if (visited[yy][xx]) continue;

            /* Ignore walls */
            if (map[yy][xx] < FRACTAL_FLOOR) continue;

            /* Append the grid to the queue */
            grid_queue.append(make_coords(yy, xx));

            /* Mark as visited */
            visited[yy][xx] = TRUE;
        }
    }

    /* Find non-visited floor grids */
    for (y = 0; (y < t_ptr->size) && connected; y++)
    {
        for (x = 0; (x < t_ptr->size) && connected; x++)
        {
            /* Check the grid */
            if ((map[y][x] >= FRACTAL_FLOOR) && !visited[y][x])
            {
                /* Found a non-visited floor grid. Done */
                connected = FALSE;
            }
        }
    }

    /* Free resources */
    FREE_ARRAY(visited);

    /* Return answer */
    return connected;
}


/*
 * Places FRACTAL_EDGE walls in a completed fractal map. These grids were
 * created to be replaced by outer walls or other similar features.
 */
static void fractal_map_mark_edge(fractal_map map, fractal_template *t_ptr)
{
    int x, y, i;

    /* Process the whole map */
    for (y = 0; y < t_ptr->size; y++)
    {
        for (x = 0; x < t_ptr->size; x++)
        {
            /* Ignore wall grids */
            if (map[y][x] < FRACTAL_FLOOR) continue;

            /* Scan adjacent grids */
            for (i = 0; i < 8; i++)
            {
                /* Get coordinates */
                int yy = y + ddx_ddd[i];
                int xx = x + ddy_ddd[i];

                /* Check bounds */
                if (!IN_FRACTAL(t_ptr, yy, xx)) continue;

                /* Turn plain walls to edge walls */
                if (map[yy][xx] == FRACTAL_WALL)
                {
                    map[yy][xx] = FRACTAL_EDGE;
                }
            }
        }
    }
}

static struct
{
    int hgt, wid;
} fractal_dim[MAX_FRACTAL_TYPES] =
{
    {17, 33},		/* FRACTAL_TYPE_17x33 */
    {33, 65},		/* FRACTAL_TYPE_33x65 */
    {9, 9},			/* FRACTAL_TYPE_9x9 */
    {17, 17},		/* FRACTAL_TYPE_17x17 */
    {33, 33},		/* FRACTAL_TYPE_33x33 */
};

/*
 * Construct a fractal room given a fractal map and the room center's coordinates.
 */
static void fractal_map_to_room(fractal_map map, byte fractal_type, int y0, int x0)
{
    int x, y, y1, x1, wid, hgt;
    bool light = FALSE;
    int floor_type;
    /*
     * No pools for now. Note that we choose the features for the pool when
     * we need them. If we pick three random features right now we might
     * generate inconsistent levels.
     */
    u16b pool1 = FEAT_NONE;
    u16b pool2 = FEAT_NONE;
    u16b pool3 = FEAT_NONE;

    /* Get the dimensions of the fractal map */
    hgt = fractal_dim[fractal_type].hgt;
    wid = fractal_dim[fractal_type].wid;

    /* Get top-left coordinate */
    y1 = y0 - hgt / 2;
    x1 = x0 - wid / 2;

    /* Occasional light */
    if (p_ptr->depth <= randint(25)) light = TRUE;

    /* Use earth floor sometimes. EXPERIMENTAL */
    floor_type = rand_int(100);

    /* Apply the map to the dungeon */
    for (y = 0; y < hgt; y++)
    {
        for (x = 0; x < wid; x++)
        {
            byte grid_type = map[y][x];
            /* Translate to dungeon coordinates */
            int yy = y1 + y;
            int xx = x1 + x;

            /* Ignore annoying locations */
            if (!in_bounds_fully(yy, xx)) continue;

            /* Translate each grid type to dungeon features */
            if (grid_type >= FRACTAL_FLOOR)
            {
                u16b feat = FEAT_NONE;

                /* Pool grid */
                if (grid_type == FRACTAL_POOL_1)
                {
                    /* Pick a feature if necessary */
                    if (pool1 == FEAT_NONE)
                    {
                        pool1 = pick_proper_feature(cave_feat_pool);
                    }

                    /* Use the pool feature */
                    feat = pool1;
                }
                /* Pool grid */
                else if (grid_type == FRACTAL_POOL_2)
                {
                    /* Pick a feature if necessary */
                    if (pool2 == FEAT_NONE)
                    {
                        pool2 = pick_proper_feature(cave_feat_pool);
                    }

                    /* Use the pool feature */
                    feat = pool2;
                }
                /* Pool grid */
                else if (grid_type == FRACTAL_POOL_3)
                {
                    /* Pick a feature if necessary */
                    if (pool3 == FEAT_NONE)
                    {
                        pool3 = pick_proper_feature(cave_feat_pool);
                    }

                    /* Use the pool feature */
                    feat = pool3;
                }

                /* Place the selected pool feature */
                if (feat != FEAT_NONE)
                {
                    build_terrain(yy, xx, feat);
                }
                /* Or place a floor */
                else
                {
                    /* Use earth floor (15%) */
                    if (floor_type < 15)
                    {
                        feat = FEAT_FLOOR_EARTH;
                    }
                    /* Use scattered earth floor (5%) */
                    else if ((floor_type < 20) && one_in_(7))
                    {
                        feat = FEAT_FLOOR_EARTH;
                    }
                    /* Plain old floor (80%) */
                    else
                    {
                        feat = FEAT_FLOOR;
                    }

                    /* Place floors */
                    cave_set_feat(yy, xx, feat);
                }
            }
            else if (grid_type == FRACTAL_EDGE)
            {
                /* Place ice walls on ice levels */
                if (level_flag & LF1_ICE)
                {
                    build_terrain(yy, xx, FEAT_WALL_ICE);
                }
                /* Place usual walls on other levels */
                else
                {
                    cave_set_feat(yy, xx, FEAT_WALL_GRANITE);
                }
            }
            else
            {
                continue;
            }

            /* Mark the grid as a part of the room */
            dungeon_info[yy][xx].cave_info |= (CAVE_ROOM);

            /* Light the feature if needed */
            if (light)
            {
                dungeon_info[yy][xx].cave_info |= (CAVE_GLOW);
            }

            /* Or turn off the lights */
            else if (grid_type != FRACTAL_EDGE)
            {
                dungeon_info[yy][xx].cave_info &= ~(CAVE_GLOW);
            }
        }
    }
}


/*
 * Creates a fractal map given a template and copy part of it in the given map
 */
static void fractal_map_merge_another(fractal_map map, fractal_template *t_ptr)
{
    int y, x;

    fractal_map map2;

    /* Create the map */
    map2 = fractal_map_create(t_ptr);

    /* Complete it */
    fractal_map_complete(map2, t_ptr);

    /* Merge the maps */
    for (y = 0; y < t_ptr->size; y++)
    {
        for (x = 0; x < t_ptr->size; x++)
        {
            /* Sometimes we overwrite a grid in the original map */
            if ((map[y][x] != map2[y][x]) && one_in_(4)) map[y][x] = map2[y][x];
        }
    }

    /* Free resources */
    FREE_ARRAY(map2);
}


/*
 * Build a fractal room given its center. Returns TRUE on success.
 */
bool build_type_fractal(int y0, int x0, byte type)
{
    fractal_map map;
    fractal_template *t_ptr;
    int tries;
    bool do_merge = FALSE;

    /* Paranoia */
    if (type >= MAX_FRACTAL_TYPES) return (FALSE);

    /* Reset the loop counter */
    tries = 0;

    /* Get a fractal template */
    while (TRUE)
    {
        /* Get a template */
        int which = rand_int(N_ELEMENTS(fractal_repository));

        t_ptr = &fractal_repository[which];

        /* Check if the type matches the wanted one */
        if (t_ptr->type == type) break;

        /* Avoid infinite loops */
        if (++tries >= 100) return (FALSE);
    }

    /* Create and reset the fractal map */
    map = fractal_map_create(t_ptr);

#ifdef DEBUG_FRACTAL_TEMPLATES
    /* Show the template to the developer */
    fractal_map_debug(map, t_ptr, "Fractal");
#endif

    /* Make medium fractal rooms more exotic sometimes */
    if ((type == FRACTAL_TYPE_33x33) && !one_in_(3)) do_merge = TRUE;

    /* Reset the loop counter */
    tries = 0;

    /* Construct the fractal map */
    while (TRUE)
    {
        /* Complete the map */
        fractal_map_complete(map, t_ptr);

        /* Put another room on top of this one if necessary */
        if (do_merge) fractal_map_merge_another(map, t_ptr);

        /* Accept only connected maps */
        if (fractal_map_is_connected(map, t_ptr)) break;

        /* Avoid infinite loops */
        if (++tries >= 100)
        {
            /* Free resources */
            FREE_ARRAY(map);

            /* Failure */
            return (FALSE);
        }

        /* Reset the map. Try again */
        fractal_map_reset(map, t_ptr);
    }

    /* Get edge information */
    fractal_map_mark_edge(map, t_ptr);

    /* Place the room */
    fractal_map_to_room(map, type, y0, x0);

    /* Free resources */
    FREE_ARRAY(map);

    /* Success */
    return (TRUE);
}


/*
 * Build a pool in a room given the center of the pool and a feature.
 * Outer and solid walls, and permanent features are unnafected.
 * Returns TRUE on success.
 */
static bool build_pool(int y0, int x0, int feat, bool do_big_pool)
{
    byte type;
    int wid, hgt;
    int x, y, x1, y1;
    fractal_map map;
    fractal_template *t_ptr;

    /* Paranoia */
    if (!feat) return (FALSE);

    /* Set some basic info */
    if (do_big_pool)
    {
        type = FRACTAL_TYPE_17x17;
    }
    else
    {
        type = FRACTAL_TYPE_9x9;
    }

    /* Get the dimensions of the fractal map */
    hgt = fractal_dim[type].hgt;
    wid = fractal_dim[type].wid;

    /* Get the top-left grid of the pool */
    y1 = y0 - hgt / 2;
    x1 = x0 - wid / 2;

    /* Choose a template for the pool */
    while (TRUE)
    {
        /* Pick a random template */
        int which = rand_int(N_ELEMENTS(fractal_repository));

        t_ptr = &fractal_repository[which];

        /* Found the desired template type? */
        if (t_ptr->type == type) break;
    }

    /* Create and reset the fractal map */
    map = fractal_map_create(t_ptr);

    /* Complete the map */
    fractal_map_complete(map, t_ptr);

    /* Copy the map into the dungeon */
    for (y = 0; y < hgt; y++)
    {
        for (x = 0; x < wid; x++)
        {
            /* Translate map coordinates to dungeon coordinates */
            int yy = y1 + y;
            int xx = x1 + x;

            /* Ignore non-floors grid types in the map */
            if (map[y][x] != FRACTAL_FLOOR) continue;

            /* Ignore annoying locations */
            if (!in_bounds_fully(yy, xx)) continue;

            /* A pool must be inside a room */
            if (!(dungeon_info[yy][xx].cave_info & (CAVE_ROOM))) continue;

            /* Ignore anti-teleport grids */
            if (dungeon_info[yy][xx].cave_info & (CAVE_ICKY)) continue;

            /* Ignore some walls, and permanent features */
            if (cave_ff1_match(yy, xx, FF1_OUTER |
                FF1_SOLID | FF1_PERMANENT)) continue;

            /* Set the feature */
            build_terrain(yy, xx, feat);
        }
    }

    /* Free resources */
    FREE_ARRAY(map);

    /* Success */
    return (TRUE);
}


/*
 * Mark a starburst shape in the dungeon with the CAVE_TEMP flag, given the
 * coordinates of a section of the dungeon in "box" format. -LM-, -DG-
 *
 * Starburst are made in three steps:
 * 1: Choose a box size-dependant number of arcs.  Large starburts need to
 *    look less granular and alter their shape more often, so they need
 *    more arcs.
 * 2: For each of the arcs, calculate the portion of the full circle it
 *    includes, and its maximum effect range (how far in that direction
 *    we can change features in).  This depends on starburst size, shape, and
 *    the maximum effect range of the previous arc.
 * 3: Use the table "get_angle_to_grid" to supply angles to each grid in
 *    the room.  If the distance to that grid is not greater than the
 *    maximum effect range that applies at that angle, change the feature
 *    if appropriate (this depends on feature type).
 *
 * Usage notes:
 * - This function uses a table that cannot handle distances larger than
 *   20, so it calculates a distance conversion factor for larger starbursts.
 * - This function is not good at handling starbursts much longer along one axis
 *   than the other.
 * This function doesn't mark any grid in the perimeter of the given box.
 *
 */
static bool mark_starburst_shape(int y1, int x1, int y2, int x2, u32b flag)
{
    int y0, x0, y, x, ny, nx;
    int i;
    int size;
    int dist, max_dist, dist_conv, dist_check;
    int height, width, arc_dist;
    int degree_first, center_of_arc, degree;

    /* Special variant starburst.  Discovered by accident. */
    bool make_cloverleaf = FALSE;

    /* Holds first degree of arc, maximum effect distance in arc. */
    int arc[45][2];

    /* Number (max 45) of arcs. */
    int arc_num;

    /* Make certain the starburst does not cross the dungeon edge. */
    if ((!in_bounds(y1, x1)) || (!in_bounds(y2, x2))) return (FALSE);

    /* Robustness -- test sanity of input coordinates. */
    if ((y1 + 2 >= y2) || (x1 + 2 >= x2)) return (FALSE);

    /* Get room height and width. */
    height = 1 + y2 - y1;
    width  = 1 + x2 - x1;

    /* Note the "size" */
    size = 2 + div_round(width + height, 22);

    /* Get a shrinkage ratio for large starbursts, as table is limited. */
    if ((width > 40) || (height > 40))
    {
        if (width > height) dist_conv = 1 + (10 * width  / 40);
        else                dist_conv = 1 + (10 * height / 40);
    }
    else dist_conv = 10;

    /* Make a cloverleaf starburst sometimes.  (discovered by accident) */
    if ((flag & (STAR_BURST_CLOVER)) && (height > 10) && (one_in_(20)))
    {
        arc_num = 12;
        make_cloverleaf = TRUE;
    }

    /* Usually, we make a normal starburst. */
    else
    {
        /* Ask for a reasonable number of arcs. */
        arc_num = 8 + (height * width / 80);
        arc_num = rand_spread(arc_num, 3);
        if (arc_num < 8) arc_num = 8;
        if (arc_num > 45) arc_num = 45;
    }

    /* Get the center of the starburst. */
    y0 = y1 + height / 2;
    x0 = x1 + width  / 2;

    /* Start out at zero degrees. */
    degree_first = 0;

    /* Determine the start degrees and expansion distance for each arc. */
    for (i = 0; i < arc_num; i++)
    {
        /* Get the first degree for this arc (using 180-degree circles). */
        arc[i][0] = degree_first;

        /* Get a slightly randomized start degree for the next arc. */
        degree_first += div_round(180, arc_num);

        /* Do not entirely leave the usual range */
        if (degree_first < 180 * (i+1) / arc_num)
        {
            degree_first = 180 * (i+1) / arc_num;
        }
        if (degree_first > (180 + arc_num) * (i+1) / arc_num)
        {
            degree_first = (180 + arc_num) * (i+1) / arc_num;
        }

        /* Get the center of the arc (convert from 180 to 360 circle). */
        center_of_arc = degree_first + arc[i][0];

        /* Get arc distance from the horizontal (0 and 180 degrees) */
        if      (center_of_arc <=  90) arc_dist = center_of_arc;
        else if (center_of_arc >= 270) arc_dist = ABS(center_of_arc - 360);
        else                           arc_dist = ABS(center_of_arc - 180);

        /* Special case -- Handle cloverleafs */
        if ((arc_dist == 45) && (make_cloverleaf)) dist = 0;

        /*
         * Usual case -- Calculate distance to expand outwards.  Pay more
         * attention to width near the horizontal, more attention to height
         * near the vertical.
         */
        else dist = ((height * arc_dist) + (width * (90 - arc_dist))) / 90;

        /* Randomize distance (should never be greater than radius) */
        arc[i][1] = rand_range(dist / 4, dist / 2);

        /* Keep variability under control (except in special cases). */
        if ((dist != 0) && (i != 0))
        {
            int diff = arc[i][1] - arc[i-1][1];

            if (ABS(diff) > size)
            {
                if (diff > 0)	arc[i][1] = arc[i-1][1] + size;
                else arc[i][1] = arc[i-1][1] - size;
            }
        }
    }

    /* Neaten up final arc of circle by comparing it to the first. */
    if (TRUE)
    {
        int diff = arc[arc_num - 1][1] - arc[0][1];

        if (ABS(diff) > size)
        {
            if (diff > 0)	arc[arc_num - 1][1] = arc[0][1] + size;
            else arc[arc_num - 1][1] = arc[0][1] - size;
        }
    }

    /* Precalculate check distance. */
    dist_check = 21 * dist_conv / 10;

    /* Change grids between (and not including) the edges. */
    for (y = y1 + 1; y < y2; y++)
    {
        for (x = x1 + 1; x < x2; x++)
        {
            /* Get distance to grid. */
            dist = distance(y0, x0, y, x);

            /* Look at the grid if within check distance. */
            if (dist < dist_check)
            {
                /* Convert and reorient grid for table access. */
                ny = 20 + 10 * (y - y0) / dist_conv;
                nx = 20 + 10 * (x - x0) / dist_conv;

                /* Illegal table access is bad. */
                if ((ny < 0) || (ny > 40) || (nx < 0) || (nx > 40))  continue;

                /* Get angle to current grid. */
                degree = get_angle_to_grid[ny][nx];

                /* Scan arcs to find the one that applies here. */
                for (i = arc_num - 1; i >= 0; i--)
                {
                    if (arc[i][0] <= degree)
                    {
                        max_dist = arc[i][1];

                        /* Must be within effect range. */
                        if (max_dist >= dist)
                        {
                            /* Mark the grid */
                            dungeon_info[y][x].cave_info |= (CAVE_TEMP);
                        }

                        /* Arc found.  End search */
                        break;
                    }
                }
            }
        }
    }

    return (TRUE);
}


/*
 * Make a starburst room. -LM-, -DG-
 *
 * Usage notes:
 * - This function is not good at handling rooms much longer along one axis
 *   than the other.
 * - It is safe to call this function on areas that might contain vaults or
 *   pits, because "icky" and occupied grids are left untouched.
 */
bool generate_starburst_room(int y1, int x1, int y2, int x2,
    u16b feat, u16b edge, u32b flag)
{
    int y, x, d;

    /* Mark the affected grids */
    if (!mark_starburst_shape(y1, x1, y2, x2, flag)) return (FALSE);

    /* Paranoia */
    if (edge == feat) edge = FEAT_NONE;

    /* Process marked grids */
    for (y = y1 + 1; y < y2; y++)
    {
        for (x = x1 + 1; x < x2; x++)
        {
            /* Marked grids only */
            if (!(dungeon_info[y][x].cave_info & (CAVE_TEMP))) continue;

            /* Do not touch "icky" grids. */
            if (dungeon_info[y][x].cave_info & (CAVE_ICKY)) continue;

            /* Do not touch occupied grids. */
            if (dungeon_info[y][x].monster_idx != 0) continue;
            if (dungeon_info[y][x].object_idx != 0) continue;

            /* Illuminate if requested */
            if (flag & (STAR_BURST_LIGHT))
            {
                dungeon_info[y][x].cave_info |= (CAVE_GLOW);
            }
            /* Or turn off the lights */
            else
            {
                dungeon_info[y][x].cave_info &= ~(CAVE_GLOW);
            }

            /* Floor overwrites the dungeon */
            if (flag & (STAR_BURST_RAW_FLOOR))
            {
                cave_set_feat(y, x, feat);
            }
            /* Floor is merged with the dungeon */
            else
            {
                build_terrain(y, x, feat);
            }

            /* Make part of a room if requested */
            if (flag & (STAR_BURST_ROOM))
            {
                dungeon_info[y][x].cave_info |= (CAVE_ROOM);
            }

            /* Special case. No edge feature */
            if (edge == FEAT_NONE)
            {
                /*
                 * We lite the outside grids anyway, to
                 * avoid lakes surrounded with blackness.
                 * We only do this if the lake is lit.
                 */
                if (!(flag & (STAR_BURST_LIGHT |
                    STAR_BURST_ROOM))) continue;

                /* Look in all directions. */
                for (d = 0; d < 8; d++)
                {
                    /* Extract adjacent location */
                    int yy = y + ddy_ddd[d];
                    int xx = x + ddx_ddd[d];

                    /* Ignore annoying locations */
                    if (!in_bounds_fully(yy, xx)) continue;

                    /* Already processed */
                    if (dungeon_info[yy][xx].cave_info & (CAVE_TEMP)) continue;

                    /* Lite the feature */
                    if (flag & (STAR_BURST_LIGHT))
                    {
                        dungeon_info[yy][xx].cave_info |= (CAVE_GLOW);
                    }

                    /* Make part of the room */
                    if (flag & (STAR_BURST_ROOM))
                    {
                        dungeon_info[yy][xx].cave_info |= (CAVE_ROOM);
                    }
                }

                /* Done */
                continue;
            }

            /* Common case. We have an edge feature */

            /* Look in all directions. */
            for (d = 0; d < 8; d++)
            {
                /* Extract adjacent location */
                int yy = y + ddy_ddd[d];
                int xx = x + ddx_ddd[d];

                /* Ignore annoying locations */
                if (!in_bounds_fully(yy, xx)) continue;

                /* Already processed */
                if (dungeon_info[yy][xx].cave_info & (CAVE_TEMP)) continue;

                /* Do not touch "icky" grids. */
                if (dungeon_info[yy][xx].cave_info & (CAVE_ICKY)) continue;

                /* Do not touch occupied grids. */
                if (dungeon_info[yy][xx].monster_idx != 0) continue;
                if (dungeon_info[yy][xx].object_idx != 0) continue;

                /* Illuminate if requested. */
                if (flag & (STAR_BURST_LIGHT))
                {
                    dungeon_info[yy][xx].cave_info |= (CAVE_GLOW);
                }

                /* Edge overwrites the dungeon */
                if (flag & (STAR_BURST_RAW_EDGE))
                {
                    cave_set_feat(yy, xx, edge);
                }
                /* Edge is merged with the dungeon */
                else
                {
                    build_terrain(yy, xx, edge);
                }

                /* Make part of a room if requested */
                if (flag & (STAR_BURST_ROOM))
                {
                    dungeon_info[yy][xx].cave_info |= (CAVE_ROOM);
                }
            }
        }
    }

    /* Clear the mark */
    for (y = y1 + 1; y < y2; y++)
    {
        for (x = x1 + 1; x < x2; x++)
        {
            dungeon_info[y][x].cave_info &= ~(CAVE_TEMP);
        }
    }

    /* Success */
    return (TRUE);
}


void build_type_starburst(int y0, int x0, bool giant_room)
{
    bool want_pools = (rand_int(150) < p_ptr->depth);
    /* Default floor and edge */
    u16b feat = FEAT_FLOOR;
    u16b edge = FEAT_WALL_GRANITE;
    /* Default flags, classic rooms */
    u32b flag = (STAR_BURST_ROOM | STAR_BURST_RAW_FLOOR |
        STAR_BURST_RAW_EDGE);

    int dy, dx;

    /*
     * Hack - get the size of the room, could be large or very large.
     */

    /* 66x44 */
    if (giant_room)
    {
        dy = 19;
        dx = 30;
    }
    /* 33x22 */
    else
    {
        dy = 9;
        dx = 14;
    }

    /* We have a second chance to build pools in themed levels */
    if (!want_pools && (feeling >= LEV_THEME_HEAD))
    {
        want_pools = (rand_int(90) < p_ptr->depth);
    }

    /* Occasional light */
    if (p_ptr->depth <= randint(25)) flag |= (STAR_BURST_LIGHT);

    /* Frozen edge on ice levels */
    if (level_flag & (LF1_ICE))
    {
        edge = FEAT_WALL_ICE;

        /* Make ice walls interesting */
        flag &= ~(STAR_BURST_RAW_EDGE);
    }

    /* Case 1. Plain starburst room */
    if (rand_int(100) < 75)
    {
        /* Allow cloverleaf rooms if pools are disabled */
        if (!want_pools) flag |= (STAR_BURST_CLOVER);

        generate_starburst_room (y0 - dy, x0 - dx, y0 + dy, x0 + dx,
            feat, edge, flag);
    }
    /* Case 2. Add an inner room */
    else
    {
        /* Note no cloverleaf room */
        generate_starburst_room (y0 - dy, x0 - dx, y0 + dy, x0 + dx,
            feat, edge, flag);

        /* Special case. Create a solid wall formation */
        if (one_in_(2))
        {
            /* Classic rooms */
            if (edge == FEAT_WALL_GRANITE)
            {
                feat = FEAT_WALL_GRANITE_INNER;
            }

            /* Ice wall formation */
            else
            {
                feat = edge;

                /* Make ice walls interesting */
                flag &= ~(STAR_BURST_RAW_FLOOR);
            }

            /* No edge */
            edge = FEAT_NONE;
        }

        /* Adjust the size of the inner room */
        if (feat_ff1_match(edge, FF1_WALL))
        {
            dy /= 4;
            dx /= 4;
        }
        else
        {
            dy /= 3;
            dx /= 3;
        }

        generate_starburst_room (y0 - dy, x0 - dx, y0 + dy, x0 + dx,
            feat, edge, flag);
    }

    /* Build pools */
    if (want_pools)
    {
        int i, n_pools, range;

        /* Randomize the number of pools */
        n_pools = randint(2);

        /* Adjust for giant rooms */
        if (giant_room) n_pools += 1;

        /* How far of room center? */
        range = giant_room ? 12: 5;

        /* Force the selection of a new feature */
        feat = FEAT_NONE;

        /* Place the pools */
        for (i = 0; i < n_pools; i++)
        {
            int tries;

            /* Pick a new feature */
            if (!feat || one_in_(4))
            {
                /* Choose a feature for the pool */
                feat = pick_proper_feature(cave_feat_pool);

                /* Got none */
                if (!feat) continue;
            }

            for (tries = 0; tries < 2500; tries++)
            {
                /* Get the center of the pool */
                int y = rand_spread(y0, range);
                int x = rand_spread(x0, range);

                /* Verify center */
                if (dungeon_info[y][x].feature_idx == FEAT_FLOOR)
                {
                    build_pool(y, x, feat, giant_room);

                    /* Done */
                    break;
                }
            }
        }
    }
}


/*
 * The center of a fog formation must be close to water/ice. This is the
 * radius of the area we check for water/ice grids.
 */
#define FOG_SRC_RAD 7

/*
 * Build a fog formation on the dungeon
 */
static void build_fog(void)
{
    int x, y, x1, y1;
    int wid, hgt;
    int tries = 0;
    fractal_template *t_ptr;
    fractal_map map;
    /* Check room grids */
    bool want_room = (*dun_cap->can_place_fog_in_rooms)();
    bool locate_near_source = one_in_(2);
    byte type = (one_in_(5) ? FRACTAL_TYPE_33x33: FRACTAL_TYPE_17x17);

    /* Pick a location */
    while (TRUE)
    {
        bool is_near_source = FALSE;
        bool is_room;

        /* Too many failed attempts. Give up */
        if (++tries > 2500) return;

        /* Pick a random spot */
        y = rand_int(p_ptr->cur_map_hgt);
        x = rand_int(p_ptr->cur_map_wid);

        /* Must be passable */
        if (!cave_ff1_match(y, x, FF1_MOVE)) continue;

        /* Check room grids */
        is_room = ((dungeon_info[y][x].cave_info & (CAVE_ROOM)) != 0);

        /* Accept/reject room grids */
        if (want_room != is_room) continue;

        /* There are other restrictions to the center grid? */
        if (!locate_near_source) break;

        /* Yes. Place fog near water or ice grids */
        for (y1 = y - FOG_SRC_RAD; y1 <= y + FOG_SRC_RAD; y1++)
        {
            for (x1 = x - FOG_SRC_RAD; x1 <= x + FOG_SRC_RAD; x1++)
            {
                /* Ignore annoying locations */
                if (!in_bounds(y1, x1)) continue;

                /* Water/ice? */
                if (cave_ff3_match(y1, x1, FF3_WATER | FF3_ICE))
                {
                    /* Done */
                    is_near_source = TRUE;

                    break;
                }
            }

            /* Done? */
            if (is_near_source) break;
        }

        /* Done? */
        if (is_near_source) break;
    }

    /* Pick a fractal template */
    while (TRUE)
    {
        /* Pick any template index */
        int which = rand_int(N_ELEMENTS(fractal_repository));

        /* Get the template */
        t_ptr = &fractal_repository[which];

        /* The size matches the desired type? */
        if (t_ptr->type == type) break;
    }

    /* Create and initialize a fractal map */
    map = fractal_map_create(t_ptr);

    /* Complete the fractal map */
    fractal_map_complete(map, t_ptr);

    /* Get the map size */
    hgt = fractal_dim[type].hgt;
    wid = fractal_dim[type].wid;

    /* Get the top-left corner */
    y1 = y - hgt / 2;
    x1 = x - wid / 2;

    /* Build the fog formation based on the fractal map */
    for (y = 0; y < hgt; y++)
    {
        for (x = 0; x < wid; x++)
        {
            /* Get dungeon coordinates */
            int yy = y1 + y;
            int xx = x1 + x;

            /* The map doesn't specifies a floor grid */
            if (map[y][x] < FRACTAL_FLOOR) continue;

            /* Ignore annoying locations */
            if (!in_bounds(yy, xx)) continue;

            /* Must be passable */
            if (!cave_ff1_match(yy, xx, FF1_MOVE)) continue;

            /* Forbid fog over open doors and stairs */
            if (cave_ff1_match(yy, xx, FF1_DOOR | FF1_STAIRS)) continue;

            /* Create the fog */
            set_effect_permanent_cloud(FEAT_EFFECT_FOG, yy, xx, 0, 0);
        }
    }

    /* Free resources */
    FREE_ARRAY(map);
}


/*
 * Pick a location for the center of a dungeon transformation (region, wall, etc.)
 * The location is stored in py and px.
 * flag must the LF1_* flag of the transformation. It can be 0.
 * marked_grids and num_marked_grids contain the array of grids marked with CAVE_TEMP.
 * These marked grids are the possible candidates for transformation centers.
 * Return TRUE on success, FALSE on failure.
 */
static bool pick_transform_center(QVector<coord> marked_grids, u32b flag, int *py, int *px)
{
    int max = 300;
    int cur = 0;
    coord *grids;
    int i, j, k;
    int x = 0;
    int y = 0;
    bool found = FALSE;
    int rad = MAX_SIGHT * 2;

    /* First, find a random grid of the given element in the dungeon */
    if (flag && level_flag)
    {
        /* Allocate storage for a list of features that match that element */
        grids = C_ZNEW(max, coord);

        /* Scan the dungeon */
        for (y = 0; y < p_ptr->cur_map_hgt; y++)
        {
            for (x = 0; x < p_ptr->cur_map_wid; x++)
            {
                /* Get the feature */
                u16b feat = dungeon_info[y][x].feature_idx;

                /* It must be an elemental feature */
                if (!feat_ff3_match(feat, TERRAIN_MASK)) continue;

                /* It must match the given flag */
                if (get_level_flag(feat) != flag) continue;

                /* Put in on the list */
                if (cur < max)
                {
                    k = cur++;
                }
                /* Overwrite the list if there isn't more space */
                else
                {
                    k = rand_int(max);
                }

                /* Save the data */
                grids[k].y = y;
                grids[k].x = x;
            }
        }

        /* Second. Pick a marked grid that is near to a valid elemental grid */
        if (cur > 0)
        {
            /* Try several times */
            for (i = 0; (i < 50) && !found; i++)
            {
                /* Pick a random elemental grid */
                k = rand_int(cur);

                /* Try several times */
                for (j = 0; (j < 100) && !found; j++)
                {
                    /* Pick a random grid near the elemental grid */
                    y = rand_spread(grids[k].y, rad);
                    x = rand_spread(grids[k].x, rad);

                    /* Check bounds */
                    if (!in_bounds(y, x)) continue;

                    /* It must be marked */
                    if (dungeon_info[y][x].cave_info & (CAVE_TEMP)) found = TRUE;
                }
            }
        }

        /* Free storage */
        FREE_ARRAY(grids);

        /* Found? */
        if (found)
        {
            /* Return that location */
            *py = y;
            *px = x;
            return (TRUE);
        }
    }

    /* Paranoia */
    if (!marked_grids.size())
    {
        return (FALSE);
    }

    /* Default case. Just put it on some random location */
    for (i = 0; i < 100; i++)
    {
        /* Pick a random index */
        k = rand_int(marked_grids.size());

        /* Get coordinates */
        y = marked_grids.at(k).y;
        x = marked_grids.at(k).x;

        /* Found a marked grid? */
        if (dungeon_info[y][x].cave_info & (CAVE_TEMP))
        {
            /* Return the location */
            *py = y;
            *px = x;
            return (TRUE);
        }
    }

    /* Failure */
    return (FALSE);
}

/*
 * Pick and return a random pair of features
 * Return NULL if there isn't a suitable pair
 */
static int feature_selector_select(QVector<feature_selector_item_type> selections)
{
    u16b total_chance = 0;

    for (int i = 0; i < selections.size(); i++)
    {
        total_chance += selections.at(i).chance;
    }

    /* We must have something */
    if (total_chance)
    {
        /* Roll a random chance value */
        int chance = randint0(total_chance);

        int running_chance = 0;

        /* Traverse the array of pairs to see which one contains that value */
        for (int i = 0; i < selections.size(); i++)
        {
            running_chance += selections.at(i).chance;

            /* Found one? */
            if (running_chance > chance)
            {
                /* Return the pair */
                return (i);
            }
        }
    }

    return (-1);
}

/*
 * Transform walls and floors in the dungeon based on the given feature selector.
 * The transformation is fractal shaped and the central point of each fractal is
 * contained in the given array of grids. The grids selected from the array must
 * have assigned the CAVE_TEMP flag too (this enables us to control the location
 * of the transformed regions)
 */
static void transform_regions(QVector<coord> grids, QVector<feature_selector_item_type> selections)
{
    int max = 0, i;
    byte dun_size;
    bool done_big = FALSE;

    /* Paranoia */
    if (!selections.size()) return;

    /* Get dungeon size measure */
    dun_size = ponder_dungeon_size();

    /* Get a number of regions suitable for each size */
    if (dun_size == 1)
    {
        max = 3;
    }
    else if (dun_size == 2)
    {
        max = 5;
    }
    else if (dun_size == 3)
    {
        max = 6;
    }
    /* Medium and large dungeons */
    else
    {
        int k = rand_int(100);

        if (k < 10) max = 10;
        else if (k < 30) max = 9;
        else max = 8;
    }

    /* Message */
    if (cheat_room)
    {
        message(QString("transform_regions: changing %1 region%2.") .arg(max) .arg((max == 1) ? "": "s"));
    }

    /* Transform "max" regions */
    for (i = 0; i < max; i++)
    {
        u16b wall, floor;
        int wid, hgt;
        int y, x, y1, x1;
        fractal_template *t_ptr;
        fractal_map map;
        byte type;
        int tries = 0;
        u32b flags;

        /* Pick a wall feature and an optional floor feature */
        while (TRUE)
        {
            /* Select a feature pair */
            int item = feature_selector_select(selections);

            /* Got one */
            if (item > -1)
            {
                /* Get wall */
                wall = selections.at(item).wall;

                /* Get floor */
                floor = selections.at(item).floor;

                /* Get element flags */
                flags = selections.at(item).level_flag;

                /* Accept feature */
                break;
            }

            /* Can't get a valid pair. Done */
            if (++tries > 50) return;
        }

        /* Pick location */
        if (!pick_transform_center(grids, flags, &y, &x)) return;

        /* Default region size */
        type = FRACTAL_TYPE_33x33;

        /* Try to get a big region */
        if (!done_big && (dun_size >= 4) && one_in_(10))
        {
            type = FRACTAL_TYPE_33x65;

            /* Success */
            done_big = TRUE;
        }

        /* Pick a fractal template */
        while (TRUE)
        {
            /* Pick any template index */
            int which = rand_int(N_ELEMENTS(fractal_repository));

            /* Get the template */
            t_ptr = &fractal_repository[which];

            /* The size matches the desired type? */
            if (t_ptr->type == type) break;
        }

        /* Create and initialize a fractal map */
        map = fractal_map_create(t_ptr);

        /* Complete the fractal map */
        fractal_map_complete(map, t_ptr);

        /* Get the map size */
        hgt = fractal_dim[type].hgt;
        wid = fractal_dim[type].wid;

        /* Get the top-left corner */
        y1 = y - hgt / 2;
        x1 = x - wid / 2;

        /* Transfor the dungeon */
        for (y = 0; y < hgt; y++)
        {
            for (x = 0; x < wid; x++)
            {
                /* Get dungeon coordinates */
                int yy = y1 + y;
                int xx = x1 + x;

                u16b feat;

                /* The map doesn't specifies a floor grid */
                if (map[y][x] < FRACTAL_FLOOR) continue;

                /* Ignore annoying locations */
                if (!in_bounds(yy, xx)) continue;

                /* Remove mark */
                dungeon_info[yy][xx].cave_info &= ~(CAVE_TEMP);

                /* Ignore forbidden locations */
                if (dungeon_info[yy][xx].cave_info & (CAVE_ICKY)) continue;

                /* Get the current feature */
                feat = dungeon_info[yy][xx].feature_idx;

                /* Certain features are forbidden */
                if (feat_ff1_match(feat, FF1_PERMANENT | FF1_DOOR | FF1_STAIRS | FF1_HAS_GOLD)) continue;

                /* Elemental features too */
                if (feat_ff3_match(feat, TERRAIN_MASK)) continue;

                /* Ignore features that contain  objects */
                if (dungeon_info[yy][xx].object_idx) continue;

                /* Replace walls */
                if (feat_ff1_match(feat, FF1_WALL))
                {
                    u16b new_wall = wall;

                    /* Flavor */
                    if ((wall == FEAT_CRACKED_WALL_OVER_LAVA) ||
                        (wall == FEAT_WALL_CRACKED_OVER_BOILING_MUD) ||
                        (wall == FEAT_WALL_CRACKED_OVER_BOILING_WATER) ||
                        (wall == FEAT_CRACKED_WALL_OVER_ACID))
                    {
                        int k = rand_int(100);

                        if (k < 7) new_wall = FEAT_SCORCHED_WALL;

                        else if (k < 10) new_wall = FEAT_WALL_GRANITE_CRACKED;
                    }
                    else if (wall == FEAT_WALL_SANDSTONE)
                    {
                        int k = rand_int(100);

                        if (k < 5) new_wall = FEAT_WALL_GRANITE_CRACKED;
                    }
                    else if (wall == FEAT_WALL_ICE)
                    {
                        int k = rand_int(100);

                        if (k < 10) new_wall = FEAT_WALL_ICE_CRACKED;
                    }
                    else if (wall == FEAT_WALL_ICE_CRACKED)
                    {
                        int k = rand_int(100);

                        if (k < 10) new_wall = FEAT_WALL_ICE;
                    }
                    else if (wall == FEAT_WALL_COAL)
                    {
                        int k = rand_int(100);

                        if (k < 10) new_wall = FEAT_SCORCHED_WALL;

                        else if (k < 17) new_wall = FEAT_WALL_COAL_BURNING;
                    }
                    else if (wall == FEAT_WALL_SHALE)
                    {
                        int k = rand_int(100);

                        if (k < 10) new_wall = FEAT_WALL_COAL;

                        else if (k < 17) new_wall = FEAT_QUARTZ_VEIN;
                    }
                    else if (wall == FEAT_WALL_VINES)
                    {
                        int k = rand_int(100);

                        if (k < 10) new_wall = FEAT_WALL_EARTH;
                    }

                    cave_set_feat(yy, xx, new_wall);
                }
                /* Replace floor if necessary */
                else if ((floor != FEAT_NONE) && feat_ff1_match(feat, FF1_MOVE))
                {
                    u16b new_floor = floor;

                    /* Flavor */
                    if (floor == FEAT_FLOOR_WET)
                    {
                        int k = rand_int(100);

                        if (k < 30) new_floor = FEAT_FLOOR_WATER;
                    }
                    else if (floor == FEAT_FOREST_SOIL)
                    {
                        int k = rand_int(100);

                        if (k < 20) new_floor = FEAT_FLOOR_EARTH;

                        else if (k < 21) new_floor = FEAT_TREE;

                        else if (k < 26) new_floor = FEAT_BUSH;

                        else if (k < 36) new_floor = FEAT_GRASS;
                    }
                    else if (floor == FEAT_FLOOR_SAND)
                    {
                        int k = rand_int(100);

                        if (k < 7) new_floor = FEAT_FLOOR_ROCK;
                    }
                    else if (floor == FEAT_FLOOR_MUD)
                    {
                        int k = rand_int(100);

                        if (k < 15) new_floor = FEAT_FLOOR_MUD;
                    }

                    cave_set_feat(yy, xx, new_floor);
                }
            }
        }

        /* Free resources */
        FREE_ARRAY(map);
    }
}


/*
 * Tranform walls in the dungeon based on the wall features contained in the
 * given feature selector
 * The location of the target grids are taken from the given array
 * The "rad" field of the pairs contained in the feature selector is used
 * to sometimes expand the size of the walls (flavor).
 */
static void transform_walls(QVector<coord> grids, QVector<feature_selector_item_type> selections)
{
    int max, y, x, i;

    int total_chance = 0;

    for (int i = 0; i < selections.size(); i++)
    {
        total_chance += selections.at(i).chance;
    }

    /* Get the number of grids to tranform */

    /* First we get a divisor */
    /* Just one feature */
    if (selections.size() == 1) max = 20;

    /* Rare features */
    else if (total_chance < 150) max = 15;

    /* Regular case */
    else max = 8 - selections.size();

    /* Must have a lower bound */
    if (max < 4) max = 4;

    /* Apply the divisor to the number of grids of the array */
    max = grids.size() / max;

    /* Paranoia */
    if (max < 1) return;

    /* Flavor */
    max += (rand_int(max) / 2);

    /* Transform "max" walls */
    for (i = 0; i < max; i++)
    {
        int yy, xx;
        int rad;
        bool is_effect = FALSE;

        /* Get a wall */
        int item_num = feature_selector_select(selections);

        // Paranoia
        if (item_num == -1) break;

        /* Got none */
        if (selections.at(item_num).wall == FEAT_NONE) continue;

        /* Find out if it is an effect */
        if (feat_ff2_match(selections.at(item_num).wall, FF2_EFFECT)) is_effect = TRUE;

        /* Pick a location */
        if (!pick_transform_center(grids, selections.at(item_num).level_flag, &y, &x)) return;

        /* Get the radius */
        rad = selections.at(item_num).rad;

        /* Flavor for radius equal to 1 */
        if ((selections.at(item_num).rad == 1) && one_in_(2)) rad = 0;

        /* Flavor for radius equal to 2 */
        if (selections.at(item_num).rad == 2)
        {
            int k = rand_int(100);
            if (k < 20) rad = 0;
            if (k < 60) rad = 1;
            else rad = 2;
        }

        /* Tranform the walls based on the calculated radius */
        for (yy = (y - rad); yy <= (y + rad); yy++)
        {
            for (xx = (x - rad); xx <= (x + rad); xx++)
            {
                /* Ignore annoying locations */
                if (!in_bounds(yy, xx)) continue;

                /* The grid  must be marked too */
                if (dungeon_info[yy][xx].cave_info & (CAVE_TEMP))
                {
                    /* Effects on walls */
                    if (is_effect)
                    {
                        set_effect_inscription(selections.at(item_num).wall, yy, xx, SOURCE_EFFECT, 0);
                    }
                    /* Normal walls */
                    else
                    {
                        cave_set_feat(yy, xx, selections.at(item_num).wall);
                    }

                    /* Clear the mark */
                    dungeon_info[yy][xx].cave_info &= ~(CAVE_TEMP);
                }
            }
        }
    }
}


/* Transformation types */
#define TRANSFORM_WALL		1
#define TRANSFORM_REGION	2

/*
 * A table that holds information of elemental features and the proper
 * transformations that must be triggered if they are present in the dungeon
 */
static struct elemental_transformation_info {
    u32b level_flag;/* The LF1_* flag that must be present in the current level */
    byte type;	/* The transformation type (one of the TRANSFORM_* constants) */
    u16b wall;
    u16b floor;
    u16b chance;
    byte rad;
} elemental_transformations[] =
{
    {LF1_ICE, TRANSFORM_REGION, FEAT_WALL_ICE, FEAT_FLOOR_ICE, 200, 0},
    {LF1_ICE, TRANSFORM_REGION, FEAT_WALL_ICE_CRACKED, FEAT_FLOOR_ICE, 200, 0},

    {LF1_WATER, TRANSFORM_REGION, FEAT_WALL_LIMESTONE, FEAT_FLOOR_WET, 150, 0},

    {LF1_LAVA, TRANSFORM_REGION, FEAT_CRACKED_WALL_OVER_LAVA, FEAT_BURNT_SPOT, 200, 0},

    {LF1_SAND, TRANSFORM_REGION, FEAT_WALL_SANDSTONE, FEAT_FLOOR_SAND, 100, 0},

    {LF1_OIL, TRANSFORM_REGION, FEAT_WALL_COAL, FEAT_NONE, 200, 0},
    {LF1_OIL, TRANSFORM_REGION, FEAT_WALL_SHALE, FEAT_NONE, 100, 0},

    {LF1_FOREST, TRANSFORM_WALL, FEAT_PUTRID_FLOWER, FEAT_NONE, 10, 1},
    {LF1_FOREST, TRANSFORM_REGION, FEAT_WALL_VINES, FEAT_FOREST_SOIL, 150, 0},

    {LF1_MUD, TRANSFORM_REGION, FEAT_WALL_EARTH, FEAT_FLOOR_MUD, 100, 0},

    {LF1_BWATER, TRANSFORM_REGION, FEAT_WALL_CRACKED_OVER_BOILING_MUD, FEAT_NONE, 200, 0},

    {LF1_BMUD, TRANSFORM_REGION, FEAT_WALL_CRACKED_OVER_BOILING_WATER, FEAT_NONE, 200, 0},

    {LF1_FIRE, TRANSFORM_REGION, FEAT_SCORCHED_WALL, FEAT_BURNT_SPOT, 100, 0},

    {LF1_ACID, TRANSFORM_REGION, FEAT_CRACKED_WALL_OVER_ACID, FEAT_BURNT_SPOT, 100, 0},

    /* Marks the end of the list */
    {0,0,0,0,0,0}
};


/*
 * Try to add the LF1_* flags of num_rolls terrain features to level_flag.
 */
static void roll_level_flag(int num_rolls)
{
    u16b feat;
    int i;

    /* Try with num_rolls features */
    for (i = 0; i < num_rolls; i++)
    {
        /* Pick a lake feature */
        feat = pick_proper_feature(cave_feat_lake);

        /* Is it an elemental feature? */
        if (feat_ff3_match(feat, TERRAIN_MASK))
        {
            /* Get the element flag */
            u32b flag = get_level_flag(feat);

            /* Debug message */
            if (cheat_room && !(level_flag & flag))
            {

                QString name;


                name = describe_one_level_flag(flag);

                color_message(QString("Adding %1 to level_flag.") .arg(name), TERM_WHITE);
            }

            /* Extend level_flag */
            level_flag |= flag;
        }
    }
}


/*
 * Transforms walls and regions in the dungeon to add more flavor to the game
 */
static void transform_walls_regions(void)
{
    QVector<coord> grids;
    grids.clear();
    int y, x, i;
    u16b feat;
    bool enable_nature = FALSE;

    /* Feature selectors */
    QVector<feature_selector_item_type> wall_selections;
    QVector<feature_selector_item_type> region_selections;
    feature_selector_item_type item_body;
    feature_selector_item_type *item = &item_body;
    wall_selections.clear();
    region_selections.clear();

    /* Ignore wilderness dungeons */
    if (!(*dun_cap->can_be_transformed)()) return;

    /* Flavor */
    if (one_in_(20)) return;

    /* Add glowing walls */
    if ((p_ptr->depth < 50) || !one_in_(4))
    {
        item->level_flag = 0;
        item->wall = FEAT_WALL_ELVISH;
        item->floor = FEAT_NONE;
        item->chance = 100;
        item->rad = 1;

        wall_selections.append(item_body);
    }

    /* Add silent watchers */
    if ((p_ptr->depth < 10) ? FALSE: (p_ptr->depth >= 35) ? one_in_(10): one_in_(20))
    {
        item->level_flag = 0;
        item->wall = FEAT_SILENT_WATCHER;
        item->floor = FEAT_NONE;
        item->chance = 30;
        item->rad = 0;

        wall_selections.append(item_body);
    }

    /* Add inscription effect */
    item->level_flag = 0;
    item->wall = FEAT_WALL_INSCRIPTION;
    item->floor = FEAT_NONE;
    item->chance = 20;
    item->rad = 0;

    wall_selections.append(item_body);

    /* Add teleport walls */
    if ((p_ptr->depth < 5) ? FALSE: (p_ptr->depth >= 40) ? one_in_(2): one_in_(10))
    {
        item->level_flag = 0;
        item->wall = FEAT_ETHEREAL_WALL;
        item->floor = FEAT_NONE;
        item->chance = 30;
        item->rad = 0;

        wall_selections.append(item_body);
    }

    feat = 0;

    /* Count elemental features in the current level */
    for (y = 1; y < (p_ptr->cur_map_hgt - 1); y++)
    {
        for (x = 1; x < (p_ptr->cur_map_wid - 1); x++)
        {
            if (cave_ff3_match(y, x, TERRAIN_MASK)) ++feat;
        }
    }

    /* We are allowed to use elemental features depending on element presence or dungeon depth */
    if ((feat > 15) || (rand_int(200) < p_ptr->depth))
    {
        /* Add more flavor */
        roll_level_flag(level_flag ? 3: 10);

        /* Allow elemental features */
        enable_nature = TRUE;
    }

    /* Traverse the array of elemental transformations */
    for (i = 0; enable_nature; i++)
    {
        /* Get the current elemental transformation */
        struct elemental_transformation_info *et_ptr = &elemental_transformations[i];

        /* The end of the array was reached */
        if (!et_ptr->level_flag) break;

        /* Allow only features compatible with the current level type */
        if (!(level_flag & et_ptr->level_flag)) continue;

        /* Check depth of features */
        if (TRUE)
        {
            int depth = -1;
            int damage = -1;

            /* Check walls */
            if (et_ptr->wall)
            {
                feature_type *f_ptr = &f_info[et_ptr->wall];

                depth = MAX(f_ptr->f_level, depth);

                damage = MAX(f_ptr->dam_non_native, damage);
            }

            /* Check floors */
            if (et_ptr->floor)
            {
                feature_type *f_ptr = &f_info[et_ptr->floor];

                depth = MAX(f_ptr->f_level, depth);

                damage = MAX(f_ptr->dam_non_native, damage);
            }

            /* Feature is OOD */
            if (depth > (p_ptr->depth + 20))
            {
                /* Feature is too dangerous */
                if (damage > (p_ptr->mhp / 2)) continue;

                /* Sometimes we allow this feature */
                if (depth > (p_ptr->depth + 40))
                {
                    if (!one_in_(7)) continue;
                }
                else
                {
                    if (!one_in_(4)) continue;
                }
            }
        }

        /* Create a feature selector item */
        item->level_flag = et_ptr->level_flag;
        item->wall = et_ptr->wall;
        item->floor = et_ptr->floor;
        item->chance = et_ptr->chance;
        item->rad = et_ptr->rad;

        /* Give it to the proper selector based on the transformation type */
        if (et_ptr->type == TRANSFORM_WALL)
        {
            wall_selections.append(item_body);
        }
        else if (et_ptr->type == TRANSFORM_REGION)
        {
            region_selections.append(item_body);
        }
    }

    /* We don't have a single feature pair */
    if (!wall_selections.size() && !region_selections.size()) return;

    /* Collect room walls locations for the transformations */
    for (y = 1; y < (p_ptr->cur_map_hgt - 1); y++)
    {
        for (x = 1; x < (p_ptr->cur_map_wid - 1); x++)
        {
            bool ignore = TRUE;

            /* Must be room grids, and they don't have to be forbidden */
            if ((dungeon_info[y][x].cave_info & (CAVE_ROOM | CAVE_ICKY)) != (CAVE_ROOM)) continue;

            /* Get the current feature */
            feat = dungeon_info[y][x].feature_idx;

            /* Ignore non-walls */
            if (!feat_ff1_match(feat, FF1_WALL)) continue;

            /* Ignore certain wall types */
            if (feat_ff1_match(feat, FF1_PERMANENT | FF1_INNER | FF1_HAS_GOLD)) continue;

            /* Ignore elemental walls */
            if (feat_ff3_match(feat, TERRAIN_MASK)) continue;

            /* They must be outer walls */
            for (i = 0; i < 8; i++)
            {
                int yy = y + ddy_ddd[i];
                int xx = x + ddx_ddd[i];

                /* Ignore walls adjacent to doors and stairs */
                if (cave_ff1_match(yy, xx, FF1_DOOR | FF1_STAIRS))
                {
                    ignore = TRUE;
                    break;
                }

                /* We found a non-room grid. Remember that */
                /* Keep looking for doors or stairs */
                if (!(dungeon_info[yy][xx].cave_info & (CAVE_ROOM))) ignore = FALSE;
            }

            /* Ignore the wall if necessary */
            if (ignore) continue;

            /* Mark the wall */
            dungeon_info[y][x].cave_info |= (CAVE_TEMP);

            /*
             * Remember only some of the valid walls
             * This prevents an excesive concentration of transformed walls
             * in the lower part of the dungeon
             */
            if (one_in_(4))
            {
                /* Save the coordinates */
                grids.append(make_coords(y, x));
            }
        }
    }

    /* Apply the transformations */
    if (grids.size())
    {
        /* To walls */
        if (wall_selections.size()) transform_walls(grids, wall_selections);

        /* To regions */
        if (region_selections.size()) transform_regions(grids, region_selections);
    }

    /* Clear the marks */
    for (y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (x = 0; x < p_ptr->cur_map_wid; x++)
        {
            dungeon_info[y][x].cave_info &= ~(CAVE_TEMP);
        }
    }

    /* Debug message */
    if (cheat_room)
    {
        debug_all_level_flags(level_flag);
    }
}



/* Available size for lakes (in blocks) */
enum
{
    LAKE_DATA_2x2,
    LAKE_DATA_2x3,
    LAKE_DATA_3x3,
    LAKE_DATA_3x4,
    LAKE_DATA_4x4,
    LAKE_DATA_4x5,
    MAX_LAKE_DATA
};

/* Block information for lakes (sorted by size, smaller first) */
static const room_data lake_data[MAX_LAKE_DATA] =
{
    {0, 1, 0, 1, 1},		/* LAKE_DATA_2x2 */
    {0, 1, -1, 1, 1},		/* LAKE_DATA_2x3 */
    {-1, 1, -1, 1, 1},		/* LAKE_DATA_3x3 */
    {-1, 1, -1, 2, 1},		/* LAKE_DATA_3x4 */
    {-1, 2, -1, 2, 1},		/* LAKE_DATA_4x4 */
    {-1, 2, -2, 2, 1},		/* LAKE_DATA_4x5 */
};


/*
 * Build a lake using the given feature.
 * Returns TRUE on success.
 * The coordinates of its center are stored in y0 and x0.
 */
static bool build_lake(int feat, bool do_big_lake, int *y0, int *x0)
{
    int bx0, by0;
    int bx1, by1, bx2, by2;
    int wid, hgt;
    int tries = 0;
    int lake_idx;
    const room_data *ld;
    /* Starburst flags */
    u32b flag = 0;

    /*
     * Notice special cases: these are replaced with passable features
     * sometimes (build_terrain)
     */
    bool solid_lake = (feat_ff1_match(feat, FF1_WALL) &&
        (feat != FEAT_WALL_LIMESTONE) && (feat != FEAT_WALL_COAL));

    /* Solid lakes are made very large sometimes */
    if (solid_lake && one_in_(2)) do_big_lake = TRUE;

    /* Prevent secluded regions in the dungeon at shallow depths */
    if (!solid_lake && (p_ptr->depth <= 10)) do_big_lake = FALSE;

    /* Choose an initial size for the lake */
    if (do_big_lake)
    {
        if (one_in_(10)) lake_idx = LAKE_DATA_4x5;
        else if (one_in_(5)) lake_idx = LAKE_DATA_4x4;
        else lake_idx = LAKE_DATA_3x4;
    }
    else
    {
        /*
         * Lakes at shallow levels are smaller, and some at deeper
         * levels too
         */
        if ((p_ptr->depth >= 25) && one_in_(7))
        {
            lake_idx = LAKE_DATA_3x4;
        }
        else
        {
            lake_idx = LAKE_DATA_2x3;
        }
    }

    /* Adjust the size of the lake, if needed */
    while (TRUE)
    {
        /* Get block information for this kind of lake */
        ld = &lake_data[lake_idx];

        /* Get the size of the lake in blocks */
        hgt = ld->dy2 - ld->dy1 + 1;
        wid = ld->dx2 - ld->dx1 + 1;

        /* Can be placed in this dungeon? */
        if ((hgt <= dun->row_rooms) && (wid <= dun->col_rooms)) break;

        /* Try again with a smaller lake */
        --lake_idx;

        /* Level too small, give up */
        if (lake_idx < 0)
        {
            if (cheat_room)
            {
                color_message(QString("Can't place lakes in this dungeon!"), TERM_WHITE);
            }

            return (FALSE);
        }
    }

    /* Try to get a location for the lake */
    while (TRUE)
    {
        /* Too many tries. Reject lake */
        if (++tries >= 200)
        {
            if (cheat_room)
            {
                color_message(QString("Can't find a blocks for lakes in this dungeon!"), TERM_WHITE);
            }

            return (FALSE);
        }

        /* Get central block */
        by0 = rand_int(dun->row_rooms);
        bx0 = rand_int(dun->col_rooms);

        /* Get the blocks */
        by1 = by0 + ld->dy1;
        by2 = by0 + ld->dy2;
        bx1 = bx0 + ld->dx1;
        bx2 = bx0 + ld->dx2;

        /* Ignore blocks outside the dungeon */
        if ((by1 < 0) || (by2 >= dun->row_rooms)) continue;
        if ((bx1 < 0) || (bx2 >= dun->col_rooms)) continue;

        /* Found a suitable location */
        break;
    }

    /* Get total height and width of the available space */
    hgt *= BLOCK_HGT;
    wid *= BLOCK_WID;

    /* Get the center of the lake */
    *y0 = by1 * BLOCK_HGT + hgt / 2;
    *x0 = bx1 * BLOCK_WID + wid / 2;

    /* Store extra information for passable lakes */
    if (!solid_lake)
    {
        /* Forests are always lit. Others not so much */
        if (feat_ff3_match(feat, FF3_FOREST) ||
            (p_ptr->depth <= randint(25)))
        {
            flag |= (STAR_BURST_LIGHT);
        }

        /* Lakes are rooms now */
        flag |= (STAR_BURST_ROOM);

        /* Connect the lake with the dungeon */
        if (dun->cent_n < CENT_MAX)
        {
            dun->cent[dun->cent_n].y = *y0;
            dun->cent[dun->cent_n].x = *x0;
            dun->cent_n++;
        }

        /* We won't build rooms over small lakes */
        if (!do_big_lake)
        {
            int bx, by;

            for (by = by1; by <= by2; by++)
            {
                for (bx = bx1; bx <= bx2; bx++)
                {
                    /* Mark the blocks as used */
                    dun->room_map[by][bx] = TRUE;
                }
            }
        }
    }

    /*
     * Convenience. Get the distance from the center to the borders.
     * Note that we substract some space to place tunnels later and to
     * avoid dungeon permanent boundry
     */
    hgt = (hgt - 4) / 2;
    wid = (wid - 4) / 2;

    /* Place the lake */
    generate_starburst_room(*y0 - hgt, *x0 - wid, *y0 + hgt, *x0 + wid,
        feat, f_info[feat].f_edge, flag);

    /* Success */
    return (TRUE);
}


/*
 * Build a river given a feature and its starting location
 */
static void build_river(int feat, int y, int x)
{
    /*
     * This map contains the directions of the grids who must be converted
     * to edge, given a compass direction [0-3]
     */
    static byte edge_map[][3] =
    {
        {1, 2, 3},
        {7, 8, 9},
        {3, 6, 9},
        {1, 4, 7}
    };

    int i, dir, old_dir;
    int old_feat;
    int edge = f_info[feat].f_edge;
    /*
     * Notice special cases: they are replaced by passable features
     * sometimes (build_terrain)
     */
    bool solid_river = (feat_ff1_match(feat, FF1_WALL) &&
        (feat != FEAT_WALL_LIMESTONE) && (feat != FEAT_WALL_COAL));

    /* Choose a random compass direction */
    dir = old_dir = rand_int(4);

    /* Place river into dungeon */
    while (TRUE)
    {
        /* Stop at dungeon edge */
        if (!in_bounds_fully(y, x)) break;

        /* Get the previous content of the grid */
        old_feat = dungeon_info[y][x].feature_idx;

        /* Stop at permanent feature */
        if (feat_ff1_match(old_feat, FF1_PERMANENT)) break;

        /* Most rivers aren't pierced by rooms. */
        if (!solid_river)
        {
            /* Forbid rooms here */
            int by = y / BLOCK_HGT;
            int bx = x / BLOCK_WID;

            dun->room_map[by][bx] = TRUE;
        }

        /* Place a piece of the river, if needed */
        if (feat != old_feat) build_terrain(y, x, feat);

        /* Place river edge, if needed */
        if (edge != FEAT_NONE)
        {
            for (i = 0; i < 3; i++)
            {
                /* Use the map to modify only grids ahead */
                int yy = y + ddy[edge_map[dir][i]];
                int xx = x + ddx[edge_map[dir][i]];

                /* Ignore annoying locations */
                if (!in_bounds_fully(yy, xx)) continue;

                /* Get the previous content of the grid */
                old_feat = dungeon_info[yy][xx].feature_idx;

                /* Avoid permanent features */
                if (feat_ff1_match(old_feat, FF1_PERMANENT)) continue;

                /* IMPORTANT: Don't overwrite the river */
                if (old_feat == feat) continue;

                /* Place river edge, if needed */
                if (edge != old_feat) build_terrain(yy, xx, edge);
            }
        }

        /* Stagger the river */
        if (one_in_(2))
        {
            dir = rand_int(4);
        }
        /* Advance the streamer using the original direction */
        else
        {
            dir = old_dir;
        }

        /* Get the next coordinates */
        y += ddy_ddd[dir];
        x += ddx_ddd[dir];
    }
}


/*
 * Place lakes and rivers given a feature
 */
static bool build_feature(int feat, bool do_big_lake)
{
    /* No coordinates yet */
    int x0 = 0, y0 = 0;

    /* Build a lake? */
    if (feat_ff2_match(feat, FF2_LAKE) ||
        !feat_ff2_match(feat, FF2_RIVER))
    {
        /* Try to place the lake. Get its center */
        if (!build_lake(feat, do_big_lake, &y0, &x0)) return (FALSE);
    }

    /* Build a river */
    if (feat_ff2_match(feat, FF2_RIVER) && (ponder_dungeon_size() > 2))
    {
        /* Pick starting coordinates, if needed */
        if ((y0 + x0) == 0)
        {
            y0 = randint(p_ptr->cur_map_hgt - 2);
            x0 = randint(p_ptr->cur_map_wid - 2);
        }

        /* Generate the river */
        build_river(feat, y0, x0);
    }

    /* Success */
    return (TRUE);
}

/*
 * This table holds aditional flags for themed levels
 * These flags are used to forbid the generation of certain features BEFORE
 * placing any lakes or pools.
 * Example: you can assign LF1_LAVA to a red dragon level to avoid the
 * generation of ice, acid, oil, etc.
 * See "build_themed_level_nature"
 */
static struct
{
    byte theme;
    u32b flags;		/* A combination of the LF1_* flags */
} themed_level_flags[] =
{
    {LEV_THEME_DEMON_MINOR,	LF1_FIRE},
    {LEV_THEME_DEMON_ALL,	LF1_FIRE},
    {LEV_THEME_DEMON_MAJOR,	LF1_LAVA | LF1_FIRE},
    {LEV_THEME_DRAGON_FIRE,	LF1_FIRE},
    {LEV_THEME_DRAGON_ACID,	LF1_ACID},
    {LEV_THEME_DRAGON_ELEC,	LF1_WATER},
    {LEV_THEME_DRAGON_COLD,	LF1_ICE},
    {LEV_THEME_UNDEAD,		LF1_ICE},
    /* Add entries for more themed levels if needed */
};

/*
 * Build lakes and rivers for the dungeon
 */
void build_nature(void)
{
    int i;
    bool big;
    QString name;

    int feat, dev_feat = FEAT_NONE;
    int count = 0, max_features;

    /* Get the maximum number of features based on level size */
    byte dun_size = ponder_dungeon_size();

     /* Clear the level's restriction */
     level_flag = 0;

    /* No NPP terrains option turned on */
    if (birth_classic_dungeons) return;

    /* Debug message */
    if (cheat_room)
    {
        color_message(QString("Dungeon size: %1.") .arg((int)dun_size), TERM_WHITE);
    }

    if (dun_size == 1) max_features = (one_in_(4) ? 1: 0);

    else if (dun_size == 2) max_features = 1;

    else if (dun_size == 3) max_features = (one_in_(3) ? 2: 1);

    else max_features = DUN_MAX_LAKES;

    /* Check quests for specific element flags */
    for (i = 0; i < z_info->q_max; i++)
    {
        /* Get the quest */
        quest_type *q_ptr = &q_info[i];

        /* Active quest? */
        if ((q_ptr->base_level == p_ptr->depth) && !is_quest_complete(i)) continue;

        /* Monster quests */
        if ((quest_fixed(q_ptr)) || (quest_single_r_idx(q_ptr)))
        {
            /* Restrict feature generation */
            level_flag |= get_level_flag_from_race(&r_info[q_ptr->mon_idx]);
        }
        /* themed quests */
        else if (quest_themed(q_ptr))
        {
            u16b j;

            /* Find specific element flags for the theme */
            for (j = 0; j < N_ELEMENTS(themed_level_flags); j++)
            {
                /* Ignore other themes */
                if (themed_level_flags[j].theme != q_ptr->q_theme) continue;

                /* Restrict feature generation */
                level_flag |= themed_level_flags[j].flags;

                /* Done */
                break;
            }
        }
    }

    /* The chance to get special terrain varies with dungeon depth */
    if ((level_flag == 0) && (rand_int(300) >= p_ptr->depth)) return;

    /* Debug message */
    if (level_flag && cheat_room)
    {
        color_message(QString("Level flags added by quests."), TERM_WHITE);
        debug_all_level_flags(level_flag);
    }

    /* Allocate some lakes and rivers */
    for (count = 0; count < max_features; count++)
    {
        /* Very small levels always get a feature -DG- */
        if ((max_features > 1) && (rand_int(100) < 20))
        {
            continue;
        }

        /* Developer, are you testing features? */
        if (dev_feat)
        {
            feat = dev_feat;

            dev_feat = FEAT_NONE;
        }
        /* NPP releases only need this "else" block */
        else
        {
            /* Pick a feature */
            feat = pick_proper_feature(cave_feat_lake);
        }

        /* Got a valid feature? */
        if (feat)
        {
            /* Try a big lake */
            if ((dun_size <= 3) || (randint(150) < p_ptr->depth))
            {
                big = TRUE;
            }
            else
            {
                big = FALSE;
            }

            /* Report creation of lakes/rivers */
            if (cheat_room)
            {
                name = feature_desc(feat, FALSE, FALSE);

                if (f_info[feat].f_edge)
                {
                    QString edge = feature_desc(f_info[feat].f_edge, FALSE, FALSE);

                    message(QString("Building %1%2 surrounded by %3.") .arg(big ? "big ": "") .arg(name) .arg(edge));
                }
                else
                {
                    message(QString("Building %1%2.") .arg(big ? "big ": ""). arg(name));
                }
            }

            /* Build one lake/river. */
            build_feature(feat, big);
        }
    }

    /* Debug message */
    if (level_flag && cheat_room)
    {
        color_message(QString("Level flags added by lake generation."), TERM_WHITE);
        debug_all_level_flags(level_flag);
    }
}


/*
 * Build lakes and other terrain features for the given themed level
 */
static void build_themed_level_nature(byte theme)
{
    u16b i;

    /* Clear the level flag */
    level_flag = 0;

    /* No NPP terrains option turned on */
    if (birth_classic_dungeons) return;

    /* Find if the theme has some restrictions to generate terrain */
    for (i = 0; i < N_ELEMENTS(themed_level_flags); i++)
    {
        /* Found the theme? */
        if (theme == themed_level_flags[i].theme)
        {
            /* Apply the restriction */
            level_flag |= themed_level_flags[i].flags;

            /* Done */
            break;
        }
    }

    /* TODO: add lakes */
}



/*
 * Select a monster type for a themed level
 */
byte get_level_theme(s16b orig_theme_num, bool quest_level)
{
    s16b mindepth, theme_depth;

    s16b theme_num = orig_theme_num;

    /*enforce minimum depth, to keep weak nests out of deep levels*/
    mindepth = theme_num / 4;

    /*keep total to 100 or less*/
    if ((mindepth + theme_num) > 100) theme_num = 100 - mindepth;

    /* Hack -- Choose a nest type */
    theme_depth = randint(theme_num) + mindepth;

    if ((theme_depth <= 20) && (orig_theme_num <= 28))
    {
        /*Coins, minor demons, Orcs, or a mixture of a couple monsters*/
        if (one_in_(4))			return (LEV_THEME_ORC_NAGA_YEEK_KOBOLD);
        else if (one_in_(3))	return (LEV_THEME_DEMON_MINOR);
        else if (one_in_(2))	return (LEV_THEME_CREEPING_COIN);
        else					return (LEV_THEME_ORC);
    }

    else if ((theme_depth <= 35)  && (orig_theme_num <= 40))
    {
        /*Trolls or Ogres, or a mixture of cave dwellers*/
        if (one_in_(3))			return (LEV_THEME_CAVE_DWELLER);
        else if (one_in_(2))	return (LEV_THEME_TROLL);
        else					return (LEV_THEME_OGRE);
    }

    else if ((theme_depth <= 50) && (orig_theme_num <= 60))
    {
        /* Hounds, demon, servants of valar, or young dragon*/
        if (one_in_(4))			return (LEV_THEME_VALAR_SERVANTS);
        if (one_in_(3))			return (LEV_THEME_DEMON_ALL);
        else if (one_in_(2))	return (LEV_THEME_HOUND);
        else					return (LEV_THEME_DRAGON_YOUNG);
    }

    /* Giant, animal, servants of valar, or humanoid pit */
    else if ((theme_depth <= 60) && (orig_theme_num <= 80))
    {
        if (one_in_(5))			return (LEV_THEME_VALAR_SERVANTS);
        else if (one_in_(4))	return (LEV_THEME_HUMANOID);
        else if (one_in_(3))	return (LEV_THEME_UNDEAD);
        else if (one_in_(2))	return (LEV_THEME_GIANT);
        else 					return (LEV_THEME_ANIMAL);
    }

    /* Dragon pit */
    else if (theme_depth <= 75)
    {
        /* Don't do specific elements for quest levels.  Too easy for those with the right immunity. */
        if (quest_level) return (LEV_THEME_DRAGON_ELEMENTAL);

        /* Pick dragon type */
        switch (rand_int(6))
        {
            /* Black */
            case 0:	return (LEV_THEME_DRAGON_ACID);
            /* Blue */
            case 1:	return (LEV_THEME_DRAGON_ELEC);
            /* Red */
            case 2:	return (LEV_THEME_DRAGON_FIRE);
            /* White */
            case 3:	return (LEV_THEME_DRAGON_COLD);
            /* Green */
            case 4:	return (LEV_THEME_DRAGON_POIS);
            /* Chromatic */
            default:return (LEV_THEME_DRAGON_CHROMATIC);
        }
    }

    else
    {
        if (one_in_(3))		return (LEV_THEME_VALAR_SERVANTS);

        /* Ancient Dragon pit */
        else if one_in_(2)	return (LEV_THEME_DRAGON_ANCIENT);

        /* > 90 - Demon pit */
        else			return (LEV_THEME_DEMON_MAJOR);
    }
}


/*
 *Helper function for max number of creatures on a themed level.
 This function is for non-uniques only.
 */
byte max_themed_monsters(const monster_race *r_ptr, u32b max_power)
{
    /*first off, handle uniques*/
    if (r_ptr->flags1 & RF1_UNIQUE) return (r_ptr->max_num);

    /*don't allow 99 of the out of depth monsters*/
    if (r_ptr->level > p_ptr->depth + 3)
    {
        int lev_ood = p_ptr->depth - r_ptr->level;

        /*Too strong*/
        if (r_ptr->mon_power > max_power) return (0);

        else if (lev_ood > 5) return (MON_RARE_FREQ);
        else return (MON_LESS_FREQ);
    }
    else if ((r_ptr->level < p_ptr->depth - 5) && (r_ptr->level < 75))
    {
        int lev_ood = p_ptr->depth - r_ptr->level;

        /*Too weak*/
        if (r_ptr->mon_power < max_power / 20) return (0);

        else if (r_ptr->mon_power < max_power / 10) return (MON_RARE_FREQ);
        else if (lev_ood > 5) return (MON_LESS_FREQ);
    }
    /*The rigth depth*/
    return (r_ptr->max_num);
}




/*
 * Generate a themed dungeon level
 *
 * Note that "dun_body" adds about 4000 bytes of memory to the stack.
 */
bool build_themed_level(void)
{
    int i;
    int r_idx;
    monster_race *r_ptr;
    int by, bx;
    byte is_quest_level = FALSE;
    long value;
    int total = 0;
    int max_depth;
    u32b max_diff, max_diff_unique;
    u32b highest_power = 0;
    int total_mon_placed = 0;

    int max_uniques = 3;

    alloc_entry *table = alloc_race_table;

    quest_type *q_ptr = &q_info[GUILD_QUEST_SLOT];

    byte level_theme;

    dun_data dun_body;

    u16b monster_number, potential_monsters;

    /* Global data */
    dun = &dun_body;

    /* Set the level type */
    set_dungeon_type(DUNGEON_TYPE_THEMED_LEVEL);

    /*check if we need a quest*/
    if (quest_check(p_ptr->depth) == QUEST_THEMED_LEVEL) is_quest_level = TRUE;

    /* Always a dungeon of 5 * 10 blocks*/
    p_ptr->cur_map_hgt = MAX_DUNGEON_HGT * 5 / 6;
    p_ptr->cur_map_wid = MAX_DUNGEON_WID * 5 / 9;

    /* Hack -- Start with basic granite */
    basic_granite();

    /* Actual maximum number of rooms on this level */
    dun->row_rooms = p_ptr->cur_map_hgt / BLOCK_HGT;
    dun->col_rooms = p_ptr->cur_map_wid / BLOCK_WID;

    /* Initialize the room table */
    for (by = 0; by < dun->row_rooms; by++)
    {
        for (bx = 0; bx < dun->col_rooms; bx++)
        {
            dun->room_map[by][bx] = FALSE;
        }
    }

    /* No rooms yet */
    dun->cent_n = 0;

    /* Select the monster theme, get the feeling */
    if (is_quest_level) level_theme = q_ptr->q_theme;
    else level_theme = get_level_theme(p_ptr->depth, FALSE);

    /* Insert the feeling now */
    feeling = level_theme + LEV_THEME_HEAD;

    /* Setup special features */
    build_themed_level_nature(level_theme);

    /* Build some rooms, note starburst rooms fail alot*/
    for (i = 0; i < DUN_ROOMS * 2; i++)
    {
        /* Pick a block for the room */
        by = rand_int(dun->row_rooms);
        bx = rand_int(dun->col_rooms);

        /*make most% of the rooms starburst rooms*/
        if (i < DUN_ROOMS)
        {
            /*make the first 33% of  attempts at an unusually large room*/
            bool large_room = ((dun->cent_n < 1) ? 11 : 10);

            if (!room_build(by, bx, large_room)) continue;
        }

        /* or else Attempt a trivial room */
        else if (room_build(by, bx, 1)) continue;
    }

    /*start over on all themed levels with less than 4 rooms due to inevitable crash*/
    if (dun->cent_n < 4)
    {
        if (cheat_room)
        {
            message(QString("not enough rooms"));
        }
        return (FALSE);
    }

    /*set the permanent walls*/
    set_perm_boundry();

    /*make the tunnels*/
    if (!scramble_and_connect_rooms_stairs())
    {
        if (cheat_room)
        {
            message(QString("unable to scramble and connect rooms"));
        }

        return (FALSE);
    }

    if (!place_traps_rubble_player()) return (FALSE);

    /*note we are going to have an extremely crowded dungeon*/
    /*there is no need for objects on the floor -JG*/

    /*get the hook*/
    get_mon_hook(level_theme);

    /* Prepare allocation table */
    get_mon_num_prep();

    /* Monsters can be up to 7 levels out of depth, 10 for a quest */
    max_depth = p_ptr->depth + (is_quest_level ? THEMED_LEVEL_QUEST_BOOST : THEMED_LEVEL_NO_QUEST_BOOST);

    /*don't make it too easy if the player isn't diving very fast*/
    if (p_ptr->depth < p_ptr->max_lev)
    {
        max_depth += ((p_ptr->max_lev - p_ptr->depth) / 2);
    }

    /* Undead themed levels are just too hard */
    if (level_theme == LEV_THEME_UNDEAD) max_depth -= 2;

    /*boundry control*/
    if (max_depth > (MAX_DEPTH - 15)) max_depth = MAX_DEPTH - 15;

    /*get the average difficulty spanning 5 levels for monsters*/
    max_diff = max_diff_unique = 0;

    /*first get the total of the 5 levels*/
    for (i = 0; i < 5; i++)
    {
        /*put some boundry control on the highest level*/
        max_diff += mon_power_ave[max_depth + i][CREATURE_NON_UNIQUE];
        max_diff_unique += mon_power_ave[max_depth + i][CREATURE_UNIQUE];
    }

    /*now get the average*/
    max_diff /= 5;
    max_diff_unique /= 5;

    /* Quest levels are a little more crowded than non-quest levels*/
    if (is_quest_level) monster_number = QUEST_THEMED_LEVEL_NUM;
    else monster_number = 250;

    /* Undead themed levels are just too hard */
    if (level_theme == LEV_THEME_UNDEAD) monster_number /= 2;

    /* Reduce the number as monsters get more powerful*/
    monster_number -= ((p_ptr->depth / 3) + randint(p_ptr->depth / 3));

    /*boundry control*/
    if (monster_number > (z_info->m_max	- 25)) monster_number = z_info->m_max - 25;

    /*start the counter for potential monsters*/
    potential_monsters = 0;

    /*
     * Process the probabilities, starting from the back forward
     */
    for (i = alloc_race_size - 1; i >= 0; i--)
    {
        /* Default */
        table[i].prob3 = 0;

        /* Monster is not a part of this theme*/
        if (table[i].prob2 == 0) continue;

        /* Get the "r_idx" of the chosen monster */
        r_idx = table[i].index;

        /* Get the actual race */
        r_ptr = &r_info[r_idx];

        /*enforce a maximum depth*/
        if (r_ptr->level > max_depth) continue;

        /*no player ghosts*/
        if (r_ptr->flags2 & (RF2_PLAYER_GHOST)) continue;

        /* Uniques only for unique quests*/
        if (r_ptr->flags1 & (RF1_UNIQUE))
        {
            /*get the right difficulty*/
            if (r_ptr->mon_power > max_diff_unique) continue;

            /* no dead ones*/
            if (r_ptr->cur_num >= r_ptr->max_num) continue;
        }
        /*other monsters based on difficulty*/
        else
        {
            /*get the right difficulty*/
            if (r_ptr->mon_power > max_diff) continue;
        }

        /*hack - no town monsters*/
        if (table[i].level <= 0) continue;

        /* Depth Monsters never appear in quests*/
        if (r_ptr->flags1 & (RF1_FORCE_DEPTH)) continue;

        /* Accept the monster*/
        table[i].prob3 = table[i].prob2;

        /*limit the probability of weaker or stronger monsters*/
        if (!(r_ptr->flags1 & (RF1_UNIQUE)))
        {
            byte num;

            /*once we have enough monsters, start limiting the weaker ones*/
            if (potential_monsters > (monster_number * 3)) num = max_themed_monsters(r_ptr, max_diff);
            else num = r_ptr->max_num;

            /*reduce the probability*/
            if (num == MON_RARE_FREQ) table[i].prob3 /= 10;
            else if (num == MON_LESS_FREQ) table[i].prob3 /= 3;
            else if (!num) table[i].prob3 = 0;

            potential_monsters += num;
        }

        /*but always allow uniques*/
        else potential_monsters += 1;

        /* Total */
        total += table[i].prob3;

        /*record the most powerful monster*/
        if (r_ptr->mon_power > highest_power) highest_power = r_ptr->mon_power;
    }

    /*no eligible creatures - should never happen*/
    if (!total)
    {
        /* Remove restriction */
        get_mon_num_hook = NULL;

        /* Prepare allocation table */
        get_mon_num_prep();

        /* No monsters - no themed level */
        return (FALSE);
    }

    /*place the monsters in the dungeon*/
    while (total_mon_placed < monster_number)
    {
        int y, x;
        bool dont_use = FALSE;
        int tries = 0;

        /* Pick a monster */
        value = rand_int(total);

        /* Find the monster */
        for (i = 0; i < alloc_race_size; i++)
        {
            /* Found the entry */
            if (value < table[i].prob3) break;

            /* Decrement */
            value = value - table[i].prob3;
        }

        /* Get the "r_idx" of the chosen monster */
        r_idx = table[i].index;

        /* Get the actual race */
        r_ptr = &r_info[r_idx];

        /*don't attempt to re-place duplicate unique entries*/
        if (r_ptr->cur_num >= r_ptr->max_num) dont_use = TRUE;

        /*No more of this type of monster on the level, the monster type has been restricted*/
        if ((table[i].prob3 < table[i].prob2)  &&
            (r_ptr->cur_num >= max_themed_monsters(r_ptr, max_diff)))
        {
            dont_use = TRUE;
        }

        /*not a monster*/
        if (!r_idx) dont_use = TRUE;

        /* Limit the number of uniques per level */
        if ((r_ptr->flags1 & (RF1_UNIQUE)) && (max_uniques < 1)) dont_use = TRUE;

        if (dont_use)
        {
            /*Take out the weakest monster and repeat*/
            total -= table[i].prob3;
            table[i].prob3 = 0;

            /*we have maxed out all possible types of monsters*/
            if (total < 1) break;

            /*go back to the beginning*/
            continue;
        }

        /* Pick a location */
        while (TRUE)
        {
            int dist;

            /* Can't find a location for this monster */
            if (++tries >= 2500)
            {
                /* Remove restriction */
                get_mon_num_hook = NULL;

                /* Prepare allocation table */
                get_mon_num_prep();

                /* No themed level */
                return (FALSE);
            }

            y = rand_int(p_ptr->cur_map_hgt);
            x = rand_int(p_ptr->cur_map_wid);

            if (!cave_empty_bold(y,x)) continue;

            /*better distance*/
            dist = distance(p_ptr->py, p_ptr->px, y, x);

            /*hack - too close*/
            if (dist < 5) continue;

            /*give the player a chance - no monsters starting in LOS*/
            if (dist < MAX_RANGE)
            {
                if (los(y, x, p_ptr->py, p_ptr->px)) continue;
            }

            /*found a spot*/
            break;
        }

        /* Attempt to place the monster, allow sleeping, don't allow groups*/
        if (!place_monster_aux(y, x, r_idx, MPLACE_SLEEP)) continue;

        /* Count it */
        total_mon_placed++;

        if (r_ptr->flags1 & (RF1_UNIQUE)) max_uniques--;

        /* Mark 1 in 17 monsters for a bonus item */
        if (dungeon_info[y][x].monster_idx > 0)
        {
            monster_type *m_ptr = &mon_list[dungeon_info[y][x].monster_idx];

            if ((mon_cnt % 15) == 0) m_ptr->mflag |= (MFLAG_BONUS_ITEM);
        }
    }

    /*final preps if this is a quest level*/
    if (is_quest_level)
    {
        q_ptr->q_num_killed = 0;
        q_ptr->q_max_num = 0;

        /*
         * Go through every monster, and mark them as a questor,
         * then make them slightly faster, and light sleepers
         */
        /* Process the monsters */
        for (i = 1; i < mon_max; i++)
        {
            monster_type *m_ptr = &mon_list[i];
            monster_race *r_ptr;

            /* Ignore non-existant monsters */
            if (!m_ptr->r_idx) continue;

            r_ptr = &r_info[m_ptr->r_idx];

            /*mark it as a quest monster*/
            m_ptr->mflag |= (MFLAG_QUEST);

            if (!(r_ptr->flags1 & RF1_UNIQUE))
            {
                m_ptr->mflag &= ~(MFLAG_SLOWER);
                m_ptr->mflag |= (MFLAG_FASTER);
                calc_monster_speed(m_ptr->fy, m_ptr->fx);
            }

            /*increase the max_num counter*/
            q_ptr->q_max_num ++;

            /*Not many of them sleeping, others lightly sleeping*/
            if (one_in_(2)) m_ptr->m_timed[MON_TMD_SLEEP] = 0;
            else m_ptr->m_timed[MON_TMD_SLEEP] /= 2;
        }

        /* Process the mimic objects */
        for (i = 1; i < o_max; i++)
        {
            object_type *o_ptr = &o_list[i];

            /* Skip dead objects */
            if (!o_ptr->k_idx) continue;

            if (!o_ptr->is_mimic()) continue;

            /* Mark it as a questor */
            o_ptr->ident |= (IDENT_QUEST);

            /*increase the max_num counter*/
            q_ptr->q_max_num ++;
        }
    }

    /* Remove restriction */
    get_mon_num_hook = NULL;

    /* Prepare allocation table */
    get_mon_num_prep();

    /* Try hard to place this level */
    rating += 25;

    return (TRUE);
}



/*
 * Place some miscellaneous features on dungeon.
 * Use this when dungeon generation is finished.
 */
void build_misc_features(void)
{
    /* Moria dungeons are simple */
    if (game_mode == GAME_NPPMORIA) return;

    /* Sometimes we place fog on water and ice levels */
    if ((level_flag & (LF1_WATER | LF1_ICE)) && !(level_flag & (LF1_LAVA | LF1_FIRE)) &&
        (p_ptr->depth >= 35) && one_in_(3))
    {
        int i, k = 2;

        /* One fog formation more if the levels is big enough */
        if (dun->cent_n >= 7) ++k;

        /* Sometimes two formations more */
        if ((dun->cent_n >= 10) && one_in_(2)) ++k;

        /* Sometimes three formations more */
        if ((dun->cent_n >= 12) && one_in_(3)) ++k;

        /* Build the fog formations */
        for (i = 0; i < k; i++)
        {
            /* Build */
            build_fog();
        }
    }

    /* More flavor! */
    if (!birth_classic_dungeons) transform_walls_regions();
}


/*
 * Build a fractal formation in the current level using the given feature.
 * y0 and x0 are the center of the formation, if they are out of bounds another point
 * is picked randomly
 * fractal_type tells us the dimensions of the formation
 * Chance is the probability (in 100) to use feat2 for a grid.
 * If chance is 0 feat2 is ignored
 * If feat2 is FEAT_NONE the grid remains untouched
 * if mode is 0 the inner part of the fractal is used
 * if mode is 1 the edge of the fractal is used
 */
static void build_formation(int y0, int x0, u16b feat, byte fractal_type, int chance, u16b feat2, byte mode)
{
    int y, x;
    int hgt, wid;
    int tries;

    fractal_template *t_ptr;
    fractal_map map;

    tries = 0;

    /* Pick a fractal template */
    while (TRUE)
    {
        /* Pick one template randomly */
        t_ptr = &fractal_repository[rand_int(N_ELEMENTS(fractal_repository))];

        /* It is of the proper type */
        if (t_ptr->type == fractal_type) break;

        /* Failure */
        if (++tries > 100) return;
    }

    /* Initialize the map */
    map = fractal_map_create(t_ptr);

    /* Create fractal */
    fractal_map_complete(map, t_ptr);

    /* Mark edges if necessary */
    if (mode == 1) fractal_map_mark_edge(map, t_ptr);

    /* Get dimensiones */
    hgt = fractal_dim[fractal_type].hgt;
    wid = fractal_dim[fractal_type].wid;

    /* Pick a random center point if necessary */
    while (!in_bounds(y0, x0))
    {
        y0 = randint(p_ptr->cur_map_hgt - 1);
        x0 = randint(p_ptr->cur_map_wid - 1);
    }

    /* Get top-left corner */
    y0 -= hgt / 2;
    x0 -= wid / 2;

    /* Apply the fractal to the dungeon */
    for (y = 0; y < hgt; y++)
    {
        /* Get real row */
        int yy = y0 + y;

        for (x = 0; x < wid; x++)
        {
            /* Get real column */
            int xx = x0 + x;

            /* Ignore annyoing grids */
            if (!in_bounds(yy, xx)) continue;

            /* Verify if we have to modify the grid. It depends on mode */
            if (mode == 0)
            {
                if (map[y][x] < FRACTAL_FLOOR) continue;
            }
            else
            {
                if (map[y][x] != FRACTAL_EDGE) continue;
            }

            /* See if we have to replace feat with feat2 */
            if ((chance > 0) && (rand_int(100) < chance))
            {
                if (feat2 != FEAT_NONE) cave_set_feat(yy, xx, feat2);
            }
            /* Common case */
            else
            {
                cave_set_feat(yy ,xx, feat);
            }
        }
    }

    /* Free resources */
    FREE_ARRAY(map);
}


/*
 * Helper function for add_wilderness_quest_terrain.  Take a spot, add dangerous wall terrain, and surround
 * it with 3-4 dangerous floor terrains. *
 */
static void add_wilderness_quest_terrain_aux(int y, int x, u16b floor_terrain, u16b wall_terrain)
{
    int yy, xx;
    int y_places[8];
    int x_places[8];
    int num_places = 0;
    int num_floor = randint1(2);
    int i;

    /* Build a solid wall here */
    cave_set_feat(y, x, wall_terrain);

    /* Now find areas to add dangerous floor terrains */
    for (i = 0; i < 8; i++)
    {
        yy = y + ddy[i];
        xx = x + ddx[i];

        if (!in_bounds_fully(yy, xx)) continue;
        if (cave_ff1_match(yy, xx, FF1_PERMANENT))
        {
            /* Don't alter stairs or walls */
            if (cave_ff1_match(yy, xx, FF1_WALL | FF1_STAIRS))continue;
        }
        y_places[num_places] = yy;
        x_places[num_places] = xx;
        num_places++;
    }

    /* Boundry control */
    if (!num_places) return;

    /* Add up to num_floor of the dangerous floor terrains */
    for (i = 0; i < num_floor; i++)
    {
        int k = randint0(num_places);
        yy = y_places[k];
        xx = x_places[k];

        /* Paranoia */
        if (!in_bounds_fully(yy, xx)) continue;

        /* Place the floor terrain, and then eliminate this square from future choices */
        cave_set_feat(yy, xx, floor_terrain);
        num_places--;
        y_places[k] = y_places[num_places];
        x_places[k] = x_places[num_places];
    }
}


/*
 * Add several pockets of terrain that will eventually overrun the level
 */
static void add_wilderness_quest_terrain(u16b floor_terrain, u16b wall_terrain)
{
    int hgt = p_ptr->cur_map_hgt;
    int wid = p_ptr->cur_map_wid;
    int j;

    /*
     * Add some patches of wall and floor in each corner of the dungeon.
     */
    for (j = 0; j < 4; j++)
    {
        int i, x, y;
        int coord_count = 0;
        int y_start, y_end, x_start, x_end, slot;
        int y_places[900];
        int x_places[900];

        /*
         * Just to avoid repeating the code below,
         * we assign the x and y ranges here and then
         * loop through the code below 4 times
         */
        switch (j)
        {
            /* Top left */
            case 1: {y_start = 1; y_end = 31; x_start = 1; x_end = 31; break;}
            /* Bottom left */
            case 2: {y_start = (hgt - 31); y_end = hgt; x_start = 1; x_end = 31; break;}
            /* Top right */
            case 3: {y_start = 1; y_end = 31; x_start = (wid - 31); x_end = wid; break;}
            /* Bottom left */
            default: {y_start = (hgt - 31); y_end = hgt; x_start = (wid - 31); x_end = wid; break;}
        }

        for (y = y_start; y < y_end; y++)
        {
            for (x = x_start; x < x_end; x++)
            {
                /* Paranoia */
                if (!in_bounds_fully(y, x)) continue;

                /* Don't alter stairs or permanent walls */
                if (cave_ff1_match(y, x, FF1_PERMANENT))
                {
                    if (cave_ff1_match(y, x, FF1_WALL | FF1_STAIRS))continue;
                }

                /* store this square */
                y_places[coord_count] = y;
                x_places[coord_count] = x;
                coord_count++;
            }
        }

        for (i = 0; i < 8; i++)
        {
            /* Paranoia */
            if (!coord_count) break;

            /* Pick one of the set of coordinates */
            slot = randint0(coord_count);
            y = y_places[slot];
            x = x_places[slot];

            /* Paranoia */
            if (!in_bounds_fully(y, x)) continue;

            /* Place the floor terrain, and then eliminate this square from future choices */
            add_wilderness_quest_terrain_aux(y, x, floor_terrain, wall_terrain);
            coord_count--;
            y_places[slot] = y_places[coord_count];
            x_places[slot] = x_places[coord_count];
        }
    }
}


/*
 * Place the irregular dungeon border of wilderness levels
 */
static void build_border(int y, int x, u16b feat)
{
    /* Ignore icky grids */
    if (dungeon_info[y][x].cave_info & (CAVE_ICKY)) return;

    /* Ignore grids with effects */
    if (dungeon_info[y][x].effect_idx) return;

    /* Ignore grids with objects */
    if (dungeon_info[y][x].object_idx) return;

    /* Ignore grids with monsters */
    if (dungeon_info[y][x].monster_idx) return;

    /* Place the border */
    cave_set_feat(y, x, feat);
}


/*
 * Create the irregular borders of a wilderness levels.
 * Based on code created by Nick McConnel (FAangband)
 */
static void build_wilderness_borders(u16b feat)
{
    int hgt = p_ptr->cur_map_hgt;
    int wid = p_ptr->cur_map_wid;
    int y, x;
    int i;

    int start_offset = 4;
    int max_offset = 7;

    /* Top border */
    i = start_offset;

    for (x = 0; x < wid; x++)
    {
        /* Modify the offset by -1, +1 or 0 */
        i += (1 - rand_int(3));

        /* Check bounds */
        if (i > max_offset) i = max_offset;
        else if (i < 0) i = 0;

        /* Place border */
        for (y = 0; y < i; y++)
        {
            build_border(y, x, feat);
        }
    }

    /* Bottom border */
    i = start_offset;

    for (x = 0; x < wid; x++)
    {
        /* Modify the offset by -1, +1 or 0 */
        i += (1 - rand_int(3));

        /* Check bounds */
        if (i > max_offset) i = max_offset;
        else if (i < 0) i = 0;

        /* Place border */
        for (y = 0; y < i; y++)
        {
            build_border(hgt - 1 - y, x, feat);
        }
    }

    /* Left border */
    i = start_offset;

    for (y = 0; y < hgt; y++)
    {
        /* Modify the offset by -1, +1 or 0 */
        i += (1 - rand_int(3));

        /* Check bounds */
        if (i > max_offset) i = max_offset;
        else if (i < 0) i = 0;

        /* Place border */
        for (x = 0; x < i; x++)
        {
            build_border(y, x, feat);
        }
    }

    /* Right border */
    i = start_offset;

    for (y = 0; y < hgt; y++)
    {
        /* Modify the offset by -1, +1 or 0 */
        i += (1 - rand_int(3));

        /* Check bounds */
        if (i > max_offset) i = max_offset;
        else if (i < 0) i = 0;

        /* Place border */
        for (x = 0; x < i; x++)
        {
            build_border(y, wid - 1 - x, feat);
        }
    }
}


/*
 * Build a full forest level
 * Returns TRUE on success
 */
static bool build_forest_level(void)
{
    int y, x;
    int i, j;
    int hgt, wid, wid2;

    /* Make it smaller size */
    hgt = p_ptr->cur_map_hgt = MAX_DUNGEON_HGT;
    wid = p_ptr->cur_map_wid = MAX_DUNGEON_WID;

     /* Actual maximum number of rooms on this level */
    dun->row_rooms = p_ptr->cur_map_hgt / BLOCK_HGT;
    dun->col_rooms = p_ptr->cur_map_wid / BLOCK_WID;

    /* Cache center point, height currently isn't needed. */
    wid2 = wid / 2;

    /* Initialize the dungoen with forest soil */
    for (y = 0; y < hgt; y++)
    {
        for (x = 0; x < wid; x++)
        {
            cave_set_feat(y, x, FEAT_FOREST_SOIL);

            dungeon_info[y][x].cave_info |= (CAVE_ROOM);
        }
    }

    /* Place some initial big earth formations */
    for (i = 0; i < 5; i++)
    {
        build_formation(-1, -1, FEAT_FLOOR_EARTH, FRACTAL_TYPE_33x65, 0, FEAT_NONE, 0);
    }

    /* Place one big earth wall formation most of the time */
    j = (one_in_(4) ? 0: 1);

    for (i = 0; i < j; i++)
    {
        build_formation(-1, -1, FEAT_WALL_EARTH, FRACTAL_TYPE_33x65, 0, FEAT_NONE, 0);
    }

    /* Place some irregular rooms of variable size */
    j = 3 + rand_int(20);

    for (i = 0; i < j; i++)
    {
        build_formation(-1, -1, FEAT_WALL_GRANITE, one_in_(3) ? FRACTAL_TYPE_9x9: FRACTAL_TYPE_17x17, 15,
            FEAT_WALL_GRANITE_CRACKED, 1);
    }

    /* Place a dense forest on the left side of the level */
    j = hgt * wid2;

    j = 10 * j / 100;

    for (i = 0; i < j; i++)
    {
        /* Pick random grid */
        y = randint(hgt - 1);
        x = randint(wid2 - 1);

        cave_set_feat(y, x, FEAT_TREE);
    }

    /* Place a clearer forest on the right side */
    j = hgt * wid2;

    j = 5 * j / 100;

    for (i = 0; i < j; i++)
    {
        /* Pick random grid */
        y = randint(hgt - 1);
        x = wid2 + randint(wid2 - 1);

        cave_set_feat(y, x, FEAT_TREE);
    }

    /* Scatter random features through the level (they pierce things) */
    j = hgt * wid;

    j = 5 * j / 100;

    for (i = 0; i < j; i++)
    {
        /* Pick random grid */
        y = randint(hgt - 1);
        x = randint(wid - 1);

        /* Place earth */
        cave_set_feat(y, x, FEAT_FLOOR_EARTH);

        /* Pick random grid */
        y = randint(hgt - 1);
        x = randint(wid - 1);

        /* Place forest soil or grass, sometimes dynamic */
        if (one_in_(4)) cave_set_feat(y, x, (i < (j / 5)) ? FEAT_GRASS_DYNAMIC: FEAT_GRASS);
        else cave_set_feat(y, x, (i < (j / 5)) ? FEAT_FOREST_SOIL_DYNAMIC: FEAT_FOREST_SOIL);
    }

    /* Place a few mud grids */
    j = one_in_(2) ? 30: 10;

    for (i = 0; i < j; i++)
    {
        /* Pick random grid */
        y = randint(hgt - 1);
        x = randint(wid - 1);

        cave_set_feat(y, x, FEAT_FLOOR_MUD);
    }

    /* Place a big lake most of the time */
    j = (one_in_(4) ? 0: 1);

    for (i = 0; i < j; i++)
    {
        build_formation(-1, -1, one_in_(2) ? FEAT_FLOOR_WATER : FEAT_FLOOR_MUD, FRACTAL_TYPE_33x65, 15, FEAT_NONE, 0);
    }

    /* Place some smaller earth wall formations */
    for (i = 0; i < 7; i++)
    {
        build_formation(-1, -1, FEAT_WALL_EARTH, FRACTAL_TYPE_17x17, 10, one_in_(2) ? FEAT_FLOOR_MUD: FEAT_NONE, 0);
    }

    /* Place some small vaults */
    for (i = 0; i < 5; i++)
    {
        /* They are somewhat rare */
        if (one_in_(10))
        {
            /* Pick a location */
            for (j = 0; j < 50; j++)
            {
                /* Get coordinates */
                y = rand_int(dun->row_rooms);
                x = rand_int(dun->col_rooms);

                /* Place the room */
                if (room_build(y, x, 7)) break;
            }
        }
    }

    /* Success */
    return (TRUE);
}


/*
 * Helper. Place an horizontal chain of ice mountains
 * row is the center of the chain
 */
static void build_ice_mountains(int row)
{
    int y, x;
    int y1, y2;
    /* Maximum offset */
    int max_offset = 10;
    /* Minimum width of the mountains */
    int pad = 2;

    /* Initial vertical offset */
    y1 = y2 = 4;

    /* Mountains are placed horizontally */
    for (x = 0; x < p_ptr->cur_map_wid; x++)
    {
        /* Randomize the top offset */
        y1 += 1 - rand_int(3);

        /* Check bounds */
        if (y1 < 0) y1 = 0;
        else if (y1 > max_offset) y1 = max_offset;

        /* Randomize the bottom offset */
        y2 += 1 - rand_int(3);

        /* Check bounds */
        if (y2 < 0) y2 = 0;
        else if (y2 > max_offset) y2 = max_offset;

        /* Place ice walls between top and bottom  */
        for (y = row - y1 - pad; y <= row + y2 + pad; y++)
        {
            /* Check sanity */
            if (in_bounds(y, x))
            {
                int k = rand_int(100);
                u16b feat;

                /* Flavor */
                if (k < 90) feat = FEAT_WALL_ICE;
                else feat = FEAT_WALL_ICE_CRACKED;

                /* Place the wall */
                cave_set_feat(y, x, feat);
            }
        }
    }
}


/*
 * Builds an ice level. Returns TRUE on success, FALSE on error
 */
static bool build_ice_level(void)
{
    int y, x;
    int i, j;
    int hgt, wid;
    int hgt2;

    /* Make it a smaller size */
    hgt = p_ptr->cur_map_hgt = MAX_DUNGEON_HGT;
    wid = p_ptr->cur_map_wid = MAX_DUNGEON_WID;

    /* Actual maximum number of rooms on this level */
    dun->row_rooms = p_ptr->cur_map_hgt / BLOCK_HGT;
    dun->col_rooms = p_ptr->cur_map_wid / BLOCK_WID;

    hgt2 = hgt / 2;

    /* Start with floors */
    for (y = 0; y < hgt; y++)
    {
        for (x = 0; x < wid; x++)
        {
            cave_set_feat(y, x, FEAT_FLOOR);

            dungeon_info[y][x].cave_info |= (CAVE_ROOM);
        }
    }

    /* Put lots of ice */
    for (i = 0; i < 15; i++)
    {
        build_formation(-1, -1, FEAT_FLOOR_ICE, FRACTAL_TYPE_33x65, 0, FEAT_NONE, 0);
    }

    /* Put some "mountains" */
    build_ice_mountains(hgt2);

    /* Add some granite formations */
    for (i = 0; i < 15; i++)
    {
        build_formation(-1, -1, FEAT_WALL_GRANITE_CRACKED, FRACTAL_TYPE_17x17, 0, FEAT_NONE, 0);
    }

    j = 20;

    /* Put some irregular ice walls to break los */
    for (i = 0; i < j; i++)
    {
        int tries;

        for (tries = 0; tries < 200; tries++)
        {
            /* Get random coordinates */
            y = randint(hgt - 1);
            x = randint(wid - 1);

            /* Location must be passable */
            if (cave_ff1_match(y, x, FF1_MOVE)) break;
        }

        build_formation(y, x, FEAT_WALL_ICE_CRACKED, FRACTAL_TYPE_17x17, 40, FEAT_NONE, 1);
    }

    j = 70 + rand_int(51);

    /* Put some pebbles */
    for (i = 0; i < j; i++)
    {
        int tries;

        for (tries = 0; tries < 200; tries++)
        {
            /* Get random coordinates */
            y = randint(hgt - 1);
            x = randint(wid - 1);

            /* Ignore ice */
            if (cave_ff3_match(y, x, ELEMENT_ICE)) continue;

            /* Location must be passable */
            if (cave_ff1_match(y, x, FF1_MOVE)) break;
        }

        cave_set_feat(y, x, FEAT_FLOOR_PEBBLES);
    }

    j = one_in_(4) ? (2 + rand_int(3)): 0;

    /* Add some water pools, sometimes */
    for (i = 0; i < j; i++)
    {
        int tries;

        for (tries = 0; tries < 200; tries++)
        {
            /* Get random coordinates */
            y = randint(hgt - 1);
            x = randint(wid - 1);

            /* Location must be passable */
            if (cave_ff1_match(y, x, FF1_MOVE)) break;
        }

        build_formation(y, x, FEAT_FLOOR_WATER, FRACTAL_TYPE_17x33, 10, FEAT_NONE, 0);
    }

    j = 4;

    /* Pierce the ice mountains making some tunnels */
    for (i = 0; i < j; i++)
    {
        int y2;
        int x2;
        int dist;

        /* Get start x coordinate */
        x = randint(wid - 1);

        do
        {
            /* Get final x coordinate */
            x2 = randint(wid - 1);

            /*
             * Calculate distance. The end must be somewhat
             * far away from the start
             */
            dist = ABS(x2 - x);
        } while ((dist < 20) || (dist > 40));

        /* Get start y coordinate */
        y = rand_range(1, hgt2 - 20);

        /* Get final y coordinate */
        y2 = rand_range(hgt2 + 20, hgt - 2);

        /* Sometimes we swap the y coordinates */
        if (one_in_(2))
        {
            int tmp = y;
            y = y2;
            y2 = tmp;
        }

        /* Make a tunnel */
        build_tunnel(y, x, y2, x2);
    }

    /* Place some fractal rooms more */
    for (i = 0; i < 5; i++)
    {
        /* Only part of the time */
        if (one_in_(2))
        {
            /* Pick a location */
            for (j = 0; j < 50; j++)
            {
                /* Get coordinates */
                y = rand_int(dun->row_rooms);
                x = rand_int(dun->col_rooms);

                /* Place the room */
                if (room_build(y, x, 13)) break;
            }
        }
    }

    return (TRUE);
}


/*
 * Lite all elemental features in the level (walls included), and their adjacent grids
 * If show_objects is TRUE we mark the objects placed on such grids
 */
static void light_elements(bool show_objects)
{
    int y, x;

    /* Look for interesting grids all over the dungeon */
    for (y = 1; y < p_ptr->cur_map_hgt - 1; y++)
    {
        for (x = 1; x < p_ptr->cur_map_wid - 1; x++)
        {
            int i;

            /* Must be an elemental feature */
            if (!cave_ff3_match(y, x, TERRAIN_MASK)) continue;

            /* Lite that grid and grids adjacent to it */
            /* We don't need to call in_bounds */
            for (i = 0; i < 9; i++)
            {
                int yy = y + ddy_ddd[i];
                int xx = x + ddx_ddd[i];

                /* Lite the grid */
                dungeon_info[yy][xx].cave_info |= (CAVE_GLOW | CAVE_MARK | CAVE_EXPLORED);

                /* Remember its objects if necessary */
                if (show_objects)
                {
                    /* Get the index of the first object */
                    s16b o_idx = dungeon_info[yy][xx].object_idx;

                    /* Mark all the objects of the pile */
                    while (o_idx)
                    {
                        /* Get the object */
                        object_type *o_ptr = &o_list[o_idx];

                        /* Mark the object */
                        o_ptr->mark_object();

                        /* Go to the next object */
                        o_idx = o_ptr->next_o_idx;
                    }
                }
            }
        }
    }
}


/*
 * Determine if a monster is suitable for the arena"
 */
static bool monster_wilderness_labrynth_okay(int r_idx)
{
    monster_race *r_ptr = &r_info[r_idx];

    /* No breeders */
    if (r_ptr->flags2 & (RF2_MULTIPLY)) return (FALSE);

    /* creature must move */
    if (r_ptr->flags1 & (RF1_NEVER_MOVE)) return (FALSE);

    /* No mimics */
    if (r_ptr->flags1 & (RF1_CHAR_MIMIC)) return (FALSE);

    /* Okay */
    return (TRUE);
}


/*
 * Builds a pseudo-wilderness level on the dungeon
 * Returns TRUE on success, FALSE on error
 */
bool build_wilderness_level(void)
{
    int y, x, i;
    dun_data dun_body;
    bool done_ice = FALSE;
    bool is_quest_level = FALSE;
    quest_type *q_ptr = &q_info[GUILD_QUEST_SLOT];

    /* Global data */
    dun = &dun_body;

    /* Clear it */
    memset(dun, 0, sizeof(dun_body));

    /* Set level type */
    set_dungeon_type(DUNGEON_TYPE_WILDERNESS);

    /* Reset terrain flags */
    level_flag = 0;

    /* Leave the player in the air for now */
    p_ptr->py = p_ptr->px = 0;

    /*check if we need a quest*/
    if (quest_check(p_ptr->depth) == QUEST_WILDERNESS)
    {
        is_quest_level = TRUE;
    }

    /* Try with a forest */
    if ((p_ptr->depth < 35) || one_in_(2))
    {
        if (!build_forest_level())
        {
            if (cheat_room)
            {
                message(QString("failed to build a forest level"));
            }

            return (FALSE);
        }
    }
    /* Or try with an ice level */
    else
    {
        if (!build_ice_level())
        {
            if (cheat_room)
            {
                message(QString("failed to build an ice level"));
            }

            return (FALSE);
        }

        done_ice = TRUE;
    }

    /* Irregular borders */
    build_wilderness_borders(FEAT_WALL_GRANITE);

    /* Mandatory dungeon borders */
    set_perm_boundry();

    /* Place 3 or 5 down stairs near some walls */
    if (!alloc_stairs(FEAT_STAIRS_DOWN, (3 + randint(2))))
    {
        if (cheat_room)
        {
            message(QString("failed to place down stairs"));
        }

        return (FALSE);
    }

    /* Place 1 or 3 up stairs near some walls */
    if (!alloc_stairs(FEAT_STAIRS_UP, (1 + randint(2))))
    {
        if (cheat_room)
        {
            message(QString("failed to place down stairs"));
        }

        return (FALSE);
    }

    /* Place some things */
    if (!place_traps_rubble_player())
    {
        if (cheat_room)
        {
            message(QString("failed to place traps, rubble and player"));
        }

        return FALSE;
    }

    /* We don't want to trap the player in the level */
    /* Build a tunnel to some far location */
    if (TRUE)
    {
        /* Get the larger dimension of the dungeon */
        int d = MAX(p_ptr->cur_map_hgt, p_ptr->cur_map_wid);
        int tries = 0;

        /* Get the distance to that far location */
        d = ((2 * d) / 5);

        /* Pick a location */
        while (TRUE)
        {
            /* Get coordinates */
            y = randint(p_ptr->cur_map_hgt - 1);
            x = randint(p_ptr->cur_map_wid - 1);

            /* Check distance */
            if (distance(y, x, p_ptr->py, p_ptr->px) >= d) break;

            /* Too many tries */
            if (++tries > 200) return (FALSE);
        }

        /* Build the tunnel */
        build_tunnel(p_ptr->py, p_ptr->px, y, x);
    }

    /* Additional features */
    build_misc_features();

    /* Start destruction of the level for quests */
    if (is_quest_level)
    {
        /* These terrain types are processed in dungeon.c during the quest level */
        if (done_ice) add_wilderness_quest_terrain(FEAT_FLOOR_WATER_BOILING, FEAT_WALL_CRACKED_OVER_BOILING_WATER);
        else add_wilderness_quest_terrain(FEAT_FLOOR_MUD_BOILING, FEAT_WALL_CRACKED_OVER_BOILING_MUD);
    }

    /*get the hook*/
    get_mon_num_hook = monster_wilderness_labrynth_okay;

    /* Prepare allocation table */
    get_mon_num_prep();

    /* Place some things */
    if (!place_monsters_objects())
    {
        /* Reset the allocation table */
        get_mon_num_hook = NULL;
        get_mon_num_prep();

        if (cheat_room)
        {
            message(QString("failed to place monsters and objects"));
        }

        return FALSE;
    }

    /* Reset the allocation table */
    get_mon_num_hook = NULL;
    get_mon_num_prep();

    /* Special illumination for ice levels */
    if (done_ice && ((p_ptr->depth < 50) || one_in_(4))) light_elements(TRUE);

    /*final preps if this is a quest level*/
    if (is_quest_level)
    {
        u16b obj_count = WILDERNESS_COLLECT;
        u16b mon_count = 0;

        q_ptr->q_num_killed = 0;
        q_ptr->q_max_num = 0;

        /*
         * Go through every monster, and mark them as a questor,
         * then make them slightly faster, and light sleepers
         */
        /* Process the monsters */
        for (i = 1; i < mon_max; i++)
        {
            monster_type *m_ptr = &mon_list[i];

            /* Ignore non-existant monsters */
            if (!m_ptr->r_idx) continue;

            /*mark it as a quest monster*/
            m_ptr->mflag |= (MFLAG_QUEST);

            /* Count it */
            mon_count++;

            /* One in 25 generate a bonus item */
            if ((mon_max % 25) == 0) m_ptr->mflag |= (MFLAG_BONUS_ITEM);
        }

        /* Process the monsters */
        for (i = 1; i < mon_max; i++)
        {
            monster_type *m_ptr = &mon_list[i];

            /* Ignore non-existant monsters */
            if (!m_ptr->r_idx) continue;

            if (randint0(mon_count) < obj_count)
            {
                object_type *i_ptr;
                object_type object_type_body;

                /* Make a piece of parchment */
                int k_idx = lookup_kind(TV_PARCHMENT, SV_PARCHMENT_FRAGMENT);
                i_ptr = &object_type_body;
                i_ptr->object_wipe();
                object_prep(i_ptr, k_idx);
                apply_magic(i_ptr, p_ptr->depth, TRUE, FALSE, FALSE, TRUE);

                /*Don't let the player see what the object it, and make it a quest item*/
                i_ptr->ident |= (IDENT_HIDE_CARRY | IDENT_QUEST);

                i_ptr->mark_known(TRUE);

                (void)monster_carry(i, i_ptr);

                /* One less object to drop */
                obj_count--;
            }

            /* One less monster to count */
            mon_count--;

            /* We are done */
            if (!obj_count) break;
        }

        /* Drop some additional parchments */
        alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_PARCHMENT, (WILDERNESS_COLLECT / 2));
    }

    /* Drop some chests to make the level worth exploring */
    alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_CHEST, damroll(3, 3));

    /* Let the player see the whole level */
    wiz_light();

    /* Try hard to place this level */
    rating += 25;

    /* Success */
    return (TRUE);
}


/**
 * Used to convert (x, y) into an array index (i) in build_labrynth_level().
 */
static int lab_toi(int y, int x, int w)
{
    return y * w + x;
}


/**
 * Used to convert an array index (i) into (x, y) in labyrinth_gen().
 */
static void lab_toyx(int i, int w, int *y, int *x)
{
    *y = i / w;
    *x = i % w;
}


/**
 * Given an adjoining wall (a wall which separates two labyrinth cells)
 * set a and b to point to the cell indices which are separated. Used by
 * labyrinth_gen().
 */
static void lab_get_adjoin(int i, int w, int *a, int *b)
{
    int y, x;
    lab_toyx(i, w, &y, &x);
    if (x % 2 == 0)
    {
        *a = lab_toi(y - 1, x, w);
        *b = lab_toi(y + 1, x, w);
    }
    else
    {
        *a = lab_toi(y, x - 1, w);
        *b = lab_toi(y, x + 1, w);
    }
}



/**
 * Shuffle an array using Knuth's shuffle.
 */
static void shuffle(int *arr, int n)
{
    int i, j, k;
    for (i = 0; i < n; i++)
    {
        j = randint0(n - i) + i;
        k = arr[j];
        arr[j] = arr[i];
        arr[i] = k;
    }
}


/**
 * Return whether (x, y) is in a tunnel.
 *
 * For our purposes a tunnel is a horizontal or vertical path, not an
 * intersection. Thus, we want the squares on either side to walls in one
 * case (e.g. up/down) and open in the other case (e.g. left/right). We don't
 * want a square that represents an intersection point.
 *
 * The high-level idea is that these are squares which can't be avoided (by
 * walking diagonally around them).
 */
static bool lab_is_tunnel(int y, int x)
{
    bool west = cave_naked_bold(y, x - 1);
    bool east = cave_naked_bold(y, x + 1);
    bool north = cave_naked_bold(y - 1, x);
    bool south = cave_naked_bold(y + 1, x);

    return ((north == south) && (west == east) && (north != west));
}


/* Note the height and width must be an odd number */
#define LABYRINTH_HGT 41
#define LABYRINTH_WID 81
#define LABYRINTH_AREA (LABYRINTH_WID * LABYRINTH_HGT)


/**
 * Build a labyrinth level.
 *
 * Note that if the function returns FALSE, a level wasn't generated.
 * Labyrinths use the dungeon level's number to determine whether to generate
 * themselves (which means certain level numbers are more likely to generate
 * labyrinths than others).  Taken from Angband 3.3.
 */
bool build_labyrinth_level(void)
{
    int i, j, k, y, x;
    bool is_quest_level = FALSE;
    int cave_squares_x[LABYRINTH_AREA];
    int cave_squares_y[LABYRINTH_AREA];
    int cave_squares_max = 0;

    /*
     * Size of the actual labyrinth part must be odd.
     * NOTE: these are not the actual dungeon size, but rather the size of the
     * area we're generating a labyrinth in (which doesn't count the enclosing
     * outer walls.
     */

    int hgt = LABYRINTH_HGT;
    int wid = LABYRINTH_WID;
    int area = hgt * wid;

    /* NOTE: 'sets' and 'walls' are too large... we only need to use about
     * 1/4 as much memory. However, in that case, the addressing math becomes
     * a lot more complicated, so let's just stick with this because it's
     * easier to read. */

    /* 'sets' tracks connectedness; if sets[i] == sets[j] then cells i and j
     * are connected to each other in the maze. */
    int sets[LABYRINTH_AREA];

    /* 'walls' is a list of wall coordinates which we will randomize */
    int walls[LABYRINTH_AREA];

    /* Most labyrinths are lit */
    bool lit = ((randint0(p_ptr->depth) < 25) || (one_in_(2)));

    /* Many labyrinths are known */
    bool known = (lit && (randint0(p_ptr->depth) < 25));

    /* Most labyrinths have soft (diggable) walls */
    bool soft = ((randint0(p_ptr->depth) < 35) || (!one_in_(3)));

    /* Check if we need a quest */
    if (quest_check(p_ptr->depth) == QUEST_LABYRINTH)
    {
        is_quest_level = TRUE;
        known = TRUE;

        /* Permanent walls for the quests */
        soft = FALSE;

        /* Quest levels are much smaller */
        wid = hgt = LABYRINTH_QUEST_DIMENSIONS;
        area = LABYRINTH_QUEST_AREA;
    }

    p_ptr->cur_map_hgt = hgt + 2;
    p_ptr->cur_map_wid = wid + 2;

    /* Set level type */
    set_dungeon_type(DUNGEON_TYPE_LABYRINTH);

    /* Reset terrain flags */
    level_flag = 0;

    /* Leave the player in the air for now */
    p_ptr->py = p_ptr->px = 0;

    /*
     * Build permanent walls.
     */
    for (y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (x = 0; x < p_ptr->cur_map_wid; x++)
        {
            /* Create permanent wall */
            cave_set_feat(y, x, FEAT_WALL_PERM_SOLID);
        }
    }

    /*
     * Start with solid wall everywhere.
     */
    for (y = 1; y < p_ptr->cur_map_hgt - 1; y++)
    {
        for (x = 1; x < p_ptr->cur_map_wid - 1; x++)
        {
            /* Either soft or permanent walls */
            cave_set_feat(y, x, soft ? FEAT_WALL_GRANITE_SOLID : FEAT_WALL_PERM_SOLID);
        }
    }

    /* Initialize each wall. */
    for (i = 0; i < area; i++)
    {
        walls[i] = i;
        sets[i] = -1;
    }

    /* Cut out a grid of 1x1 rooms which we will call "cells" */
    for (y = 0; y < hgt; y += 2)
    {
        for (x = 0; x < wid; x += 2)
        {
            int k = lab_toi(y, x, wid);
            sets[k] = k;
            cave_set_feat(y + 1, x + 1, FEAT_FLOOR);
            dungeon_info[y + 1][x + 1].cave_info |= (CAVE_ROOM);
            if (lit) dungeon_info[y + 1][x + 1].cave_info |= (CAVE_GLOW);
        }
    }

    /* Shuffle the walls, using Knuth's shuffle. */
    shuffle(walls, area);

    /*
     * For each adjoining wall, look at the cells it divides. If they aren't
     * in the same set, remove the wall and join their sets.
     *
     * This is a randomized version of Kruskal's algorithm.
      */
    for (i = 0; i < area; i++)
    {
        int a, b, x, y;

        j = walls[i];

        /* If this cell isn't an adjoining wall, skip it */
        lab_toyx(j, wid, &y, &x);
        if ((x < 1 && y < 1) || (x > wid - 2 && y > hgt - 2)) continue;
        if (x % 2 == y % 2) continue;

        /* Figure out which cells are separated by this wall */
        lab_get_adjoin(j, wid, &a, &b);

        /* If the cells aren't connected, kill the wall and join the sets */
        if (sets[a] != sets[b])
        {
            int sa = sets[a];
            int sb = sets[b];
            cave_set_feat(y + 1, x + 1, FEAT_FLOOR);
            if (lit) dungeon_info[y + 1][x + 1].cave_info |= (CAVE_GLOW);

            for (k = 0; k < area; k++)
            {
                if (sets[k] == sb) sets[k] = sa;
            }
        }
    }

    /* Place 1 or 2 down stairs  */
    if (!alloc_stairs(FEAT_STAIRS_DOWN, (randint1(2))))
    {
        if (cheat_room)
        {
            message(QString("failed to place down stairs"));
        }

        return (FALSE);
    }

    /* Place 1 or 2 up stairs */
    if (!alloc_stairs(FEAT_STAIRS_UP, (randint1(2))))
    {
        if (cheat_room)
        {
            message(QString("failed to place down stairs"));
        }
        return (FALSE);
    }

    /* No doors, traps or rubble in quest levels */
    if (!is_quest_level)
    {
        /* Test each square in (random) order for openness */
        for (y = 1; y < p_ptr->cur_map_hgt - 1; y++)
        {
            for (x = 1; x < p_ptr->cur_map_wid - 1; x++)
            {
                if (cave_naked_bold(y, x))
                {
                    /* Not in the rooms */
                    if (!lab_is_tunnel(y, x)) continue;

                    cave_squares_y[cave_squares_max] = y;
                    cave_squares_x[cave_squares_max] = x;
                    cave_squares_max++;
                }
            }
        }

        /* Paranoia */
        if (!cave_squares_max)
        {
            if (cheat_room)
            {
                message(QString("failed to place doors"));
            }
            return (FALSE);
        }

        /* Generate a door for every 100 squares in the labyrinth */
        for (i = area / 100; i > 0; i--)
        {
            /* Paranoia */
            if (!cave_squares_max) break;

            x = randint0(cave_squares_max);

            /* Place same doors */
            place_random_door(cave_squares_y[x], cave_squares_x[x]);

            /* Replace the current with the top one */
            cave_squares_max--;
            cave_squares_y[x] = cave_squares_y[cave_squares_max];
            cave_squares_x[x] = cave_squares_x[cave_squares_max];
        }

        /* Place some traps and rubble */
        x = MAX(MIN(p_ptr->depth / 3, 10), 2);
        x = (3 * x * (hgt * wid)) / (MAX_DUNGEON_HGT * MAX_DUNGEON_WID);
        alloc_object(ALLOC_SET_CORR, ALLOC_TYP_RUBBLE, randint(x));
        alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_TRAP, randint(x));
    }

    /* Determine the character location, if it is needed */
    if (!new_player_spot_old())
    {
        if (cheat_room)
        {
            message(QString("failed to place player"));
        }

        return (FALSE);
    }

    /* Only put monsters on non-quest levels */
    if (!is_quest_level)
    {
        /*get the hook*/
        get_mon_num_hook = monster_wilderness_labrynth_okay;

        /* Prepare allocation table */
        get_mon_num_prep();

        /* Slightly out of depth */
        monster_level = p_ptr->depth + 2;

        /* Place some things */
        if (!place_monsters_objects())
        {
            if (cheat_room)
            {
                message(QString("failed to place monsters and objects"));
            }

            /* Reset the allocation table */
            get_mon_num_hook = NULL;
            get_mon_num_prep();
            monster_level = p_ptr->depth;

            return FALSE;
        }

        /* Reset the allocation table */
        get_mon_num_hook = NULL;
        get_mon_num_prep();
        monster_level = p_ptr->depth;
    }

    else q_info->turn_counter = (p_ptr->game_turn - 170);

    /* If we want the players to see the maze layout, do that now */
    if (known) wiz_light();

    /* Try hard to place this level */
    rating += 25;

    /* Success */
    return (TRUE);
}


/*
 * Helper function for building and updating arena levels.
 * Also called from cave.c to have the walls gradually disappear
 * as the arena quest progresses.
 */
void update_arena_level(byte stage)
{
    byte y, x;

    /* No values higher than 9, see arena_level_map in table.c */
    if (stage >= ARENA_MAX_STAGES) return;

    /*
     * Start with add floor spaces where appropriate.
     * See arena_level_map in tables.c
     */
    for (y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (x = 0; x < p_ptr->cur_map_wid; x++)
        {
            /* Ignore the outer walls locations */
            if (!in_bounds_fully(y, x)) continue;

            /* Look for an exact match to the stage */
            if (arena_level_map[y][x] != stage) continue;

            /* Expand the floor area */
            cave_set_feat(y, x, FEAT_FLOOR);

            /* Make it all one big room, and light it up */
            dungeon_info[y][x].cave_info |= (CAVE_ROOM | CAVE_GLOW);
            if (character_dungeon) light_spot(y, x);
        }
    }

    /*
     * On the permanent walls bordering the floor, make them
     * an outer permanent wall.
     */
    for (y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (x = 0; x < p_ptr->cur_map_wid; x++)
        {
            byte d;

            /* Ignore the outer walls locations */
            if (!in_bounds_fully(y, x)) continue;

            /* Really set the feature */
            if (dungeon_info[y][x].feature_idx != FEAT_WALL_PERM_SOLID) continue;

            /* Look in all directions to see if it borders a floor space */
            for (d = 0; d < 8; d++)
            {
                /* Extract adjacent location */
                byte yy = y + ddy_ddd[d];
                byte xx = x + ddx_ddd[d];

                if (!in_bounds_fully(yy, xx)) continue;

                /* Square (y, x) borders the floor */
                if (cave_ff1_match(yy, xx, FF1_MOVE))
                {
                    /* Set it to be an inner permanent wall */
                    cave_set_feat(y, x, FEAT_WALL_PERM_INNER);

                    /* Make it all one big room, and light it up */
                    dungeon_info[y][x].cave_info |= (CAVE_ROOM | CAVE_GLOW);
                    if (character_dungeon) light_spot(y, x);

                    /* Go to the next square */
                    break;
                }
            }
        }
    }
}

/*
 * Helper function for build_arena_level.  Because it is a simple,
 * empty 1x5 corridor, placing the player should be easy *
 */
static bool player_place_arena(void)
{
    u16b empty_squares_y[ARENA_LEVEL_AREA];
    u16b empty_squares_x[ARENA_LEVEL_AREA];
    byte empty_squares = 0;
    byte slot, y, x;

    /*
     * Start with add floor spaces where appropriate.
     * See where the new squares are
     */
    for (y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (x = 0; x < p_ptr->cur_map_wid; x++)
        {
            /* New, and open square */
            if (cave_naked_bold(y, x))
            {
                empty_squares_y[empty_squares] = y;
                empty_squares_x[empty_squares] = x;
                empty_squares++;
            }
        }
    }

    /* Paranoia - shouldn't happen */
    if (!empty_squares) return (FALSE);

    /* Pick a square at random */
    slot = randint0(empty_squares);

    /* Hack - excape stairs stairs */
    p_ptr->create_stair = FEAT_STAIRS_UP;

    return (player_place(empty_squares_y[slot], empty_squares_x[slot]));
}


/*
 * Build a small room to place the player in an arena-like quest.
 * This level should only be built for arena quests.
 * Returns TRUE on success, FALSE on error, but there should never be an error.
 * Monsters and objects are added later in cave.c about every 100 game turns.
 */
bool build_arena_level(void)
{
    quest_type *q_ptr = &q_info[GUILD_QUEST_SLOT];
    byte y, x;

    /* Set level type */
    set_dungeon_type(DUNGEON_TYPE_ARENA);

    /* Reset terrain flags */
    level_flag = 0;

    /* Leave the player in the air for now */
    p_ptr->py = p_ptr->px = 0;

    /* Make it a single size, normal room */
    p_ptr->cur_map_hgt = ARENA_LEVEL_HGT;
    p_ptr->cur_map_wid = ARENA_LEVEL_WID;

    /*
     * Start with solid wall everywhere.
     * See arena_level_map in tables.c
     */
    for (y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (x = 0; x < p_ptr->cur_map_wid; x++)
        {
            /* Create permanent wall */
            cave_set_feat(y, x, FEAT_WALL_PERM_SOLID);
        }
    }

    /* Build the initial arena */
    update_arena_level(0);

    /* Should never fail, since there is only a simple dungeon floor */
    if (!player_place_arena())
    {
        if (cheat_room)
        {
            message(QString("Failed to place player"));
        }

        return (FALSE);
    }

    /* Mark the start of the quest */
    q_ptr->turn_counter = p_ptr->game_turn;

    /* Always use this level */
    rating += 100;

    /* Success */
    return (TRUE);
}


/*
 * Helper function for build_greater_vault_level.
 * Placing the player should be easy *
 */
static bool player_place_greater_vault_level(void)
{
    QVector<coord> empty_squares;
    int slot, y, x;

    /*
     * Start with add floor spaces where appropriate.
     * See where the new squares are
     */
    for (y = 0; y < p_ptr->cur_map_hgt; y++)
    {
        for (x = 0; x < p_ptr->cur_map_wid; x++)
        {
            if (!in_bounds_fully(y, x)) continue;

            /* Not part of the vault */
            if (dungeon_info[y][x].cave_info & (CAVE_ICKY)) continue;

            /* We want to be next to a wall */
            if (!next_to_walls(y, x)) continue;

            /* New, and open square */
            if (cave_naked_bold(y, x))
            {
                empty_squares.append(make_coords(y, x));
            }
        }
    }

    /* Paranoia - shouldn't happen */
    if (empty_squares.size() < 3) return (FALSE);

    /* Pick a square at random */
    slot = randint0(empty_squares.size());

    /* Hack - escape stairs */
    p_ptr->create_stair = FEAT_STAIRS_UP;

    if (!player_place(empty_squares.at(slot).y, empty_squares.at(slot).x)) return (FALSE);

    /* Select a new location for down stairs */
    empty_squares.removeAt(slot);

    /* Pick a square at random */
    slot = randint0(empty_squares.size());

    /* Now place one up stair */
    cave_set_feat(empty_squares.at(slot).y, empty_squares.at(slot).x, FEAT_STAIRS_UP);

    /* Select a new location for down stairs */
    empty_squares.removeAt(slot);

    /* Pick a square at random */
    slot = randint0(empty_squares.size());

    /* Now place one down stair */
    cave_set_feat(empty_squares.at(slot).y, empty_squares.at(slot).x, FEAT_STAIRS_DOWN);

    return (TRUE);
}


/*
 * Build a small room to place the player in an arena-like quest.
 * This level should only be built for arena quests.
 * Returns TRUE on success, FALSE on error, but there should never be an error.
 * Monsters and objects are added later in cave.c about every 100 game turns.
 * Surround it with a border three walls thick of normal granite.
 */
bool build_greater_vault_level(void)
{
    quest_type *q_ptr = &q_info[GUILD_QUEST_SLOT];
    bool is_quest_level = FALSE;
    vault_type *v_ptr;
    int hgt, wid;

    /*check if we need a quest*/
    if (quest_check(p_ptr->depth) == QUEST_GREATER_VAULT)
    {
        is_quest_level = TRUE;
    }

     /* Get the vault record */
    if (is_quest_level)
    {
        v_ptr = &v_info[q_ptr->q_theme];
    }
    /* For ordinary greater vault levels, select one at random */
    else while (TRUE)
    {
        u16b vault_choice = randint0(z_info->v_max);

        /* Get a random vault record */
        v_ptr = &v_info[vault_choice];

        /* Accept the first greater vault */
        if (v_ptr->typ == 8) break;
    }

    /* Set level type */
    set_dungeon_type(DUNGEON_TYPE_GREATER_VAULT);

    /* Reset terrain flags */
    level_flag = 0;

    /* Leave the player in the air for now */
    p_ptr->py = p_ptr->px = 0;

    /* Make it a single greater vault with  */
    hgt = p_ptr->cur_map_hgt = v_ptr->hgt + 6;
    wid = p_ptr->cur_map_wid = v_ptr->wid + 6;

    /* All floors to start, except the outer boundry */
    generate_fill(0, 0, hgt - 1, wid - 1, FEAT_WALL_GRANITE_OUTER);
    generate_fill(4, 4, hgt - 5, wid - 5, FEAT_FLOOR);
    set_perm_boundry();

    /* Message */
    if (cheat_room)
    {
        message(QString("Greater vault (%1)") .arg(v_ptr->vault_name));
    }

    /* Boost the rating */
    rating += v_ptr->rat;

    /* Build the vault */
    build_vault((hgt / 2), (wid / 2), v_ptr);

    /* Mark vault grids with the CAVE_G_VAULT flag */
    mark_g_vault((hgt / 2), (wid / 2), v_ptr->hgt, v_ptr->wid);

    /* Remember the vault's name */
    g_vault_name = v_ptr->vault_name;

    /* Should never fail, since there is only a simple dungeon floor */
    if (!player_place_greater_vault_level())
    {
        if (cheat_room)
        {
            message(QString("Failed to place player"));
        }

        return (FALSE);
    }

    /*final preps if this is a quest level*/
    if (is_quest_level)
    {
        s16b i;

        /*
         * Go through every monster, and mark them as a questor.
         */
        for (i = 1; i < mon_max; i++)
        {
            monster_type *m_ptr = &mon_list[i];

            /* Ignore non-existant monsters */
            if (!m_ptr->r_idx) continue;

            /*mark it as a quest monster*/
            m_ptr->mflag |= (MFLAG_QUEST);
        }
    }

    /* Mark the start of the quest */
    q_ptr->turn_counter = p_ptr->game_turn;

    /* Always place this level */
    rating += 100;

    /* Success */
    return (TRUE);
}

