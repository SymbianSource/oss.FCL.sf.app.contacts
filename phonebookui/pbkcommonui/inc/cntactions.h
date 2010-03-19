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

#ifndef CNTACTIONS_H
#define CNTACTIONS_H

#include "qtpbkglobal.h"

#include <QObject>
#include <QList>
#include <QMap>
#include <hbdocumentloader.h>
#include <hbaction.h>

class HbToolBar;
class HbMenu;

class QTPBK_EXPORT CntActions : public QObject
{
    Q_OBJECT

public slots:
signals:

public:

public:
    explicit CntActions(QObject *aParent);
    ~CntActions();

    QList<HbAction*> &actionList();
    void clearActionList();

    HbAction *baseAction(const QString& name);

    void addActionsToToolBar(HbToolBar *aToolBar);
    void addActionToToolBar(HbAction *aAction, HbToolBar *aToolBar);

    void addActionsToMenu(HbMenu *aMenu);
    void addActionToMenu(HbAction *aAction, HbMenu *aMenu);

private:

    QList<HbAction*>                            mActionList;
    QMap<QString, HbAction*>                    mStringActionMap;
    HbDocumentLoader                            mDocumentLoader;

private: // views , not own

};


#endif /* CNTACTIONS_H */
