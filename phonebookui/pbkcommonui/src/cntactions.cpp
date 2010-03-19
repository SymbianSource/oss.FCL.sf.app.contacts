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

#include "cntactions.h"

#include <hbtoolbar.h>
#include <hbmenu.h>


const char *CNT_ACTIONS_XML = ":/xml/contacts_actions.docml";

CntActions::CntActions(QObject *aParent)
{
    setParent(aParent);
    mActionList.clear();
    mStringActionMap.clear();

    bool ok = false;
    mDocumentLoader.load(CNT_ACTIONS_XML, &ok);
    if (!ok)
    {
        qFatal("Unable to read :/xml/contacts_actions.docml");
    }
}


CntActions::~CntActions()
{

}

HbAction *CntActions::baseAction(const QString& aName)
{
    if (mStringActionMap.contains(aName))
    {
        return mStringActionMap.value(aName);
    }

    HbAction *baseAction = qobject_cast<HbAction *>(mDocumentLoader.findObject(aName));

    if (baseAction)
    {
        mStringActionMap.insert(aName, baseAction);
        baseAction->setParent(this);
    }

    return baseAction;
}

void CntActions::addActionToToolBar(HbAction *aAction, HbToolBar *aToolBar)
{
    aToolBar->addAction(aAction);
}

void CntActions::addActionsToToolBar(HbToolBar *aToolBar)
{
    for (int i=0;i<mActionList.count();i++)
    {
        aToolBar->addAction(mActionList[i]);
    }
}

void CntActions::addActionsToMenu(HbMenu *aMenu)
{
    for (int i=0;i<mActionList.count();i++)
    {
        aMenu->addAction(mActionList[i]);
    }
}

void CntActions::addActionToMenu(HbAction *aAction, HbMenu *aMenu)
{
    aMenu->addAction(aAction);
}

QList<HbAction*> &CntActions::actionList()
{
    return mActionList;
}

void CntActions::clearActionList()
{
    mActionList.clear();
}


