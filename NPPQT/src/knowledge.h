#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include <QComboBox>
#include <QDialogButtonBox>

#include "src/npp.h"


class DisplayNotesFile : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayNotesFile(void);
};

// Used for DisplayMonKillCount
class mon_kills
{
public:
    s16b mon_idx;
    s16b total_kills;
};

class DisplayMonKillCount : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayMonKillCount(void);
};

extern void display_notes_file(void);
extern void display_mon_kill_count(void);


#endif // KNOWLEDGE_H
