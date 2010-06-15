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

#include <hbdocumentloader.h>
#include <hbview.h>
#include <hbdataform.h>
#include <hbaction.h>
#include "cntsettingsview.h"

const char *CNT_SETTINGS_XML = ":/xml/contacts_settings.docml";

CntSettingsView::CntSettingsView() : QObject(),
mView( NULL ),
mDoc( NULL ),
mForm( NULL ),
mViewMgr( NULL ),
mModel( NULL )
{
    bool ok;
    document()->load(CNT_SETTINGS_XML, &ok);
    if (!ok) {
        qFatal("Unable to read %S", CNT_SETTINGS_XML);
    }    
    mModel = new CntDefaultSettingsModel(); // default implementation
    
    mView = static_cast<HbView*> (document()->findWidget(QString("view")));
    mForm = static_cast<HbDataForm*> (document()->findWidget(QString("dataForm")));
    mForm->setItemRecycling(true);
    mForm->setModel( mModel );
    
    mBack = new HbAction(Hb::BackNaviAction, mView);
    connect( mBack, SIGNAL(triggered()), this, SLOT(back()) );       
}

CntSettingsView::~CntSettingsView()
{
    mView->deleteLater();
    delete mForm;
    delete mModel;
    delete mDoc;
}
    
void CntSettingsView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mViewMgr = aMgr;
    mArgs = aArgs;
    
    if ( mView->navigationAction() != mBack) {
        mView->setNavigationAction(mBack);
    }
    
    mModel->loadSettings();
}

void CntSettingsView::deactivate()
{
}

bool CntSettingsView::isDefault() const
{
    return false;
}

HbView* CntSettingsView::view() const
{
    return mView;
}

int CntSettingsView::viewId() const
{
    return settingsView;
}

void CntSettingsView::back()
{
    mModel->saveSettings();
    mViewMgr->back( mArgs );
}
HbDocumentLoader* CntSettingsView::document() 
{
    if ( !mDoc )
    {
        mDoc = new HbDocumentLoader();
    }
    return mDoc;
}
