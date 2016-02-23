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


void MainWindow::set_font_win_obj_recall(QFont newFont)
{
    win_obj_recall_settings.win_font = newFont;
    win_obj_recall_update();
}

void MainWindow::win_obj_recall_font()
{
    bool selected;
    QFont font = QFontDialog::getFont(&selected, win_obj_recall_settings.win_font, this );

    if (selected)
    {
        set_font_win_obj_recall(font);
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_obj_recall_wipe()
{
    if (!win_obj_recall_settings.win_show) return;
    if (!character_generated) return;

    obj_recall_area->clear();

}

void MainWindow::win_obj_recall_update()
{
    win_obj_recall_wipe();
    if (!win_obj_recall_settings.win_show) return;
    if (!character_generated) return;
    if (!p_ptr->object_kind_idx && !p_ptr->object_idx) return;

    obj_recall_area->setFont(win_obj_recall_settings.win_font);
    obj_recall_area->moveCursor(QTextCursor::Start);

    QString obj_recall;

    if (p_ptr->object_idx)
    {
        object_type *o_ptr = object_from_item_idx(p_ptr->object_idx);
        obj_recall = get_object_description(o_ptr);
    }
    else // p_ptr->object_kind_idx
    {
        s16b k_idx = p_ptr->object_kind_idx;

        // Initialize and prepare a fake object
        object_type object;
        object_type *o_ptr = &object;
        o_ptr->object_wipe();
        object_prep(o_ptr, k_idx);

        if (k_info[k_idx].aware) o_ptr->ident |= (IDENT_STORE);

        /* draw it */
        obj_recall = get_object_description(o_ptr);
    }


    obj_recall_area->insertHtml(obj_recall);
}


/*
 *  Show widget is called after this to allow
 * the settings to restore the save geometry.
 */
void MainWindow::win_obj_recall_create()
{
    window_obj_recall = new QWidget();
    obj_recall_vlay = new QVBoxLayout;
    window_obj_recall->setLayout(obj_recall_vlay);
    obj_recall_area = new QTextEdit;
    obj_recall_area->setReadOnly(TRUE);
    obj_recall_area->setStyleSheet("background-color: lightGray;");
    obj_recall_area->setTextInteractionFlags(Qt::NoTextInteraction);
    obj_recall_vlay->addWidget(obj_recall_area);
    obj_recall_menubar = new QMenuBar;
    obj_recall_vlay->setMenuBar(obj_recall_menubar);
    window_obj_recall->setWindowTitle("Object Recall Window");
    win_obj_recall_win_settings = obj_recall_menubar->addMenu(tr("&Settings"));
    obj_recall_set_font_act = new QAction(tr("Set Object Recall Font"), this);
    obj_recall_set_font_act->setStatusTip(tr("Set the font for the Object Recall Window."));
    connect(obj_recall_set_font_act, SIGNAL(triggered()), this, SLOT(win_obj_recall_font()));
    win_obj_recall_win_settings->addAction(obj_recall_set_font_act);

    window_obj_recall->setAttribute(Qt::WA_DeleteOnClose);
    connect(window_obj_recall, SIGNAL(destroyed(QObject*)), this, SLOT(win_obj_recall_destroy(QObject*)));
}



/*
 * win_obj_recall_close should be used when the game is shutting down.
 * Use this function for closing the window mid-game
 */
void MainWindow::win_obj_recall_destroy(QObject *this_object)
{
    (void)this_object;
    if (!win_obj_recall_settings.win_show) return;
    if (!window_obj_recall) return;
    win_obj_recall_settings.get_widget_settings(window_obj_recall);
    window_obj_recall->deleteLater();
    win_obj_recall_settings.win_show = FALSE;
    win_obj_recall_act->setText("Show Object Recall Window");
}

/*
 * This version should only be used when the game is shutting down.
 * So it is remembered if the window was open or not.
 * For closing the window mid-game use win_obj_recall_destroy directly
 */
void MainWindow::win_obj_recall_close()
{
    bool was_open = win_obj_recall_settings.win_show;
    win_obj_recall_destroy(window_obj_recall);
    win_obj_recall_settings.win_show = was_open;
}

void MainWindow::toggle_win_obj_recall()
{
    if (!win_obj_recall_settings.win_show)
    {
        win_obj_recall_create();
        win_obj_recall_settings.win_show = TRUE;
        window_obj_recall->setGeometry(win_obj_recall_settings.win_geometry);
        win_obj_recall_act->setText("Hide Object Recall Window");
        if (win_obj_recall_settings.win_maximized) window_obj_recall->showMaximized();
        else window_obj_recall->show();

        win_obj_recall_update();
    }
    else win_obj_recall_destroy(window_obj_recall);
}

