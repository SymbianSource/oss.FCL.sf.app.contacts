/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 view stack element.
*
*/


#include "cpbk2filteredviewstackelement.h"

// Virtual Phonebook
#include <CVPbkContactFindPolicy.h>
#include <MVPbkContactViewFiltering.h>
#include <MVPbkContactViewBase.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPreCond_CreateViewL = 1,
    EBaseViewHasNoFilter,
    EViewAlreadyExists,
    EPreCond_UpdateFilterL,
    EPreCond_SetUnderDestruction,
    EPreCond_FindMustBeDigraphicL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2FilteredViewStackElement");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::CPbk2FilteredViewStackBaseElement
// --------------------------------------------------------------------------
//
inline CPbk2FilteredViewStackBaseElement::CPbk2FilteredViewStackBaseElement( 
        MVPbkContactViewBase& aBaseView ) :
        iBaseView( aBaseView )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::NewL
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStackBaseElement* CPbk2FilteredViewStackBaseElement::NewL( 
        MVPbkContactViewBase& aBaseView )
    {
    CPbk2FilteredViewStackBaseElement* self =
        new ( ELeave ) CPbk2FilteredViewStackBaseElement( aBaseView );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::CreateViewL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackBaseElement::CreateViewL( 
        MVPbkContactViewBase& /*aBaseView*/,
        MVPbkContactViewBase& /*aParentView*/,
        MVPbkContactViewObserver& /*aObserver*/ )
    {
    // Base element gets the view in constructor
    __ASSERT_DEBUG( EFalse, Panic( EViewAlreadyExists ) );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::View
// --------------------------------------------------------------------------
//  
MVPbkContactViewBase* CPbk2FilteredViewStackBaseElement::View() const
    {
    return &iBaseView;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::FindArray
// --------------------------------------------------------------------------
//  
const MDesCArray& CPbk2FilteredViewStackBaseElement::FindArray() const
    {
    CDesCArray* nil = NULL;
    return *nil;
    }
    
// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::Level
// --------------------------------------------------------------------------
//  
TInt CPbk2FilteredViewStackBaseElement::Level() const
    {
    // Level is always zero for base element
    return 0;
    }
    
// --------------------------------------------------------------------------
// CPbk2ViewStackBaseElement::AlwaysIncluded
// --------------------------------------------------------------------------
//  
const MVPbkContactBookmarkCollection* 
        CPbk2FilteredViewStackBaseElement::AlwaysIncluded() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::IsSame
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStackBaseElement::IsSame( 
        const MPbk2FilteredViewStackElement& aOther ) const
    {
    // For all contacts view element it enough to compare levels
    return aOther.Level() == Level();  
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::UpdateFilterL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackBaseElement::UpdateFilterL( 
        MVPbkContactViewBase& /*aBaseView*/,
        MVPbkContactViewObserver& /*aObserver*/,
        const MDesCArray& /*aSearchLevelArray*/,
        const MVPbkContactBookmarkCollection* /*aAlwaysIncluded*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EBaseViewHasNoFilter ) );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::ViewState
// --------------------------------------------------------------------------
//
MPbk2FilteredViewStackElement::TViewState 
        CPbk2FilteredViewStackBaseElement::ViewState() const
    {
    return iState;    
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::SetViewState
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackBaseElement::SetViewState( TViewState aState )
    {
    iState = aState;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::UnderDestruction
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStackBaseElement::UnderDestruction() const
    {
    // Base element can not be under destruction
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackBaseElement::SetUnderDestruction
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackBaseElement::SetUnderDestruction()
    {
    __ASSERT_DEBUG( EFalse, Panic( EPreCond_SetUnderDestruction ) );
    }
    
// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::CPbk2FilteredViewStackElement
// --------------------------------------------------------------------------
//
inline CPbk2FilteredViewStackElement::CPbk2FilteredViewStackElement( 
        MVPbkContactFindPolicy& aFindPolicy,
        const MVPbkContactBookmarkCollection* aAlwaysIncluded,
        const MDesCArray* aSearchLevelArray )
        :   iFindPolicy( aFindPolicy ),
            iAlwaysIncluded( aAlwaysIncluded ),
            iSearchLevelArray( aSearchLevelArray )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackElement::ConstructL()
    {
    iLengthOfSearchArray = CountLengthOfStrings( *iSearchLevelArray );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::NewL
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStackElement* CPbk2FilteredViewStackElement::NewL( 
        MVPbkContactFindPolicy& aFindPolicy,
        const MDesCArray* aSearchLevelArray,
        const MVPbkContactBookmarkCollection* aAlwaysIncluded )
    {
    CPbk2FilteredViewStackElement* self = 
            new ( ELeave ) CPbk2FilteredViewStackElement( aFindPolicy, 
                aAlwaysIncluded, aSearchLevelArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::~CPbk2FilteredViewStackElement
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStackElement::~CPbk2FilteredViewStackElement()
    {
    delete iView;
    delete iSearchLevelArray; //from FilteredViewStack
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::CreateViewL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackElement::CreateViewL( 
        MVPbkContactViewBase& aBaseView,
        MVPbkContactViewBase& aParentView,
        MVPbkContactViewObserver& aObserver )
    {
    __ASSERT_DEBUG( !iView && aParentView.ViewFiltering(), 
            Panic ( EPreCond_CreateViewL ) );

    if( FindMustBeDigraphicL() )
        {
        PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
                ("FilteredViewStackElement::CreateViewL: Ortographic") );
        // Use base view as the filtering view.
        // This is needed to prevent accidentally filtering out contacts
        // that include diraphic characters. For example with Croatian
        // locale a contact name beginning with "nj" character would
        // be filtered out with the (parent) find filter "n" and it would
        // not be shown with find filter "nj".
        iView = aBaseView.ViewFiltering()->CreateFilteredViewLC( 
            aObserver, *iSearchLevelArray, iAlwaysIncluded );
        }
    else
        {
        PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
                ("FilteredViewStackElement::CreateViewL: No ortographic") );
        iView = aParentView.ViewFiltering()->CreateFilteredViewLC( 
            aObserver, *iSearchLevelArray, iAlwaysIncluded );  
        }

    CleanupStack::Pop(); // iView
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::View
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPbk2FilteredViewStackElement::View() const
    {
    return iView;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::FindArray
// --------------------------------------------------------------------------
//
const MDesCArray& CPbk2FilteredViewStackElement::FindArray() const
    {
    return *iSearchLevelArray;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::FindMustBeDigraphicL
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStackElement::FindMustBeDigraphicL() const
    {
    __ASSERT_DEBUG( iSearchLevelArray, Panic( EPreCond_FindMustBeDigraphicL ) );

    TBool isDigraphic( EFalse );

    // Go through the find string tokens one-by-one and check if they
    // include digraphs
    for ( TInt i(0); i < iSearchLevelArray->MdcaCount(); i++ )
        {
        TPtrC findStringToken = iSearchLevelArray->MdcaPoint( i );
        const TInt KDigraphLength(2);
        const TInt tokenLength( findStringToken.Length() );
        // No need to check single character find strings
        if( tokenLength >= KDigraphLength )
            {
            TPtrC substring = findStringToken.Left( tokenLength - 1 );
            if( !iFindPolicy.MatchRefineL( findStringToken, substring ) )
                {
                // The substring did not match the characters of the find
                // string token -> the find string token must include digraphs
                // For example with Croatian locale the find string "nj" does
                // not include the substring "n" because "nj" is a digraph
                isDigraphic = ETrue;
                break;
                }
            }
        }

    return isDigraphic;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::Level
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStackElement::Level() const
    {
    return iLengthOfSearchArray;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::AlwaysIncluded
// --------------------------------------------------------------------------
//
const MVPbkContactBookmarkCollection* 
        CPbk2FilteredViewStackElement::AlwaysIncluded() const
    {
    return iAlwaysIncluded;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::IsSame
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStackElement::IsSame( 
        const MPbk2FilteredViewStackElement& aOther ) const
    {
    // Elements are same if:
    // 1) Filter level is same
    // 2) Find texts match
    return ( aOther.Level() == Level() && 
             Match( aOther.FindArray(), FindArray() ) );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::UpdateFilterL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackElement::UpdateFilterL(
        MVPbkContactViewBase& aBaseView,
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aSearchLevelArray,
        const MVPbkContactBookmarkCollection* aAlwaysIncluded )
    {
    __ASSERT_DEBUG( iView, Panic( EPreCond_UpdateFilterL ) );

    // update find text, first copy it to new array
    TInt count = aSearchLevelArray.MdcaCount();
    CDesCArrayFlat* temp = new ( ELeave ) CDesCArrayFlat( count );
    
    for ( TInt i = 0; i < count; ++i )
        {
        temp->AppendL( aSearchLevelArray.MdcaPoint( i ) );
        }

    delete iSearchLevelArray;
    iSearchLevelArray = temp;

    if( FindMustBeDigraphicL() )
        {
        delete iView;
        iView = 0;
        iView = aBaseView.ViewFiltering()->CreateFilteredViewLC( 
            aObserver, *iSearchLevelArray, iAlwaysIncluded );
        CleanupStack::Pop(); // iView
        }
    else
        {
        iView->ViewFiltering()->UpdateFilterL( 
            *iSearchLevelArray, aAlwaysIncluded );
        iState = EUpdating;
        }
    }


// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::ViewState
// --------------------------------------------------------------------------
//
MPbk2FilteredViewStackElement::TViewState 
        CPbk2FilteredViewStackElement::ViewState() const
    {
    return iState;    
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::SetViewState
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackElement::SetViewState( TViewState aState )
    {
    iState = aState;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::UnderDestruction
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStackElement::UnderDestruction() const
    {
    return iUnderDestruction;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::SetUnderDestruction
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackElement::SetUnderDestruction()
    {
    iUnderDestruction = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::Match
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStackElement::Match( const MDesCArray& aArray1, 
        const MDesCArray& aArray2 ) const
    {
    TBool isMatch = EFalse;    
    TInt count = aArray1.MdcaCount();
    if ( count == aArray2.MdcaCount() )
        {
        for ( TInt i = 0; i < count; ++i )
            {
            if ( aArray1.MdcaPoint( i ).CompareC( 
                 aArray2.MdcaPoint( i ) ) == 0 )
                {
                isMatch = ETrue;
                }
            else
                {
                isMatch = EFalse;
                break;
                }
            }
        }
    return isMatch;
    }
    
// --------------------------------------------------------------------------
// CPbk2FilteredViewStackElement::CountLengthOfStrings
// Counts characters of array of strings
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStackElement::CountLengthOfStrings( const MDesCArray& 
    aStringArray ) const
    {
    TInt stringCount = aStringArray.MdcaCount();
    TInt totalLength = 0;
    for ( TInt i = 0; i < stringCount; ++i )
        {
        totalLength += aStringArray.MdcaPoint( i ).Length();
        }
        
    return totalLength;
    }
    
// End of File
