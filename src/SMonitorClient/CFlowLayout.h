﻿#ifndef CFLOWLAYOUT_H
#define CFLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>
//! [0]
class CFlowLayout : public QLayout
{
public:
    explicit CFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit CFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~CFlowLayout();

    void addItem(QLayoutItem *item);
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};
//! [0]

#endif // CFLOWLAYOUT_H
