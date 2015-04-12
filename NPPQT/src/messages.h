#ifndef MESSAGES_H
#define MESSAGES_H

#include <src/npp.h>
#include <QDialog>
#include <QTextEdit>
#include <QLabel>

typedef struct message_type message_type;

struct message_type
{
    QColor msg_color;
    QString message;
    u16b repeats;
    s32b message_turn;
    bool append;

};

class DisplayMessages : public QDialog
{
    Q_OBJECT
public:
    DisplayMessages(void);


};

extern QVector<message_type> message_list;

extern void update_message_area(QTextEdit *message_area, int max_messages, QFont message_font);
extern void update_message_area(QLabel *message_label, int max_messages);
extern void display_message_log(void);

#endif // MESSAGES_H
