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
* Description:  Composite contact view factory.
*
*/


#include "VPbkCompositeContactViewFactory.h"

#include "CVPbkOrderedCompositeContactView.h"
#include "CVPbkSortedCompositeContactView.h"

// --------------------------------------------------------------------------
// VPbkCompositeContactViewFactory::CreateViewLC
// --------------------------------------------------------------------------
//
CVPbkCompositeContactView* VPbkCompositeContactViewFactory::CreateViewLC(
        MVPbkContactViewObserver& aObserver,
        TVPbkContactViewSortPolicy aSortPolicy,
        const MVPbkFieldTypeList& aSortOrder,
        const MVPbkFieldTypeList& aFieldTypes,
        RFs& aFs)
    {
    CVPbkCompositeContactView* result = NULL;

    if ( aSortPolicy == EVPbkOrderedContactView )
        {
        result = CVPbkOrderedCompositeContactView::NewLC(
            aObserver, aSortOrder);
        }
    else
        {
        result = CVPbkSortedCompositeContactView::NewLC( 
            aObserver, aSortOrder, aFieldTypes, aFs);
        }

    return result;
    }

// End of File
