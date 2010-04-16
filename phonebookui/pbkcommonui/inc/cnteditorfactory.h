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

#ifndef CNTEDITORFACTORY_H_
#define CNTEDITORFACTORY_H_

#include <hbdataformviewitem.h>
#include <QObject>
#include "cntdetaileditormodel.h"
#include "cntviewparameters.h"
#include <qtcontacts.h>
#include "qtpbkglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntBaseView;
class CntViewManager;

class CntEditorFactory : public QObject 
    {
    Q_OBJECT
    
public:
    // ownership of aViewMgr is not transferred
    CntEditorFactory( CntViewManager* aViewMgr, QObject* aParent = 0 );
    virtual ~CntEditorFactory();
    
public:
    virtual CntBaseView* createNameEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createNumberEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createEmailEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createAddressEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createUrlEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createNoteEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createDateEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createCompanyEditorView( QContact aContact ); // ownership transferred
    virtual CntBaseView* createFamilyEditorView( QContact aContact ); // ownership transferred
    
protected:
    virtual CntBaseView* createEditor( 
        CntViewParameters::ViewId aId,
        CntDetailEditorModel* aModel, 
        HbDataFormViewItem* aProto, 
        QString aTitle, 
        QString aInsert=QString() );
    
protected:
    CntViewManager* iViewMgr; // not owned
    };
#endif /* CNTEDITORFACTORY_H_ */
