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
#include <cntviewparams.h>

class QStandardItemModel;
class HbListView;

class CntDetailPopup : public HbDialog
{
    Q_OBJECT
public:
    static int selectDetail();

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
    CntDetailPopup(QGraphicsItem *parent = 0);
    ~CntDetailPopup();

    QString selectedDetail();
    void addListItem(QString aIcon, QString label, int aId);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    QStandardItemModel  *mListModel;
    HbListView          *mListView;
    QString              mSelectedDetail;
};

#endif // CNTDETAILPOPUP_H
