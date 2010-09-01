/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "TPbk2ContactIterator.h"

#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkTopContactManager.h>



void TPbk2ContactIterator::SetLinkArray( MVPbkContactLinkArray& aArray )
    {
    iLinkArray = &aArray;
    iIndexOfNext = 0;
    }

TBool TPbk2ContactIterator::HasNext() const
    {
    return iIndexOfNext < iLinkArray->Count();
    }

TBool TPbk2ContactIterator::HasPrevious() const
    {
    return iIndexOfNext > 0;
    }

MVPbkContactLink* TPbk2ContactIterator::NextL()
    {
    MVPbkContactLink* link = iLinkArray->At(iIndexOfNext).CloneLC();
    CleanupStack::Pop(); // link
    ++iIndexOfNext;
    return link; // give ownership
    }

MVPbkContactLink* TPbk2ContactIterator::PreviousL()
    {
    MVPbkContactLink* link = iLinkArray->At(iIndexOfNext - 1).CloneLC();
    CleanupStack::Pop(); // link
    --iIndexOfNext;
    return link; // give ownership    
    }

void TPbk2ContactIterator::SetToFirst()
    {
    iIndexOfNext = 0; // the first is at index 0
    }

void TPbk2ContactIterator::SetToLast()
    {
    iIndexOfNext = iLinkArray->Count() - 1;
    }


//  End of File
