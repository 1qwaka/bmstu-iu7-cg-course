#ifndef UIUTILS_H
#define UIUTILS_H

#include <QColorDialog>
#include <qpushbutton.h>
#include "colorop.h"


class UiUtils
{
public:
    UiUtils();

    static void color_button_setup_click(QObject* connector, QPushButton *but, QColor *connected_color, std::function<void()> callback = {})
    {
        connector->connect(but, &QPushButton::clicked, [=](){
            QColorDialog *dialog = new QColorDialog(*connected_color);
            connector->connect(dialog, &QColorDialog::colorSelected, [=](const QColor& col) {
                *connected_color = col;
                dialog->deleteLater();
                set_button_color(but, col);
                if (callback) {
                    callback();
                }
            });
            dialog->show();
        });
        set_button_color(but, *connected_color);


    }

    static void set_button_color(QPushButton *but, const QColor& col)
    {
        but->setText(col.name());
        but->setStyleSheet("background-color: " + col.name() + "; color:" + colorInv(col).name());
    }
};

#endif // UIUTILS_H
