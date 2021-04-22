//  This file is part of DigiPlot.
//
//  DigiPlot is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  DigiPlot is distributed in the hope that it will be useful,
#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "qdigiplot.h"
#include "view.h"
#include "scene.h"
#include "cross.h"

using namespace std;

View::View(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(Raised | StyledPanel);
    graphicsView = new QGraphicsView;
    graphicsView->setRenderHint(QPainter::Antialiasing, false);
    graphicsView->setCursor(QCursor(Qt::CrossCursor));

    QToolButton *zoomInIcon = new QToolButton;
    zoomInIcon->setAutoRepeat(true);
    zoomInIcon->setAutoRepeatInterval(33);
    zoomInIcon->setAutoRepeatDelay(0);
    zoomInIcon->setIcon(QPixmap(":/images/zoom_in.png"));
    zoomInIcon->setIconSize(QSize(16,16));
    zoomInIcon->setStatusTip(tr("Increase Zoom with 1%"));
    QToolButton *zoomOutIcon = new QToolButton;
    zoomOutIcon->setAutoRepeat(true);
    zoomOutIcon->setAutoRepeatInterval(33);
    zoomOutIcon->setAutoRepeatDelay(0);
    zoomOutIcon->setIcon(QPixmap(":/images/zoom_out.png"));
    zoomOutIcon->setIconSize(QSize(16,16));
    zoomOutIcon->setStatusTip(tr("Decrease Zoom with 1%"));
    zoomSlider = new QSlider;
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(200);
    zoomSlider->setValue(100);
    zoomSlider->setTickPosition(QSlider::TicksRight);

    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
    zoomSliderLayout->addWidget(zoomInIcon);
    zoomSliderLayout->addWidget(zoomSlider);
    zoomSliderLayout->addWidget(zoomOutIcon);

    QToolButton *rotateLeftIcon = new QToolButton;
    rotateLeftIcon->setIcon(QPixmap(":/images/arrow_rotate_anticlockwise.png"));
    rotateLeftIcon->setIconSize(QSize(16,16));
    rotateLeftIcon->setStatusTip(tr("Rotate image one degree counter clockwise"));
    QToolButton *rotateRightIcon = new QToolButton;
    rotateRightIcon->setIcon(QPixmap(":/images/arrow_rotate_clockwise.png"));
    rotateRightIcon->setIconSize(QSize(16,16));
    rotateRightIcon->setStatusTip(tr("Rotate image one degree clockwise"));
    rotateSlider = new QSlider;
    rotateSlider->setOrientation(Qt::Horizontal);
    rotateSlider->setMinimum(-90);
    rotateSlider->setMaximum(90);
    rotateSlider->setValue(0);
    rotateSlider->setTickPosition(QSlider::TicksBelow);

    // Rotate slider layout
    QHBoxLayout *rotateSliderLayout = new QHBoxLayout;
    rotateSliderLayout->addWidget(rotateLeftIcon);
    rotateSliderLayout->addWidget(rotateSlider);
    rotateSliderLayout->addWidget(rotateRightIcon);

    resetButton = new QToolButton;
    resetButton->setIcon(QPixmap(":/images/anchor.png"));
    resetButton->setIconSize(QSize(16,16));
    resetButton->setStatusTip(tr("Reset zoom and rotation to normal"));
    resetButton->setEnabled(false);

    // Label layout
    QHBoxLayout *labelLayout = new QHBoxLayout;

    xAxisButton = new QToolButton;
    xAxisButton->setIcon(QPixmap(":/images/flag_blue.png"));
    xAxisButton->setIconSize(QSize(16,16));
    xAxisButton->setStatusTip(tr("Redefine X axis"));
    xAxisButton->setEnabled(false);
    yAxisButton = new QToolButton;
    yAxisButton->setIcon(QPixmap(":/images/flag_pink.png"));
    yAxisButton->setIconSize(QSize(16,16));
    yAxisButton->setStatusTip(tr("Redefine Y axis"));
    yAxisButton->setEnabled(false);

    xLogBox = new QCheckBox;
    xLogBox->setText(tr("log"));
    yLogBox = new QCheckBox;
    yLogBox->setText(tr("log"));
    xeLineEdit = new QLineEdit;
    xeLineEdit->setText(tr("x0"));
    xeLineEdit->resize(48,16);
    xtLineEdit = new QLineEdit;
    xtLineEdit->setText(tr("x1"));
    xtLineEdit->resize(48,16);
    yeLineEdit = new QLineEdit;
    yeLineEdit->setText(tr("y0"));
    yeLineEdit->resize(48,16);
    ytLineEdit = new QLineEdit;
    ytLineEdit->setText(tr("y1"));
    ytLineEdit->resize(48,16);

    labelLayout->addWidget(xAxisButton);
    labelLayout->addWidget(xLogBox);
    labelLayout->addWidget(xeLineEdit);
    labelLayout->addWidget(xtLineEdit);
    labelLayout->addStretch();
    labelLayout->addWidget(yAxisButton);
    labelLayout->addWidget(yLogBox);
    labelLayout->addWidget(yeLineEdit);
    labelLayout->addWidget(ytLineEdit);

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addLayout(labelLayout, 0, 0);
    topLayout->addWidget(graphicsView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->addLayout(rotateSliderLayout, 2, 0);
    topLayout->addWidget(resetButton, 2, 1);
    setLayout(topLayout);

    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetView()));
    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(graphicsView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));
    connect(graphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));
    connect(rotateLeftIcon, SIGNAL(clicked()), this, SLOT(rotateLeft()));
    connect(rotateRightIcon, SIGNAL(clicked()), this, SLOT(rotateRight()));
    connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));

    setupMatrix();
}

QGraphicsView *View::view() const
{
    return graphicsView;
}

void View::resetView()
{
    zoomSlider->setValue(100);
    rotateSlider->setValue(0);
    setupMatrix();
    graphicsView->ensureVisible(QRectF(0, 0, 0, 0));

    resetButton->setEnabled(false);
}

void View::setResetButtonEnabled()
{
    resetButton->setEnabled(true);
}

void View::setupMatrix()
{
    qreal scale = ::pow(2.0, (zoomSlider->value() - 100) / 50.0);

    QMatrix matrix;
    matrix.scale(scale, scale);
    matrix.rotate(rotateSlider->value());

    graphicsView->setMatrix(matrix);
    setResetButtonEnabled();
}

void View::print()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        graphicsView->render(&painter);
    }
}

void View::zoomIn()
{
    zoomSlider->setValue(zoomSlider->value() + 10);
}

void View::zoomOut()
{
    zoomSlider->setValue(zoomSlider->value() - 10);
}

void View::zoom25()
{
    zoomSlider->setValue(0);
}

void View::zoom50()
{
    zoomSlider->setValue(50);
}

void View::zoom100()
{
    zoomSlider->setValue(100);
}

void View::zoom200()
{
    zoomSlider->setValue(150);
}

void View::zoom400()
{
    zoomSlider->setValue(200);
}


void View::rot90()
{
    rotateSlider->setValue(rotateSlider->value() + 90);
}

void View::rotminus90()
{
    rotateSlider->setValue(rotateSlider->value() - 90);
}

void View::rotateLeft()
{
    rotateSlider->setValue(rotateSlider->value() - 1);
}

void View::rotateRight()
{
    rotateSlider->setValue(rotateSlider->value() + 1);
}
