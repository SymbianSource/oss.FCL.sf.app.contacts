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

#include "cntsettingsview.h"
#include "cntsettingsmodel.h"
#include "cntdebug.h"

#include <hbdocumentloader.h>
#include <hbview.h>
#include <hbdataform.h>
#include <hbaction.h>

const char *CNT_SETTINGS_XML = ":/xml/contacts_settings.docml";

/*!
Constructor
*/
CntSettingsView::CntSettingsView() :
    QObject(),
    mView( NULL ),
    mDoc( NULL ),
    mForm( NULL ),
    mViewMgr( NULL ),
    mModel( NULL )
{
    CNT_ENTRY
    
    bool ok;
    document()->load(CNT_SETTINGS_XML, &ok);
    if (!ok)
    {
        qFatal("Unable to read %S", CNT_SETTINGS_XML);
    }
    
    mView = static_cast<HbView*> (document()->findWidget(QString("view")));
    mForm = static_cast<HbDataForm*> (document()->findWidget(QString("dataForm")));
    mForm->setItemRecycling(true);
    
	mModel = new CntSettingsModel();
    mForm->setModel( mModel );
    
    mBack = new HbAction(Hb::BackNaviAction, mView);
    connect( mBack, SIGNAL(triggered()), this, SLOT(showPreviousView()) );
    
    CNT_EXIT
}

/*!
Destructor
*/
CntSettingsView::~CntSettingsView()
{
    CNT_ENTRY
    
    mView->deleteLater();
    delete mForm;
    delete mModel;
    delete mDoc;
    
    CNT_EXIT
}

/*!
Called when view is activated
*/
void CntSettingsView::activate( const CntViewParameters aArgs )
{
    CNT_ENTRY
    
    mArgs = aArgs;
    mViewMgr = &mEngine->viewManager();
        
    if ( mView->navigationAction() != mBack)
    {
        mView->setNavigationAction(mBack);
    }
    
    CNT_EXIT
}

void CntSettingsView::deactivate()
{
}

/*!
Called when user presses back
*/
void CntSettingsView::showPreviousView()
{
    CNT_ENTRY
    
    mViewMgr->back( mArgs );
    
    CNT_EXIT
}

/*!
Returns the HbDocumentLoader pointer
*/
HbDocumentLoader* CntSettingsView::document() 
{
    CNT_ENTRY
    
    if ( !mDoc )
    {
        mDoc = new HbDocumentLoader();
    }
    
    CNT_EXIT
    return mDoc;
}
