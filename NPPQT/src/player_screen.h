#ifndef PLAYER_SCREEN_H
#define PLAYER_SCREEN_H

#include <QVBoxLayout>
#include <src/npp.h>
#include <QLabel>


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

};

extern void do_cmd_player_screen(void);
extern QString moria_speed_labels(int speed);
extern byte analyze_speed_bonuses(int speed, byte default_attr);

#endif // PLAYER_SCREEN_H
