#include "nppdialog.h"
#include <QScrollArea>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include "npp.h"

NPPDialog::NPPDialog(QWidget *parent, int _padding, qreal _max_ratio) :
    QDialog(parent)
{
    padding = _padding;
    max_ratio = _max_ratio;

    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(false);

    QVBoxLayout *layout = new QVBoxLayout;
    this->setLayout(layout);

    layout->addWidget(scrollArea);

    layout->setContentsMargins(0, 0, 0, 0);

    viewport = 0;
}

QSize NPPDialog::sizeHint() const
{
    if (viewport == 0) return QSize(-1, -1);

    QSize size = viewport->sizeHint();

    QDesktopWidget dsk;

    QRect geo = dsk.screenGeometry();

    // Check screen size
    int w = geo.width() * max_ratio;
    int h = geo.height() * max_ratio;

    if (size.width() > w) {
        size.setWidth(w);
    }

    if (size.height() > h) {
        size.setHeight(h);
    }

    size += QSize(padding, padding);

    return size;
}

void NPPDialog::setWidget(QWidget *_viewport)
{
    viewport = _viewport;

    scrollArea->setWidget(viewport);

    this->updateGeometry();

    this->resize(this->sizeHint());
}
