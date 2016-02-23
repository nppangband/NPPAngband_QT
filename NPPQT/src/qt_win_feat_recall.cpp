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

void MainWindow::set_font_win_feat_recall(QFont newFont)
{
    win_feat_recall_settings.win_font = newFont;
    win_feat_recall_update();
}

void MainWindow::win_feat_recall_font()
{
    bool selected;
    QFont font = QFontDialog::getFont(&selected, win_feat_recall_settings.win_font, this );

    if (selected)
    {
        set_font_win_feat_recall(font);
    }
}

// For when savefiles close but the game doesn't.
void MainWindow::win_feat_recall_wipe()
{
    if (!win_feat_recall_settings.win_show) return;
    if (!character_generated) return;

    feat_recall_area->clear();

}

void MainWindow::win_feat_recall_update()
{
    win_feat_recall_wipe();
    if (!win_feat_recall_settings.win_show) return;
    if (!character_generated) return;
    if (!p_ptr->feature_kind_idx) return;

    feat_recall_area->setFont(win_feat_recall_settings.win_font);
    feat_recall_area->moveCursor(QTextCursor::Start);

    QString feat_recall = get_feature_description(p_ptr->feature_kind_idx, FALSE, TRUE);
    feat_recall_area->insertHtml(feat_recall);
}



/*
 *  Show widget is called after this to allow
 * the settings to restore the save geometry.
 */
void MainWindow::win_feat_recall_create()
{
    window_feat_recall = new QWidget();
    feat_recall_vlay = new QVBoxLayout;
    window_feat_recall->setLayout(feat_recall_vlay);
    feat_recall_area = new QTextEdit;
    feat_recall_area->setReadOnly(TRUE);
    feat_recall_area->setStyleSheet("background-color: lightGray;");
    feat_recall_area->setTextInteractionFlags(Qt::NoTextInteraction);
    feat_recall_vlay->addWidget(feat_recall_area);
    win_feat_recall_menubar = new QMenuBar;
    feat_recall_vlay->setMenuBar(win_feat_recall_menubar);
    window_feat_recall->setWindowTitle("Feature Recall Window");
    win_feat_recall_win_settings = win_feat_recall_menubar->addMenu(tr("&Settings"));
    feat_recall_set_font_act = new QAction(tr("Set Feature Recall Font"), this);
    feat_recall_set_font_act->setStatusTip(tr("Set the font for the Feature Recall Window."));
    connect(feat_recall_set_font_act, SIGNAL(triggered()), this, SLOT(win_feat_recall_font()));
    win_feat_recall_win_settings->addAction(feat_recall_set_font_act);

    window_feat_recall->setAttribute(Qt::WA_DeleteOnClose);
    connect(window_feat_recall, SIGNAL(destroyed(QObject*)), this, SLOT(win_feat_recall_destroy(QObject*)));
}


void MainWindow::win_feat_recall_destroy(QObject *this_object)
{
    (void)this_object;
    if (!win_feat_recall_settings.win_show) return;
    if (!window_feat_recall) return;
    win_feat_recall_settings.get_widget_settings(window_feat_recall);
    window_feat_recall->deleteLater();
    win_feat_recall_settings.win_show = FALSE;
    win_feat_recall_act->setText("Show Feature Recall Window");
}

void MainWindow::win_feat_recall_close()
{
    bool was_open = win_feat_recall_settings.win_show;
    win_char_inventory_destroy(window_feat_recall);
    win_feat_recall_settings.win_show = was_open;
}

void MainWindow::toggle_win_feat_recall()
{
    if (!win_feat_recall_settings.win_show)
    {
        win_feat_recall_create();
        win_feat_recall_settings.win_show = TRUE;
        window_feat_recall->setGeometry(win_feat_recall_settings.win_geometry);
        win_feat_recall_act->setText("Hide Feature Recall Window");
        if (win_feat_recall_settings.win_maximized) window_feat_recall->showMaximized();
        else window_feat_recall->show();
        win_feat_recall_update();
    }
    else win_feat_recall_destroy(window_feat_recall);
}

