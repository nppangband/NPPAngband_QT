#ifndef PLAYER_SCREEN_H
#define PLAYER_SCREEN_H

#include <QVBoxLayout>
#include <src/npp.h>
#include <QLabel>


struct player_flag_record
{
    QString name;           /* Name of resistance/ability */
    byte set;				/* Which field this resistance is in { 1 2 3 } */
    u32b res_flag;			/* resistance flag bit */
    u32b im_flag;			/* corresponding immunity bit, if any */
    bool moria_flag;		/* Is it used in Moria? */
    bool bad_flag;          // Is it a bad one?
};

class PlayerScreenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerScreenDialog(void);

private:
    void char_basic_info(QGridLayout *return_layout);
    void char_basic_data(QGridLayout *return_layout);
    void char_game_info(QGridLayout *return_layout);
    void char_stat_info(QGridLayout *stat_layout);
    void char_combat_info(QGridLayout *return_layout);
    void char_ability_info(QGridLayout *return_layout);
    void equip_resist_info(QGridLayout *return_layout, bool resists);
    void equip_stat_info(QGridLayout *return_layout);
};

extern void do_cmd_player_screen(void);
extern QString moria_speed_labels(int speed);
extern byte analyze_speed_bonuses(int speed, byte default_attr);

#endif // PLAYER_SCREEN_H
