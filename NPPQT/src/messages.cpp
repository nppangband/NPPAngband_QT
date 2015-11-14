/*
 * Copyright (c) 2015 Jeff Greene, Diego Gonzalez
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 3, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include "messages.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <src/utilities.h>



// Holds all of the messages.
QVector<message_type> message_list;

DisplayMessages::DisplayMessages(void)
{
    QVBoxLayout *main_layout = new QVBoxLayout;
    QTextEdit *message_area = new QTextEdit;

    main_layout->addWidget(message_area);

    message_area->setReadOnly(true);
    message_area->setStyleSheet("background-color: black;");

    update_message_area(message_area, 400, ui_message_window_font());

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
    main_layout->addWidget(buttons);

    setLayout(main_layout);
    setWindowTitle(tr("Messages"));

    QSize this_size = QSize(width()* 4 / 3, height() * 3 / 2);
    resize(ui_max_widget_size(this_size));
    updateGeometry();

    this->exec();
}

void update_message_area(QTextEdit *message_area, int max_messages, QFont message_font)
{
    int num_messages = 0;

    message_area->clear();
    QString next_message;
    next_message.clear();

    message_area->setFont(message_font);

    for (int i = 0; i < message_list.size(); i++)
    {
        QString this_message = message_list[i].message;

        if (message_list[i].repeats > 1)
        {
           this_message.append(QString(" (x%1)") .arg(message_list[i].repeats));
        }

        next_message.prepend(this_message);

        // See if the next message should go before this one.
        if ((i+1) < message_list.size())
        {
            if (message_list[i+1].append)
            {
                next_message.prepend("  ");
                continue;
            }
        }

        // Add a linebreak if needed.
        if (num_messages) next_message.append("\n");

        message_area->moveCursor(QTextCursor::Start);
        message_area->setTextColor(message_list[i].msg_color);
        message_area->insertPlainText(QString("%1") .arg(next_message));

        next_message.clear();

        num_messages++;

        if (num_messages >= max_messages) break;
    }

    message_area->moveCursor(QTextCursor::End);
}

void update_message_area(QLabel *message_label, int max_messages)
{
    int num_messages = 0;

    QString next_message;
    next_message.clear();

    QString output;
    output.clear();

    message_label->setFont(ui_message_window_font());

    for (int i = 0; i < message_list.size(); i++)
    {
        QString this_message = message_list[i].message;

        if (message_list[i].repeats > 1)
        {
           this_message.append(QString(" (x%1)") .arg(message_list[i].repeats));
        }

        next_message.prepend(color_string(this_message, message_list[i].msg_color));

        // See if the next message should go before this one.
        if ((i+1) < message_list.size())
        {

            if (message_list[i+1].append)
            {
                next_message.prepend("  ");
                continue;
            }
        }

        output.prepend(next_message);

        next_message.clear();

        num_messages++;

        if (num_messages >= max_messages) break;

        output.prepend("\n");
    }

    message_label->setText(output);
}

QString output_messages(byte max_messages)
{
    int num_messages = 0;

    QString next_message;
    next_message.clear();

    QString output;
    output.clear();

    for (int i = 0; i < message_list.size(); i++)
    {
        QString this_message = message_list[i].message;

        if (message_list[i].repeats > 1)
        {
           this_message.append(QString(" (x%1)") .arg(message_list[i].repeats));
        }

        next_message.prepend(color_string(this_message, message_list[i].msg_color));

        // See if the next message should go before this one.
        if ((i+1) < message_list.size())
        {

            if (message_list[i+1].append)
            {
                next_message.prepend("  ");
                continue;
            }
        }

        output.prepend(next_message);

        next_message.clear();

        num_messages++;

        if (num_messages >= max_messages) break;

        output.prepend("<br>");
    }

    return (output);
}

void display_message_log(void)
{
    DisplayMessages();
}

/*
 * Add a message
 * This should be the only function to add to the message list, to make sure
 * it never gets larger than 200 messages.
 */
static void add_message_to_vector(QString msg, QColor which_color)
{
    message_type message_body;
    message_type *msg_ptr = &message_body;

    bool add_message = TRUE;

    // First make sure the message list gets no greater than 200
    while (message_list.size() >= 400)
    {
        message_list.removeLast();
    }

    // Without this check, the game will crash when adding the first message
    if (!message_list.empty())
    {
        // Point to the last message
        message_type *msg_one = &message_list[0];

        if (operator==(msg_one->message, msg) && (msg_one->msg_color == which_color) && !p_ptr->message_append)
        {
            msg_one->repeats++;
            add_message = FALSE;
        }
    }

    if (add_message)
    {
        // Default is a while message
        msg_ptr->msg_color = which_color;

        msg_ptr->message = msg;
        msg_ptr->message_turn = p_ptr->game_turn;
        msg_ptr->repeats = 1;
        if (!p_ptr->message_append) msg_ptr->append = FALSE;
        else msg_ptr->append = TRUE;

        // Add the message at the beginning of the list
        message_list.prepend(message_body);
    }

    p_ptr->redraw |= (PR_MESSAGES | PR_WIN_MESSAGES);

    redraw_stuff();
}

int html_length(QString this_string)
{
    int in_bracket = 0;
    int counter = 0;

    for (int space = 0; space < this_string.length(); space++)
    {
        if (this_string[space] == QChar('<')) in_bracket++;

        else if (this_string[space] == QChar('>')) in_bracket--;

        // Wait until we are outside a bracket and past the counter
        if (in_bracket) continue;
        counter++;
    }
    return (counter);
}



// This should be the only function sending messages to add_message_to_vector.
// This is so long messages are handled properly.
static void add_message(QString this_string, QColor this_color)
{
    if (html_length(this_string) < 70)
    {
        add_message_to_vector(this_string, this_color);
        return;
    }

    int findspace = 70;
    int in_bracket = 0;
    int counter = 0;

    // add the messages 80 characters at a time
    for (int space = 0; space < this_string.length(); space++)
    {
        if (this_string[space] == QChar('<')) in_bracket++;
        else if (this_string[space] == QChar('>')) in_bracket--;

        // Wait until we are outside a bracket and past the counter
        if (in_bracket) continue;
        counter++;
        if (counter < findspace) continue;

        // Looking for the next space
        if (this_string[space] != QChar(' ')) continue;

        add_message_to_vector(this_string.left(space), this_color);

        this_string.remove(0, space);

        space = 0;
    }

    // See if there is anything left
    if (this_string.length()) add_message_to_vector(this_string, this_color);
}

//  Add a message - assume the color of white
void message(QString msg)
{
    add_message(msg, add_preset_color(TERM_WHITE));
}

/*
 *  Add a message with a preset color.
 */
void color_message(QString msg, int which_color)
{
    QColor msg_color;

    //Paranoia
    if (which_color > MAX_COLORS) which_color = TERM_WHITE;

    // Default is a while message
    msg_color = defined_colors[which_color];

    add_message(msg, msg_color);
}


//  Add a message with any 24 bit color
void color_message(QString msg, byte red, byte green, byte blue)
{
    QColor msg_color;

    msg_color.setRgb(red, green, blue, 255);

    add_message(msg, msg_color);
}

//  Add a message with any 24 bit color
void color_message(QString msg, QColor msg_color)
{

    add_message(msg, msg_color);
}
