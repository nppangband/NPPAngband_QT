#ifndef PLAYER_SCREEN_H
#define PLAYER_SCREEN_H

#include <QVBoxLayout>
#include <src/npp.h>
#include <QLabel>

enum
{
    FLAGS_RESIST = 0,
    FLAGS_ABILITY,
    FLAGS_NATIVITY,
};


struct player_flag_record
{
    QString name;           /* Name of resistance/ability */
    byte set;				/* Which field this resistance is in { 1 2 3 } */
    u32b this_flag;			/* resistance flag bit */
    u32b extra_flag;		/* corresponding immunity or sustain bitflag, if any */
    bool moria_flag;		/* Is it used in Moria? */
    bool bad_flag;          // Is it a bad one?
};

class PlayerScreenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerScreenDialog(void);

public slots:
    void name_change(void);

private:
    void name_change_pushbutton(QGridLayout *return_layout);

};

extern void equip_flag_info(QWidget *this_widget, QGridLayout *return_layout, int flag_set, QFont this_font);
extern void equip_modifier_info(QWidget *this_widget, QGridLayout *return_layout, QFont this_font);
extern void char_basic_info(QGridLayout *return_layout);
extern void char_basic_data(QGridLayout *return_layout);
extern void char_game_info(QGridLayout *return_layout);
extern void char_stat_info(QGridLayout *stat_layout);
extern void char_combat_info(QGridLayout *return_layout);
extern void char_ability_info(QGridLayout *return_layout);
extern void equip_flag_info_update(QWidget *this_widget, QGridLayout *return_layout, int flag_set, QFont this_font);
extern void equip_modifier_info_update(QWidget *this_widget, QGridLayout *return_layout, QFont this_font);
extern QString stat_entry(int stat);
extern void do_cmd_character_screen(void);
extern QString moria_speed_labels(int speed);
extern byte analyze_speed_bonuses(int speed, byte default_attr);
extern void make_standard_label(QLabel *this_label, QString title, byte preset_color, QFont this_font);
extern void make_standard_label(QLabel *this_label, QString title, byte preset_color);
extern void update_char_screen(QWidget *return_widget);
#endif // PLAYER_SCREEN_H
