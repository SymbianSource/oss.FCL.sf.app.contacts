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

#ifndef CNTDETAILEDITOR_H_
#define CNTDETAILEDITOR_H_

#include "cntbaseview.h"

#include <hbdataform.h>
#include <hbabstractviewitem.h>
#include "cntdetaileditormodel.h"

#include <qtcontacts.h>
#include <hbgroupbox.h>

class CntDetailEditor : public CntBaseView
    {
    Q_OBJECT
    
public:
    CntDetailEditor(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntDetailEditor();
    
private slots:
    void aboutToCloseView();
    void discardChanges();
    void insertField();
    
public:
    void setViewId( CntViewParameters::ViewId aId );
    void setHeader( QString aHeader );
    void setDetails( CntDetailEditorModel* aModel, HbAbstractViewItem* aPrototype );
    void setInsertAction( const QString aInsert );
    
public: // from CntBaseView
    void activateView( const CntViewParameters &viewParameters );
    CntViewParameters::ViewId viewId() const;
    
private:
    QPointer<HbDataForm> mDataForm;
    QPointer<CntDetailEditorModel> mDataFormModel;
    QPointer<HbAbstractViewItem> mPrototype;
    QPointer<HbGroupBox> mHeader;
    CntViewParameters::ViewId iId;
    };
#endif /* CNTDETAILEDITOR_H_ */
