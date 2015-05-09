#ifndef TILEBAG_H
#define TILEBAG_H

#include <QtCore>
#include <QHash>
#include <QPixmap>

class Package;

class TileBag
{
public:
    // It takes the path to the package file
    TileBag(QString path);

    Package *pak;
    QHash<QString,QPixmap> cache;

    QPixmap get_tile(QString name);         // Use this!

    void clear_cache();

    bool has_tile(QString name);

    virtual ~TileBag();
};

extern TileBag *tiles_64x64;
extern TileBag *tiles_32x32;
extern TileBag *tiles_8x8;
extern TileBag *tiles_projections;

// This will hold tiles_64x64, tiles_32x32 or tiles_8x8 depending on graphics mode
// or null in ascii_mode
extern TileBag *current_tiles;

extern void init_tile_bags();

extern void extract_tiles(void);

#endif // TILEBAG_H
