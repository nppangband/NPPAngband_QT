#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QTableWidget>
#include <QButtonGroup>
#include <QSplitter>
#include "src/npp.h"


typedef struct monster_group monster_group;


struct monster_group
{
    QString chars;
    QString name;
};


class DisplayMonsterKnowledge : public QDialog
{
    Q_OBJECT

public:
    DisplayMonsterKnowledge(void);

private:
    QSortFilterProxyModel *monster_proxy_model;
    QTableWidget *monster_table;
    QTableWidget *mon_group_table;
    QVector<bool> monster_group_info;
    QButtonGroup *mon_info_group;

    QSplitter *mon_knowledge_splitter;

    bool do_spoiler;

private slots:
    // Receives the number of the button pressed.
    void button_press(int mon_race);
    void filter_rows(int row, int col);
    bool mon_matches_mon_group(int r_idx, int group);
};

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

extern void display_monster_knowledge(void);
extern void display_object_knowledge(void);
extern void display_ego_item_knowledge(void);
extern void display_artifact_knowledge(void);
extern void display_terrain_knowledge(void);
extern void display_notes_file(void);
extern void display_home_inventory(void);
extern void display_player_scores(void);
extern void display_mon_kill_count(void);



#endif // KNOWLEDGE_H
