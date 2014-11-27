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



extern void display_notes_file(void);


#endif // KNOWLEDGE_H
