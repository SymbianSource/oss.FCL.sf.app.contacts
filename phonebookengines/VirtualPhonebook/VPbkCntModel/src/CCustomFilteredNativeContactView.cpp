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
* Description:  Contacts Model store custom filtered native contact
*                view implementation. Derives from CContactViewBase.
*
*/


#include "CCustomFilteredNativeContactView.h"

// VPbkCntModel
#include "CViewContact.h"
#include "CContactStore.h"
#include "CContactLink.h"

// VPbkEng
#include <MVPbkContactViewBase.h>

namespace VPbkCntModel {

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::CCustomFilteredNativeContactView
// --------------------------------------------------------------------------
//
inline CCustomFilteredNativeContactView::CCustomFilteredNativeContactView
          ( CContactStore& aContactStore ) :
                CContactViewBase( aContactStore.NativeDatabase() ),
                iContactStore( aContactStore )
    {
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::~CCustomFilteredNativeContactView
// --------------------------------------------------------------------------
//
CCustomFilteredNativeContactView::~CCustomFilteredNativeContactView()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::NewL
// --------------------------------------------------------------------------
//
CCustomFilteredNativeContactView* CCustomFilteredNativeContactView::NewL
        ( CContactStore& aContactStore )
    {
    CCustomFilteredNativeContactView* self =
        new ( ELeave ) CCustomFilteredNativeContactView( aContactStore );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::ConstructL
// --------------------------------------------------------------------------
//
void CCustomFilteredNativeContactView::ConstructL()
    {
    // Base construct
    CContactViewBase::ConstructL();
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::SetCustomFilterView
// --------------------------------------------------------------------------
//
void CCustomFilteredNativeContactView::SetCustomFilterView
        ( MVPbkContactViewBase& aFilteredView )
    {
    iFilteredView = &aFilteredView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::SetNativeBaseView
// --------------------------------------------------------------------------
//
void CCustomFilteredNativeContactView::SetNativeBaseView
        ( CContactViewBase& aNativeBaseView )
    {
    iNativeBaseView = &aNativeBaseView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::AtL
// --------------------------------------------------------------------------
//
TContactItemId CCustomFilteredNativeContactView::AtL( TInt aIndex ) const
    {
    const MVPbkViewContact& viewContact =
        iFilteredView->ContactAtL( aIndex );

    return static_cast<const CViewContact&>( viewContact ).Id();
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::ContactAtL
// --------------------------------------------------------------------------
//
const ::CViewContact& CCustomFilteredNativeContactView::ContactAtL
        ( TInt aIndex ) const
    {
    const MVPbkViewContact* viewContact =
        &iFilteredView->ContactAtL( aIndex );

    const CViewContact* cntModelStoreContact =
        static_cast<const CViewContact*>( viewContact );

    return *cntModelStoreContact->NativeContact();
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::CountL
// --------------------------------------------------------------------------
//
TInt CCustomFilteredNativeContactView::CountL() const
    {
    return iFilteredView->ContactCountL();
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::AllFieldsLC
// --------------------------------------------------------------------------
//
TInt CCustomFilteredNativeContactView::FindL( TContactItemId aId ) const
    {
    CContactLink* contactLink = CContactLink::NewLC( iContactStore, aId );
    TInt ret = iFilteredView->IndexOfLinkL( *contactLink );
    CleanupStack::PopAndDestroy( contactLink );
    return ret;
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::AllFieldsLC
// --------------------------------------------------------------------------
//
HBufC* CCustomFilteredNativeContactView::AllFieldsLC(
        TInt aIndex, const TDesC& aSeparator ) const
    {
    return iNativeBaseView->AllFieldsLC( aIndex, aSeparator );
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::ContactViewPreferences
// --------------------------------------------------------------------------
//
TContactViewPreferences
        CCustomFilteredNativeContactView::ContactViewPreferences()
    {
    return iNativeBaseView->ContactViewPreferences();
    }

// --------------------------------------------------------------------------
// CCustomFilteredNativeContactView::SortOrderL
// --------------------------------------------------------------------------
//
const RContactViewSortOrder&
        CCustomFilteredNativeContactView::SortOrderL() const
    {
    return iNativeBaseView->SortOrderL();
    }

} // namespace VPbkCntModel

// End of File
