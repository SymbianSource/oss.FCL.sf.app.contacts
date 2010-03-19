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

#ifndef CNTNAMESVIEW_H
#define CNTNAMESVIEW_H

#include "cntbaselistview.h"

class HbSearchPanel;
class HbTextItem;

class CntNamesView : public CntBaseListView
{
    Q_OBJECT

public:
    CntNamesView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntNamesView();

public slots:
    void aboutToCloseView();
    void handleExecutedCommand(QString command, QContact contact);
    void showFind();
    void closeFind();
    void setFilter(const QString &filterString);
    void openCollections();

public:
    CntViewParameters::ViewId viewId() const { return CntViewParameters::namesView; }
    void activateView(const CntViewParameters &viewParameters);
    void deActivateView();

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    void addMenuItems();
    void addActionsToToolBar();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    HbSearchPanel *mSearchPanel;
    HbTextItem    *mEmptyListLabel;
};

#endif // CNTNAMESVIEW_H

// EOF
