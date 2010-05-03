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
    QList<QContactNote> noteList = mContact->details<QContactNote>();
    if ( noteList.isEmpty() )
        {
        QContactNote emptyNote;
        noteList.append( emptyNote );
        }
    
    HbDataFormModelItem* root = invisibleRootItem();
    foreach ( QContactNote note, noteList )
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
        QContactNote note = detail->detail();
        if ( note.note().length() > 0 ) {
            mContact->saveDetail( &note );
        }
    }
}
// End of File
