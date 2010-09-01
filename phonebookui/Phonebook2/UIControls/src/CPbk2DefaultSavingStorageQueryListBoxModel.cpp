/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Default saving storage settings item list box model for Phonebook 2.
*
*/


// INCLUDE FILES
#include "CPbk2DefaultSavingStorageQueryListBoxModel.h"   // This class

#include <badesca.h>
#include <AknUtils.h>

const TInt KBufferSize( 100 );

// ================= MEMBER FUNCTIONS =======================

CPbk2DefaultSavingStorageQueryListBoxModel*
        CPbk2DefaultSavingStorageQueryListBoxModel::NewLC
            (const CArrayPtr<HBufC>& aSelectionList)
    {
    CPbk2DefaultSavingStorageQueryListBoxModel* self = 
        new(ELeave)CPbk2DefaultSavingStorageQueryListBoxModel(aSelectionList);
    CleanupStack::PushL(self);
    self->ConstructL();    
    return self;                        
    }

CPbk2DefaultSavingStorageQueryListBoxModel*
        CPbk2DefaultSavingStorageQueryListBoxModel::NewL
            (const CArrayPtr<HBufC>& aSelectionList)
    {
    CPbk2DefaultSavingStorageQueryListBoxModel* self = NewLC(aSelectionList);
    CleanupStack::Pop(self);
    return self;                        
    }

CPbk2DefaultSavingStorageQueryListBoxModel::
        CPbk2DefaultSavingStorageQueryListBoxModel
        (const CArrayPtr<HBufC>& aSelectionList):
            iSelectionList( &aSelectionList )
    {
    }

void CPbk2DefaultSavingStorageQueryListBoxModel::ConstructL()
    {
    iBuffer = HBufC::NewL( KBufferSize );
    }
    
CPbk2DefaultSavingStorageQueryListBoxModel::
        ~CPbk2DefaultSavingStorageQueryListBoxModel()
    {    
    delete iBuffer;
    }    

TInt CPbk2DefaultSavingStorageQueryListBoxModel::MdcaCount() const
    {
    return iSelectionList->Count();
    }
    
TPtrC CPbk2DefaultSavingStorageQueryListBoxModel::MdcaPoint(TInt aIndex) const
    {
    TPtr ptr(iBuffer->Des());
    ptr.Zero();
    
    if (aIndex >= 0 &&
         aIndex < iSelectionList->Count())
        {
        ptr.Append( *iSelectionList->At(aIndex));
        }       

    AknTextUtils::LanguageSpecificNumberConversion(ptr);
    return ptr;
    }
    
//  End of File
