#ifndef PLAYER_BIRTH_H
#define PLAYER_BIRTH_H

#include <src/player_screen.h>
#include <QButtonGroup>
#include <QLineEdit>


class PlayerBirth : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerBirth(bool quickstart);

    bool done_birth;
    bool quick_start;

private:    


    //Option checkboxes
    QVBoxLayout *vlay_options;
    QButtonGroup *group_options;
    void add_option_boxes(QVBoxLayout *return_layout);

    //Gender and buttons
    QVBoxLayout *vlay_gender;
    QButtonGroup *group_gender;
    QLineEdit *player_name;
    QString cur_name;
    int cur_gender;
    void add_genders(QVBoxLayout *return_layout);

    //Race
    QVBoxLayout *vlay_race;
    QButtonGroup *group_race;
    int cur_race;
    void add_races(QVBoxLayout *return_layout);

    //Class
    QVBoxLayout *vlay_class;
    QButtonGroup *group_class;
    int cur_class;
    void add_classes(QVBoxLayout *return_layout);

    //help boxes
    QVBoxLayout *vlay_help_area;
    QLabel *race_info;
    QLabel *class_info;
    void add_info_boxes(QVBoxLayout *return_layout);



private slots:
    void name_changed(QString new_name);
    void gender_changed(int new_gender);
    void race_changed(int new_race);
    void class_changed(int new_class);
    void option_changed(int index);
    void call_options_dialog(void);

    // Random char slots
    void random_name(void);
    void random_gender(void);
    void random_race(void);
    void random_class(void);

};


#endif // PLAYER_BIRTH_H
