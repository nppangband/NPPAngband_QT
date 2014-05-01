#ifndef PLAYER_DEATH_H
#define PLAYER_DEATH_H

#include <QTime>
#include <QDate>
#include <QtWidgets>
#include "src/npp.h"
#include "src/store.h"



class PlayerDeathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerDeathDialog(void);

private:
    void death_info(void);
    void death_messsages(void);
    void death_file_dump(void);
    void death_scores(void);
    void death_examine(void);
    void death_notes(void);
    void death_spoilers(void);

};

#endif // PLAYER_DEATH_H
