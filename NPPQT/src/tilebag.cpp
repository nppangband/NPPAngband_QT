#include "tilebag.h"
#include "package.h"
#include "npp.h"
#include "src/init.h"
#include <src/knowledge.h>

TileBag *tiles_64x64;
TileBag *tiles_32x32;
TileBag *tiles_8x8;
TileBag *tiles_projections;
TileBag *current_tiles;

class tile_coords
{
public:
    QString name;
    int y_coord;
    int x_coord;
};

static tile_coords these_tiles[] =
{
    {"1  Phial~",7,29},
    {"2  Star~",7,30},
    {"3  Arkenstone~",7,31},
    {"4  Amulet~",7,2},
    {"5  Amulet~",7,3},
    {"6  Necklace~",7,4},
    {"7  Palantir~",7,31},
    {"8  Elfstone~",7,3},
    {"9  Jewel~",7,4},
    {"10  Gem~",7,30},
    {"11  Ring~",2,3},
    {"12  Ring~",2,3},
    {"13  Ring~",2,4},
    {"14  Ring~",2,5},
    {"15  Ring~",2,6},
    {"16  Ring~",2,7},
    {"19  Mighty Hammer~",7,26},
    {"20  Massive Iron Crown~",7,27},
    {"21  Filthy Rag~",8,20},
    {"22  Robe~",8,21},
    {"23 Soft Leather Armour~",8,22},
    {"24 Soft Studded Leather~",77,18},
    {"25 Hard Leather Armour~",8,23},
    {"26 Hard Studded Leather~",77,19},
    {"27 Leather Scale Mail~",78,14},
    {"28 Rusty Chain Mail~",8,26},
    {"29 Metal Scale Mail~",8,24},
    {"30 Chain Mail~",8,25},
    {"31 Augmented Chain Mail~",77,16},
    {"32 Double Chain Mail~",77,17},
    {"33 Metal Brigandine Armour~",77,16},
    {"34 Partial Plate Armour~",77,20},
    {"35 Metal Lamellar Armour~",77,21},
    {"36 Full Plate Armour~",77,26},
    {"37 Ribbed Plate Armour~",77,25},
    {"38 Mithril Chain Mail~",77,24},
    {"39 Mithril Plate Mail~",77,23},
    {"40 Adamantite Plate Mail~",77,22},
    {"41  Small Leather Shield~",8,15},
    {"42  Small Metal Shield~",8,17},
    {"43  Large Leather Shield~",8,16},
    {"44  Large Metal Shield~",8,18},
    {"45  Shield~ of Deflection",8,19},
    {"46 Dragon Scale Mail~",8,30},
    {"47 Mature Dragon Scale Mail~",8,28},
    {"48 Ancient Dragon Scale Mail~",8,27},
    {"49 Great Wyrm Scale Mail~",9,2},
    {"50 Dragon Scale Shield~",78,7},
    {"51 Mature Dragon Scale Shield~",78,8},
    {"52 Ancient Dragon Scale Shield~",78,9},
    {"53 Great Wyrm Scale Shield~",78,10},
    {"54  Broken Dagger~",3,13},
    {"55  Broken Sword~",3,14},
    {"56  Dagger~",3,24},
    {"57  Main Gauche~",3,24},
    {"58  Rapier~",3,25},
    {"59  Small Sword~",3,25},
    {"60  Short Sword~",3,17},
    {"61  Sabre~",3,25},
    {"62  Cutlass~",3,25},
    {"63  Broad Sword~",3,15},
    {"64  Long Sword~",3,26},
    {"65  Scimitar~",3,16},
    {"66  Katana~",3,16},
    {"67  Bastard Sword~",3,22},
    {"68  Two-Handed Sword~",3,21},
    {"69  Executioner's Sword~",3,27},
    {"70  Blade~ of Chaos",3,28},
    {"71  Spear~",11,17},
    {"72  Awl-Pike~",11,18},
    {"73  Trident~",9,18},
    {"74  Pike~",11,19},
    {"75  Beaked Axe~",11,17},
    {"76  Broad Axe~",11,18},
    {"77  Throwing Axe~",11,19},
    {"78  Glaive~",11,20},
    {"79  Halberd~",11,21},
    {"80  Scythe~",9,20},
    {"81  Battle Axe~",11,25},
    {"82  Great Axe~",11,26},
    {"83  Lochaber Axe~",11,27},
    {"84  Scythe~ of Slicing",11,30},
    {"85  Whip~",9,9},
    {"86  Quarterstaff~",9,14},
    {"87  Mace~",9,12},
    {"88  Ball-and-Chain~",9,8},
    {"89  War Hammer~",9,15},
    {"90  Lucerne Hammer~",11,28},
    {"91  Throwing Hammer~",11,29},
    {"92  Morning Star~",9,11},
    {"93  Flail~",11,31},
    {"94  Lead-Filled Mace~",11,12},
    {"95  Mace~ of Disruption",9,13},
    {"96  Shovel~",10,24},
    {"97  Pick~",10,23},
    {"98  Mattock~",77,0},
    {"99  Hard Leather Cap~",8,2},
    {"100  Metal Cap~",8,3},
    {"101  Iron Helm~",8,4},
    {"102  Steel Helm~",8,5},
    {"103  Iron Crown~",8,6},
    {"104  Golden Crown~",8,7},
    {"105  Jewel Encrusted Crown~",8,8},
    {"106  Cloak~",8,0},
    {"107  Shadow Cloak~",8,1},
    {"108  Sling~",9,25},
    {"109  Short Bow~",9,21},
    {"110  Long Bow~",9,22},
    {"111  Light Crossbow~",9,23},
    {"112  Heavy Crossbow~",9,24},
    {"113  Rounded Pebble~",9,30},
    {"114  Iron Shot~",9,31},
    {"115  Mithril Shot~",77,5},
    {"116  Arrow~",9,26},
    {"117  Seeker Arrow~",9,27},
    {"118  Mithril Arrow~",78,17},
    {"119  Bolt~",9,28},
    {"120  Seeker Bolt~",9,29},
    {"121  Mithril Bolt~",78,18},
    {"122  Pair~ of Soft Leather Boots",8,9},
    {"123  Pair~ of Hard Leather Boots",8,10},
    {"124  Pair~ of Metal Shod Boots",8,11},
    {"125  Set~ of Leather Gloves",8,12},
    {"126  Set~ of Gauntlets",8,13},
    {"127  Set~ of Cesti",8,14},
    {"128  Wooden Torch~",11,6},
    {"129  Brass Lantern~",11,5},
    {"372  Small wooden chest~",0,22},
    {"373  Small iron chest~",0,24},
    {"374  Small steel chest~",0,26},
    {"375  Large wooden chest~",0,23},
    {"376  Large iron chest~",0,25},
    {"377  Large steel chest~",0,27},
    {"378  Large Jeweled chest~",0,28},
    {"403  Iron Spike~",11,4},
    {"404 copper",0,9},
    {"405 copper",0,10},
    {"406 copper",0,11},
    {"407 silver",0,12},
    {"408 silver",0,12},
    {"409 silver",0,12},
    {"410 garnets",0,15},
    {"411 garnets",0,15},
    {"412 gold",0,13},
    {"413 gold",0,13},
    {"414 gold",0,13},
    {"415 opals",0,16},
    {"416 sapphires",0,17},
    {"417 rubies",0,18},
    {"418 diamonds",0,19},
    {"419 emeralds",0,20},
    {"420 mithril",0,14},
    {"421 adamantite",0,21},
    {"422 Magic for Beginners",10,3},
    {"423 Conjurings and Tricks",10,3},
    {"424 Incantations and Illusions",10,3},
    {"425 Sorcery and Evocations",10,3},
    {"426 Resistances of Scarabtarices",10,4},
    {"427 Raal's Tome of Destruction",10,8},
    {"428 Mordenkainen's Escapes",10,5},
    {"429 Tenser's Transformations",10,7},
    {"430 Kelek's Grimoire of Power",10,6},
    {"431 Beginners Handbook",10,12},
    {"432 Words of Wisdom",10,12},
    {"433 Chants and Blessings",10,12},
    {"434 Exorcism and Dispelling",10,12},
    {"435 Ethereal Openings",10,13},
    {"436 Godly Insights",10,14},
    {"437 Purifications and Healing",10,15},
    {"438 Holy Infusions",10,16},
    {"439 Wrath of God",10,17},
    {"440  Broken Bone~",11,11},
    {"441 Mass Identify",6,25},
    {"442 *Identify*",6,22},
    {"443 Mass Identify",6,22},
    {"444 Call of the Wild",10,1},
    {"445 Environmental Adjurations",10,1},
    {"446 Commanding Nature",10,1},
    {"447 Lore of Engagement",10,1},
    {"448 Radagast's Protections",10,2},
    {"449 Melian's Reformations",10,9},
    {"450 Arda's Habitats",10,10},
    {"451 Natural Infusions",10,11},
    {"452 Nature's Fury",10,18},
    {"453  Faded Scroll Fragment",6,0},



    {NULL, 0, 0},
};


void init_tile_bags()
{
    if (tiles_64x64) delete tiles_64x64;
    tiles_64x64 = 0;
    if (tiles_32x32) delete tiles_32x32;
    tiles_32x32 = 0;
    if (tiles_8x8) delete tiles_8x8;
    tiles_8x8 = 0;

    if (!tiles_projections) tiles_projections = new TileBag(npp_dir_graf.absoluteFilePath("projections.pak"));

    QString pak64("tiles_64x64.pak");
    QString pak32("tiles_32x32.pak");
    QString pak8("tiles_8x8.pak");
    if (game_mode == GAME_NPPMORIA)
    {
        pak64 = "moria_tiles_64x64.pak";
        pak32 = "moria_tiles_32x32.pak";
        pak8 = "moria_tiles_8x8.pak";
    }
    tiles_64x64 = new TileBag(npp_dir_graf.absoluteFilePath(pak64));
    tiles_32x32 = new TileBag(npp_dir_graf.absoluteFilePath(pak32));
    tiles_8x8 = new TileBag(npp_dir_graf.absoluteFilePath(pak8));

    if (use_graphics == GRAPHICS_RAYMOND_GAUSTADNES) current_tiles = tiles_64x64;
    else if (use_graphics == GRAPHICS_DAVID_GERVAIS) current_tiles = tiles_32x32;
    else if (use_graphics == GRAPHICS_ORIGINAL) current_tiles = tiles_8x8;
    else current_tiles = 0;
}

TileBag::TileBag(QString path)
{
    pak = new Package(path);
    if (!pak->is_open())
    {
        pop_up_message_box(QString("Couldn't load %1").arg(path), QMessageBox::Critical);
    }
}

bool TileBag::has_tile(QString name)
{
    if (!pak->is_open()) return false;
    QString ext(".png");
    if (!name.endsWith(ext)) name += ext;
    if (pak->item_position(name) < 0) return false;
    return true;
}

QPixmap TileBag::get_tile(QString name)
{
    if (!pak->is_open()) return ui_make_blank();

    if (!name.endsWith(".png")) name += ".png";

    if (cache.contains(name)) // Cache hit?
    {
        return cache.value(name);
    }

    QByteArray data = pak->get_item(name);  // Get png data from package
    if (data.size() < 1)
    {
        color_message("Tile not found: " + name, TERM_ORANGE);
        return ui_make_blank(); // Check existence of the tile
    }

    QImage img;
    img.loadFromData(data);                 // Convert png data to RGB

    QPixmap pix = QPixmap::fromImage(img);

    cache.insert(name, pix);                // Save for later use

    return pix;
}

void TileBag::clear_cache()
{
    cache.clear();
}

TileBag::~TileBag()
{
    delete pak;
}

static QString tile_obj_name_convert(QString orig_name)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    QString star_string = "*";

    bool contains_star = orig_name.contains(star_string);

    //now delete all commas, and then replace spaces with '_', then start with "obj_".
    orig_name.remove(QChar(','));
    orig_name.remove(QString("'"));
    orig_name.remove(QChar('['));
    orig_name.remove(QChar(']'));
    orig_name.remove(QChar('('));
    orig_name.remove(QChar(')'));
    orig_name.remove(QChar('*'));
    orig_name.replace(QChar('/'), QChar('_'));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.prepend("obj_");
    if (contains_star) orig_name.append("_star");
    return (orig_name);
}


static QString tile_mon_name_convert(QString orig_name)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "mon_".
    orig_name.remove(QChar(','));
    orig_name.remove(QString("'"));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.prepend("mon_");
    return (orig_name);
}

static QString tile_feat_name_convert(QString orig_name)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "mon_".
    orig_name.remove(QChar(','));
    orig_name.remove(QChar('('));
    orig_name.remove(QChar(')'));
    orig_name.remove(QString("'"));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.prepend("feat_");
    return (orig_name);
}

static QString tile_flav_name_convert(QString orig_name, int tval)
{
    // first, make it standard ASCII, then lowercase
    orig_name = to_ascii(orig_name);
    orig_name = orig_name.toLower();

    //now delete all commas, and then replace spaces with '_', then start with "mon_".
    orig_name.remove(QChar(','));
    orig_name.remove(QChar('('));
    orig_name.remove(QChar(')'));
    orig_name.remove(QString("'"));
    orig_name.remove(QChar('*'));
    orig_name.replace(QChar('/'), QChar('_'));
    orig_name.replace(QChar(' '), QChar('_'));
    if (tval == TV_RING) orig_name.prepend("ring_");
    else if (tval == TV_AMULET) orig_name.prepend("amulet_");
    else if (tval == TV_STAFF) orig_name.prepend("staff_");
    else if (tval == TV_WAND) orig_name.prepend("wand_");
    else if (tval == TV_ROD) orig_name.prepend("rod_");
    else if (tval == TV_FOOD) orig_name.prepend("mushroom_");
    else if (tval == TV_POTION) orig_name.prepend("potion_");
    else if (tval == TV_SCROLL) orig_name.prepend("scroll_");
    orig_name.prepend("flav_");
    return (orig_name);
}


void extract_tiles(void)
{
    int i;

    for (i = 0; i < z_info->r_max; i++)\
    {
        monster_race *r_ptr = &r_info[i];
        if (r_ptr->r_name_full.isEmpty()) continue;
        r_ptr->tile_id = tile_mon_name_convert(monster_desc_race(i));
    }
    for (i = 0; i < z_info->k_max; i++)\
    {
        object_kind *k_ptr = &k_info[i];
        if (k_ptr->k_name.isEmpty()) continue;
        object_type object_type_body;
        object_type *o_ptr = &object_type_body;
        /* Check for known artifacts, display them as artifacts */
        if (k_ptr->k_flags3 & (TR3_INSTA_ART))
        {
            int art_num = i;
            if (game_mode == GAME_NPPANGBAND) {
                if (k_ptr->tval == TV_HAFTED && k_ptr->sval == SV_GROND) art_num = ART_GROND;
                if (k_ptr->tval == TV_CROWN && k_ptr->sval == SV_MORGOTH) art_num = ART_MORGOTH;
            }
            make_fake_artifact(o_ptr, art_num);
            object_aware(o_ptr);
            object_known(o_ptr);
            o_ptr->ident |= (IDENT_MENTAL);
            o_ptr->update_object_flags();
        }
        else
        {
            o_ptr->object_wipe();
            object_prep(o_ptr, i);
            apply_magic_fake(o_ptr);
            o_ptr->ident |= (IDENT_STORE);
            if (!k_info[i].flavor)
            {
                /* Mark the item as fully known */
                o_ptr->ident |= (IDENT_MENTAL | IDENT_STORE);
                object_aware(o_ptr);
                object_known(o_ptr);
                o_ptr->update_object_flags();
            }
        }

        QString object_name;
        if (o_ptr->tval == TV_GOLD) object_name = strip_name(i);
        else object_name = object_desc(o_ptr, ODESC_BASE);
        k_ptr->tile_id = tile_obj_name_convert(object_name);
    }
    for (i = 0; i < z_info->f_max; i++)
    {
        feature_type *f_ptr = &f_info[i];
        if (f_ptr->f_name.isEmpty()) continue;
        QString feat_name = feature_desc(i, FALSE, FALSE);
        f_ptr->tile_id = tile_feat_name_convert(feat_name);
    }
    for (i = 0; i < z_info->flavor_max; i++)
    {
        flavor_type *flavor_ptr = &flavor_info[i];
        if (flavor_ptr->text.isEmpty() && flavor_ptr->tval != TV_SCROLL) continue;
        QString flavor_name = flavor_ptr->text;
        flavor_ptr->tile_id = tile_flav_name_convert(flavor_name, flavor_ptr->tval);
    }

    QString race_name = to_ascii(p_info[p_ptr->prace].pr_name).toLower();
    QString class_name = to_ascii(c_info[p_ptr->pclass].cl_name).toLower();

    if (use_graphics == GRAPHICS_RAYMOND_GAUSTADNES)
    {
        p_ptr->tile_id = QString("player_%1").arg(race_name);
        if (p_ptr->psex == SEX_FEMALE) p_ptr->tile_id.append("_female");
        else p_ptr->tile_id.append("_male");
        p_ptr->tile_id.append(QString("_%1").arg(class_name));
    }
    else p_ptr->tile_id = QString("player_%1_%2").arg(race_name).arg(class_name);
}

void get_8x8_tiles(void)
{
    QPixmap tileset(":/icons/lib/graf/npp8x8.png");

    for (int i = 0; i < 2000; i++)
    {
        tile_coords *this_tile = &these_tiles[i];
        if (this_tile->name.isNull()) break;
        QPixmap tile_bit = tileset.copy(this_tile->x_coord*8, this_tile->y_coord*8, 8, 8);
        QString file_name = npp_dir_graf.absoluteFilePath(this_tile->name);
        file_name.append(".png");

        tile_bit.save(file_name);

    }


}
