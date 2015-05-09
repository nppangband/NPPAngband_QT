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

    //now delete all commas, and then replace spaces with '_', then start with "obj_".
    orig_name.remove(QChar(','));
    orig_name.remove(QString("'"));
    orig_name.remove(QChar('['));
    orig_name.remove(QChar(']'));
    orig_name.remove(QChar('('));
    orig_name.remove(QChar(')'));
    orig_name.remove(QChar('/'));
    orig_name.replace(QChar(' '), QChar('_'));
    orig_name.replace("*", "_star_");
    orig_name.prepend("obj_");
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
        QString race_name = monster_desc_race(i);
        if (r_ptr->flags1 & (RF1_FRIEND | RF1_FRIENDS)) race_name = plural_aux(race_name);
        r_ptr->tile_id = tile_mon_name_convert(race_name);
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
