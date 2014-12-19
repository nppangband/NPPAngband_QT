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
typedef struct object_grouper object_grouper;

struct monster_group
{
    QString chars;
    QString name;
};

/**
 * Defines a (value, name) pairing.  Variable names used are historical.
 */
struct object_grouper
{
    int tval;
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
    QButtonGroup *mon_button_group;

    bool do_spoiler;

private slots:
    // Receives the number of the button pressed.
    void button_press(int mon_race);
    void filter_rows(int row, int col);
    bool mon_matches_mon_group(int r_idx, int group);
};

class DisplayObjectKnowledge : public QDialog
{
    Q_OBJECT

public:
    DisplayObjectKnowledge(void);

private:
    QSortFilterProxyModel *object_proxy_model;
    QTableWidget *object_table;
    QTableWidget *object_group_table;
    QVector<bool> object_group_info;
    QButtonGroup *object_button_group;
    QButtonGroup *object_settings_group;

    bool do_spoiler;

private slots:
    // Receives the number of the button pressed.
    void button_press(int k_idx);
    void settings_press(int k_idx);
    void filter_rows(int row, int col);
    int object_matches_group(int k_idx);
};

class DisplayEgoItemKnowledge : public QDialog
{
    Q_OBJECT

public:
    DisplayEgoItemKnowledge(void);

private:
    QSortFilterProxyModel *ego_item_proxy_model;
    QTableWidget *ego_item_table;
    QTableWidget *ego_item_group_table;
    QVector<bool> ego_item_group_info;
    QButtonGroup *ego_item_button_group;
    QButtonGroup *ego_item_squelch_toggle;

    bool do_spoiler;

private slots:
    // Receives the number of the button pressed.
    void button_press(int e_idx);
    void settings_press(int e_idx);
    void filter_rows(int row, int col);
    bool ego_item_matches_group(int e_idx, int group);
};

class DisplayTerrainKnowledge : public QDialog
{
    Q_OBJECT

public:
    DisplayTerrainKnowledge(void);

private:
    QSortFilterProxyModel *terrain_proxy_model;
    QTableWidget *terrain_table;
    QTableWidget *terrain_group_table;
    QVector<bool> terrain_group_info;
    QButtonGroup *terrain_button_group;

    bool do_spoiler;

private slots:
    // Receives the number of the button pressed.
    void button_press(int f_idx);
    void filter_rows(int row, int col);
    int terrain_matches_group(int f_idx);
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
extern void make_object_fake(object_type *o_ptr, int k_idx);
extern void apply_magic_fake(object_type *o_ptr);


#endif // KNOWLEDGE_H
