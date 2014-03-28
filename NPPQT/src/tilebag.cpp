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

    tiles_projections = new TileBag(graf.absoluteFilePath("projections.pak"));
}

TileBag::TileBag(QString path)
{
    pak = new Package(path);
    if (!pak->is_open()) {
        pop_up_message_box(QString("Couldn't load %1").arg(path), QMessageBox::Critical);
    }
}

QPixmap TileBag::get_tile(QString name)
{
    if (!pak->is_open()) return ui_make_blank();

    QPixmap pix;
    if (cache.find(name, &pix)) {            // Cache hit?
        return pix;
    }

    QByteArray data = pak->get_item(name);  // Get png data from package
    if (data.size() < 1) return ui_make_blank(); // Check existence of the tile

    QImage img;
    img.loadFromData(data);                 // Convert png data to RGB

    pix = QPixmap::fromImage(img);

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

