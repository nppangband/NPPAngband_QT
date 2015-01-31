#ifndef PLAYER_BIRTH_H
#define PLAYER_BIRTH_H

#include <src/player_screen.h>
#include <QButtonGroup>
#include <QLineEdit>

/*
 * A class to hold "rolled" information, and any
 * other useful state for the birth process.
 */
class Birther
{
public:


    byte mode;

    QString full_name;

    byte p_sex;
    byte p_race;
    byte p_class;

    s16b age;
    s16b wt;
    s16b ht;
    s16b sc;

    s32b au;

    s16b stat[A_MAX];

    QString history;

    void birther_wipe();
    void save();
    void load();

};



class PlayerBirth : public PlayerScreenInfo
{
    Q_OBJECT

public:
    explicit PlayerBirth(bool quickstart);

    bool done_birth;
    bool quick_start;

private:
    void setup_character();

    // try not to update the character more than once each action
    bool hold_update;


    //Option checkboxes
    QButtonGroup *group_options;
    void add_option_boxes(QVBoxLayout *return_layout);

    //Gender and buttons
    QButtonGroup *group_gender;
    QLineEdit *player_name;
    QString cur_name;
    int cur_gender;
    void add_genders(QVBoxLayout *return_layout);

    //Race
    QButtonGroup *group_race;
    int cur_race;
    void add_races(QVBoxLayout *return_layout);

    //Class
    QButtonGroup *group_class;
    int cur_class;
    void add_classes(QVBoxLayout *return_layout);

    //help boxes
    QLabel *race_info;
    QLabel *class_info;
    void add_info_boxes(QVBoxLayout *return_layout);

    //stats
    void add_stat_boxes(QVBoxLayout *return_layout);
    void add_stat_results(void);
    void update_stats_info();

    //Statroll options
    QVBoxLayout *vlay_stats_current;
    QGridLayout *grid_stat_results;
    QButtonGroup *group_stat_choice;
    void add_stat_choices(QVBoxLayout *return_layout);
    bool point_based;
    int stats[A_MAX];
    int points_spent[A_MAX];
    int points_left;

    void update_points();
    void update_character();


private slots:
    void name_changed(QString new_name);
    void gender_changed(int new_gender);
    void race_changed(int new_race);
    void class_changed(int new_class);
    void option_changed(int index);
    void call_options_dialog(void);
    void point_button_chosen(void);
    void random_button_chosen(void);
    void redo_stat_box(void);
    void stat_spin_changed(int new_value);


    // Random char slots
    void random_name(void);
    void random_gender(void);
    void random_race(void);
    void random_class(void);
    void random_all(void);

};

// birth.cpp
extern void init_birth();
extern void finish_birth();
extern void reset_stats(int stats[A_MAX], int points_spent[A_MAX], int *points_left);
extern bool buy_stat(int choice, int stats[A_MAX], int points_spent[A_MAX], int *points_left);
extern void sell_stat(int choice, int stats[A_MAX], int points_spent[A_MAX], int *points_left);
extern void generate_stats(int stats[A_MAX], int points_spent[A_MAX], int *points_left);
extern void generate_player();
extern void roll_player(int stats[A_MAX]);
extern bool has_prev_character();
extern void save_prev_character();
extern void load_prev_character();
extern QString format_stat(s16b value);



#endif // PLAYER_BIRTH_H
