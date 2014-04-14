#include "tilebag.h"
#include "package.h"
#include "npp.h"

TileBag *tiles_32x32;
TileBag *tiles_8x8;
TileBag *tiles_projections;
TileBag *current_tiles;

void init_tile_bags()
{
    QDir graf(NPP_DIR_GRAF);

    if (tiles_32x32) delete tiles_32x32;
    tiles_32x32 = 0;
    if (tiles_8x8) delete tiles_8x8;
    tiles_8x8 = 0;

    if (!tiles_projections) tiles_projections = new TileBag(graf.absoluteFilePath("projections.pak"));

    QString pak32("tiles_32x32.pak");
    QString pak8("tiles_8x8.pak");
    if (game_mode == GAME_NPPMORIA) {
        pak32 = "moria_tiles_32x32.pak";
        pak8 = "moria_tiles_8x8.pak";
    }
    tiles_32x32 = new TileBag(graf.absoluteFilePath(pak32));
    tiles_8x8 = new TileBag(graf.absoluteFilePath(pak8));

    if (use_graphics == GRAPHICS_DAVID_GERVAIS) current_tiles = tiles_32x32;
    else if (use_graphics == GRAPHICS_ORIGINAL) current_tiles = tiles_8x8;
    else current_tiles = 0;
}

TileBag::TileBag(QString path)
{
    pak = new Package(path);
    if (!pak->is_open()) {
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

    if (cache.contains(name)) {            // Cache hit?
        return cache.value(name);
    }

    QByteArray data = pak->get_item(name);  // Get png data from package
    if (data.size() < 1) {
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

