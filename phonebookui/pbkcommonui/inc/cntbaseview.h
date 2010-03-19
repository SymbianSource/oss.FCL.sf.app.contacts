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

#ifndef CNTBASEVIEW_H
#define CNTBASEVIEW_H

#include <hbview.h>
#include <hbapplication.h>

#include "cntviewmanager.h"
#include "cntmodelprovider.h"
#include "cntactions.h"
#include "cntcommands.h"
#include "qtpbkglobal.h"
#include "cntdocumentloader.h"

// forward declarations
class HbAbstractViewItem;

QTM_USE_NAMESPACE

class QTPBK_EXPORT CntBaseView : public HbView
{
    Q_OBJECT

public:
    CntBaseView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    virtual ~CntBaseView();
    virtual void setupView();
    CntActions *actions();
    CntViewManager *viewManager();

public:
    virtual void deActivateView() { }
    virtual void activateView(const CntViewParameters &viewParameters){ Q_UNUSED(viewParameters); }
    virtual CntViewParameters::ViewId viewId() const = 0;

public slots:
    virtual void aboutToCloseView() { }
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void setOrientation(Qt::Orientation orientation) { Q_UNUSED(orientation); }

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif

    MobCntModel *contactModel();
    QContactManager *contactManager();

    //toolbar
    virtual void addActionsToToolBar() { }
    void addToolBar();
    void clearToolBar();
    virtual void setToolBarOrientation();

    // top right corner button
    virtual void addSoftkeyAction();

    // menu
    virtual void addMenuItems() { }

    CntCommands *commands();

    // XML related code
    CntDocumentLoader &documentLoader();
    bool loadDocument(const char *aDocument);
    bool loadDocument(const char *aDocument, const char *aSection);
    QGraphicsWidget* findWidget(const QString& aWidget);
    QObject* findObject(const QString& aObject);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    CntViewManager      *mViewManager;
    CntModelProvider    *mModelProvider;
    CntActions          *mActions;
    CntCommands         *mCommands;
    CntViewParameters   mViewParameters;
    HbAction            *mSoftKeyBackAction;
    CntDocumentLoader    mDocumentLoader;    

};

#endif // CNTBASEVIEW_H
