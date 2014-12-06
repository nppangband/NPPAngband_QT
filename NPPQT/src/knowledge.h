#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QTableWidget>
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

class DisplayHomeInven : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayHomeInven(void);
};

class DisplayScores : public QDialog
{
    Q_OBJECT

public:
    DisplayScores();

private:
    QTableWidget *scores_table;
    QSortFilterProxyModel *scores_proxy_model;

};


class DisplayMonKillCount : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayMonKillCount(void);

private:
    QTableWidget *kill_count_table;
    QSortFilterProxyModel *kill_count_proxy_model;

};

extern void display_notes_file(void);
extern void display_home_inventory(void);
extern void display_player_scores(void);
extern void display_mon_kill_count(void);



#endif // KNOWLEDGE_H
