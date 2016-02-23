/*
 * Copyright (c) 2015 Jeff Greene, Diego Gonzalez
 *
 * Please see copyright.txt for complete copyright and licensing restrictions.
 *
 */

#include <src/npp.h>
#include <src/qt_mainwindow.h>
#include <src/player_command.h>
#include <QHeaderView>
#include <QFontDialog>
#include <QPushButton>

void MainWindow::inven_button_click()
{
    QString item_id = QObject::sender()->objectName();

    bool ok;

    int splitter = item_id.indexOf("_");

    QString command_string = item_id;

    command_string.truncate(splitter);
    item_id.remove(0, splitter+1);

    int item_num = item_id.toInt(&ok, 10);
    // Paranoia
    if (!ok) return;
    int command_num = command_string.toInt(&ok, 10);
    // Paranoia
    if (!ok) return;

    // Hack = Special handling for object settings
    if (command_num == CMD_SETTINGS)
    {
        object_settings(item_num);
        p_ptr->message_append_stop();
        return;
    }

    p_ptr->message_append_start();

    // We aren't repeating the previous command
    p_ptr->player_previous_command_wipe();

    process_command(item_num, command_num);
    p_ptr->message_append_stop();

    win_char_inventory_update();
}

void MainWindow::toggle_inven_show_buttons()
{
    if (!inven_show_buttons)
    {
        inven_show_buttons = TRUE;
        char_inventory_buttons_act->setText("Hide Command Buttons");
    }
    else
    {
        inven_show_buttons = FALSE;
        char_inventory_buttons_act->setText("Show Command Buttons");
    }
    win_char_inventory_update();
}

void MainWindow::update_label_inventory_font()
{
    QList<QLabel *> lbl_list = window_char_inventory->findChildren<QLabel *>();
    for (int i = 0; i < lbl_list.size(); i++)
    {
        QLabel *this_lbl = lbl_list.at(i);
        this_lbl->setFont(char_inventory_settings.win_font);
    }

    // Now make the buttons about the same size
    QFontMetrics metrics(char_inventory_settings.win_font);
    QSize button_size((metrics.width('M') + 2), (metrics.height() + 2));
    QList<QPushButton *> pushbutton_list = window_char_inventory->findChildren<QPushButton *>();
    for (int i = 0; i < pushbutton_list.size(); i++)
    {
        QPushButton *this_button = pushbutton_list.at(i);
        if (!this_button->objectName().length()) continue;
        this_button->setIconSize(button_size);
    }
}

void MainWindow::set_font_char_inventory(QFont newFont)
{
    char_inventory_settings.win_font = newFont;
    update_label_inventory_font();

}

void MainWindow::win_char_inventory_font()
{
    bool selected;
    QFont font = QFontDialog::getFont( &selected, char_inventory_settings.win_font, this);

    if (selected)
    {
        set_font_char_inventory(font);
    }
}

void MainWindow::inven_link_pushbuttons()
{
    QList<QPushButton *> pushbutton_list = window_char_inventory->findChildren<QPushButton *>();

    for (int i = 0; i < pushbutton_list.size(); i++)
    {
        QPushButton *this_button = pushbutton_list.at(i);
        if (!this_button->objectName().length()) continue;
        connect(this_button, SIGNAL(pressed()), this, SLOT(inven_button_click()));
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_char_inventory_wipe()
{
    if (!char_inventory_settings.win_show) return;
    if (!character_generated) return;
    clear_layout(main_vlay_inventory);
}


void MainWindow::win_char_inventory_update()
{
    if (!character_generated) return;
    if (!char_inventory_settings.win_show) return;

    update_inven_list(inven_list, TRUE, inven_show_buttons);
    if (inven_show_buttons) inven_link_pushbuttons();
    update_label_inventory_font();
}

void MainWindow::create_win_char_inventory()
{
    if (!character_generated) return;
    if (!char_inventory_settings.win_show) return;

    // Add the inventory
    QPointer<QLabel> header_inven = new QLabel(QString("<b><h1>Inventory</b></h1>"));
    main_vlay_inventory->addWidget(header_inven, Qt::AlignCenter);
    inven_list = new QGridLayout;
    main_vlay_inventory->addLayout(inven_list);
    // I have no idea why a stretch of 1 doesn't work here.
    main_vlay_inventory->addStretch(1000);

    win_char_inventory_update();
}


/*
 *  Make the inven shell
 *  The game crashes if the labels are drawn before the character is created
 *  So that is filled after a character is created.
 */
void MainWindow::win_char_inventory_create()
{
    window_char_inventory = new QWidget();
    main_vlay_inventory = new QVBoxLayout;
    window_char_inventory->setLayout(main_vlay_inventory);

    char_inventory_menubar = new QMenuBar;
    main_vlay_inventory->setMenuBar(char_inventory_menubar);
    window_char_inventory->setWindowTitle("Character Inventory Screen");
    win_char_inventory_settings = char_inventory_menubar->addMenu(tr("&Settings"));
    char_inventory_font_act = new QAction(tr("Set Inventory Screen Font"), this);
    char_inventory_font_act->setStatusTip(tr("Set the font for the Inventory screen."));
    connect(char_inventory_font_act, SIGNAL(triggered()), this, SLOT(win_char_inventory_font()));
    win_char_inventory_settings->addAction(char_inventory_font_act);
    char_inventory_buttons_act = new QAction(tr("Show Command Buttons"), this);
    if (inven_show_buttons) char_inventory_buttons_act->setText("Hide Command Buttons");
    char_inventory_buttons_act->setStatusTip(tr("Displays or hides the command buttons."));
    connect(char_inventory_buttons_act, SIGNAL(triggered()), this, SLOT(toggle_inven_show_buttons()));
    win_char_inventory_settings->addAction(char_inventory_buttons_act);

    window_char_inventory->setAttribute(Qt::WA_DeleteOnClose);
    connect(window_char_inventory, SIGNAL(destroyed(QObject*)), this, SLOT(win_char_inventory_destroy(QObject*)));
}

/*
 * Win_char_inventory_close should be used when the game is shutting down.
 * Use this function for closing the window mid-game
 */
void MainWindow::win_char_inventory_destroy(QObject *this_object)
{
    (void)this_object;
    if (!char_inventory_settings.win_show) return;
    if (!window_char_inventory) return;
    char_inventory_settings.get_widget_settings(window_char_inventory);
    window_char_inventory->deleteLater();
    char_inventory_settings.win_show = FALSE;
    win_char_inventory_act->setText("Show Character Inventory Screen");
}

/*
 * This version should only be used when the game is shutting down.
 * So it is remembered if the window was open or not.
 * For closing the window mid-game use win_char_inventory_destroy directly
 */
void MainWindow::win_char_inventory_close()
{
    bool was_open = char_inventory_settings.win_show;
    win_char_inventory_destroy(window_char_inventory);
    char_inventory_settings.win_show = was_open;
}

void MainWindow::toggle_win_char_inventory_frame()
{
    if (!char_inventory_settings.win_show)
    {
        win_char_inventory_create();
        char_inventory_settings.win_show = TRUE;
        create_win_char_inventory();
        window_char_inventory->setGeometry(char_inventory_settings.win_geometry);
        win_char_inventory_act->setText("Hide Character Inventory Screen");
        if (char_inventory_settings.win_maximized) window_char_inventory->showMaximized();
        else window_char_inventory->show();
    }
    else win_char_inventory_destroy(window_char_inventory);
}



