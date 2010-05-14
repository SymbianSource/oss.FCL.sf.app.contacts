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
#include "cntnoteeditormodel.h"
#include "cntdetailmodelitem.h"
#include <qcontactnote.h>

CntNoteEditorModel::CntNoteEditorModel( QContact* aContact ) :
CntDetailEditorModel( aContact )
    {
    HbDataFormModelItem* root = invisibleRootItem();
    foreach ( QContactNote note, mContact->details<QContactNote>() )
        {
        CntDetailModelItem* item = new CntDetailModelItem(note);
        appendDataFormItem( item, root );
        }
    }

CntNoteEditorModel::~CntNoteEditorModel()
    {
    }

void CntNoteEditorModel::insertDetailField()
{
    QContactNote emptyNote;
    appendDataFormItem( new CntDetailModelItem(emptyNote), invisibleRootItem() );
}

void CntNoteEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
      
    int count( root->childCount() );
    for ( int i(0); i < count; i++ ) {
        CntDetailModelItem* detail = static_cast<CntDetailModelItem*>( root->childAt(i) );
        QContactDetail note = detail->detail();
        mContact->saveDetail( &note );
        
        if ( note.value(QContactNote::FieldNote).isEmpty() )
        {
            mContact->removeDetail( &note );
        }
    }
}
// End of File
