#ifndef LOADSAVE_H
#define LOADSAVE_H

#include <QDataStream>
#include <QFile>
#include <QTextStream>
#include <QBitArray>

extern byte sf_major;
extern byte sf_minor;
extern byte sf_patch;
extern byte sf_extra;
extern u32b sf_xtra;
extern u32b sf_when;
extern u16b sf_lives;
extern u16b sf_saves;
extern bool arg_fiddle;

#endif // LOADSAVE_H
