/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef CNTDETAILPOPUP_H
#define CNTDETAILPOPUP_H

#include <QObject>
#include <hbdialog.h>

class QStandardItemModel;
class HbListView;

class CntDetailPopup : public HbDialog
{
    Q_OBJECT
public:
    static QString selectDetail(QStringList excludeList);

#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif
    void listItemSelected(QModelIndex index);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    CntDetailPopup(QStringList excludeList, QGraphicsItem *parent = 0);
    ~CntDetailPopup();

    QString selectedDetail();
    void addListItem(QString label, QString id1, QString id2 = "");

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    QStandardItemModel  *mListModel;
    HbListView          *mListView;
    QString              mSelectedDetail;
    QStringList          mExcludeList;
};

#endif // CNTDETAILPOPUP_H
