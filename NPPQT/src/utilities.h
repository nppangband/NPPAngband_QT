#ifndef UTILITIES_H
#define UTILITIES_H

#include <QVector>
#include <src/defines.h>
#include <QString>
#include <QColor>

typedef struct letters_and_numbers letters_and_numbers;
typedef struct message_type message_type;
typedef struct notes_type notes_type;

struct letters_and_numbers
{
    QChar let;
    int num;
};

struct message_type
{
    QColor msg_color;
    QString message;
    u16b repeats;
    s32b message_turn;
    bool append;

};

struct notes_type
{
    byte player_level;
    s16b dun_depth;
    s32b game_turn;
    QString recorded_note;
};

extern QVector<message_type> message_list;
extern QVector<notes_type>  notes_log;

static letters_and_numbers lowercase_and_numbers[26] =
{
    { 'a', 0},
    { 'b', 1},
    { 'c', 2},
    { 'd', 3},
    { 'e', 4},
    { 'f', 5},
    { 'g', 6},
    { 'h', 7},
    { 'i', 8},
    { 'j', 9},
    { 'k', 10},
    { 'l', 11},
    { 'm', 12},
    { 'n', 13},
    { 'o', 14},
    { 'p', 15},
    { 'q', 16},
    { 'r', 17},
    { 's', 18},
    { 't', 19},
    { 'u', 20},
    { 'v', 21},
    { 'w', 22},
    { 'x', 23},
    { 'y', 24},
    { 'z', 25}
};

#define IS_GRAY(color) ((color).red() == (color).green() && (color).green() == (color).blue())

extern QString likert(int x, int y, byte *attr);
extern QString _num(int n);
extern void popup1(QString message, int id = 0);



#endif // UTILITIES_H
