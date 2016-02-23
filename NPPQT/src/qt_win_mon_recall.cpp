/*
 * Copyright (c) 2015 Jeff Greene, Diego Gonzalez
 *
 * Please see copyright.txt for complete copyright and licensing restrictions.
 *
 */

#include <src/npp.h>
#include <src/qt_mainwindow.h>
#include <QHeaderView>
#include <QFontDialog>

void MainWindow::set_font_win_mon_recall(QFont newFont)
{
    win_mon_recall_settings.win_font = newFont;
    win_mon_recall_update();
}

void MainWindow::win_mon_recall_font()
{
    bool selected;
    QFont font = QFontDialog::getFont(&selected, win_mon_recall_settings.win_font, this );

    if (selected)
    {
        set_font_win_mon_recall(font);
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_mon_recall_wipe()
{
    if (!win_mon_recall_settings.win_show) return;
    if (!character_generated) return;

    mon_recall_area->clear();

}

void MainWindow::win_mon_recall_update()
{
    win_mon_recall_wipe();
    if (!win_mon_recall_settings.win_show) return;
    if (!character_generated) return;
    if (!p_ptr->monster_race_idx) return;


    mon_recall_area->setFont(win_mon_recall_settings.win_font);
    mon_recall_area->moveCursor(QTextCursor::Start);
    QString mon_recall = get_monster_description(p_ptr->monster_race_idx, FALSE, NULL, TRUE);
    mon_recall_area->insertHtml(mon_recall);
}


/*
 *  Show widget is called after this to allow
 * the settings to restore the save geometry.
 */
void MainWindow::win_mon_recall_create()
{
    window_mon_recall = new QWidget();
    mon_recall_vlay = new QVBoxLayout;
    window_mon_recall->setLayout(mon_recall_vlay);
    mon_recall_area = new QTextEdit;
    mon_recall_area->setReadOnly(TRUE);
    mon_recall_area->setStyleSheet("background-color: lightGray;");
    mon_recall_area->setTextInteractionFlags(Qt::NoTextInteraction);
    mon_recall_vlay->addWidget(mon_recall_area);
    mon_recall_menubar = new QMenuBar;
    mon_recall_vlay->setMenuBar(mon_recall_menubar);
    window_mon_recall->setWindowTitle("Monster Recall Window");
    win_mon_recall_win_settings = mon_recall_menubar->addMenu(tr("&Settings"));
    mon_recall_set_font_act = new QAction(tr("Set Monster Recall Font"), this);
    mon_recall_set_font_act->setStatusTip(tr("Set the font for the Monster Recall Window."));
    connect(mon_recall_set_font_act, SIGNAL(triggered()), this, SLOT(win_mon_recall_font()));
    win_mon_recall_win_settings->addAction(mon_recall_set_font_act);

    window_mon_recall->setAttribute(Qt::WA_DeleteOnClose);
    connect(window_mon_recall, SIGNAL(destroyed(QObject*)), this, SLOT(win_mon_recall_destroy(QObject*)));
}

/*
 * win_mon_recall_close should be used when the game is shutting down.
 * Use this function for closing the window mid-game
 */
void MainWindow::win_mon_recall_destroy(QObject *this_object)
{
    (void)this_object;
    if (!win_mon_recall_settings.win_show) return;
    if (!window_mon_recall) return;
    win_mon_recall_settings.get_widget_settings(window_mon_recall);
    window_mon_recall->deleteLater();
    win_mon_recall_settings.win_show = FALSE;
    win_mon_recall_act->setText("Show Monster Recall Window");
}

/*
 * This version should only be used when the game is shutting down.
 * So it is remembered if the window was open or not.
 * For closing the window mid-game use win_mon_list_destroy directly
 */
void MainWindow::win_mon_recall_close()
{
    bool was_open = win_mon_recall_settings.win_show;
    win_mon_recall_destroy(window_mon_recall);
    win_mon_recall_settings.win_show = was_open;
}


void MainWindow::toggle_win_mon_recall()
{
    if (!win_mon_recall_settings.win_show)
    {
        win_mon_recall_create();
        win_mon_recall_settings.win_show = TRUE;
        window_mon_recall->setGeometry(win_mon_recall_settings.win_geometry);
        win_mon_recall_act->setText("Hide Monster Recall Window");
        if (win_mon_recall_settings.win_maximized) window_mon_recall->showMaximized();
        else window_mon_recall->show();
        win_mon_recall_update();
    }
    else win_mon_recall_destroy(window_mon_recall);
}
