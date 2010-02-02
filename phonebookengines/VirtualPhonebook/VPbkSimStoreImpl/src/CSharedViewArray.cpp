/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An array that manages shared views
*
*/


//  INCLUDES
#include "CSharedViewArray.h"

#include <CVPbkSimContactView.h>

// FORWARD DECLARATIONS

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

// -----------------------------------------------------------------------------
// CSharedViewArray::CSharedViewArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSharedViewArray::CSharedViewArray( 
        TViewDestructionPolicy aDestructionPolicy )
        :   iDestructionPolicy( aDestructionPolicy )
    {
    }

// -----------------------------------------------------------------------------
// CSharedViewArray::NewL
// -----------------------------------------------------------------------------
//
CSharedViewArray* CSharedViewArray::NewL( 
        TViewDestructionPolicy aDestructionPolicy )
    {
    return new (ELeave ) CSharedViewArray( aDestructionPolicy );
    }

// -----------------------------------------------------------------------------
// CSharedViewArray::~CSharedViewArray
// -----------------------------------------------------------------------------
//    
CSharedViewArray::~CSharedViewArray()
    {
    iSharedViews.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSharedViewArray::CreateNewHandleL
// -----------------------------------------------------------------------------
//        
CViewHandle* CSharedViewArray::CreateNewHandleL(
        const RVPbkSimFieldTypeArray& aSortOrder,
        TVPbkSimViewConstructionPolicy aConstructionPolicy, 
        MVPbkSimCntStore& aParentStore, const TDesC& aViewName )
    {
    CSharedViewOwner* viewOwner = FindOwner( aViewName );
    
    if ( !viewOwner )
        {
        CVPbkSimContactView* sharedView = 
                CVPbkSimContactView::NewLC( aSortOrder, aConstructionPolicy,
                aParentStore, aViewName, NULL );
        viewOwner = new (ELeave ) CSharedViewOwner( *this, sharedView );
        CleanupStack::Pop( sharedView );
        CleanupStack::PushL( viewOwner );
        iSharedViews.AppendL( viewOwner );
        CleanupStack::Pop( viewOwner );
        }
    
    return viewOwner->CreateNewHandleL();
    }

// -----------------------------------------------------------------------------
// CSharedViewArray::RemoveHandle
// -----------------------------------------------------------------------------
//        
void CSharedViewArray::RemoveHandle( CViewHandle& aHandle )
    {
    CSharedViewOwner* viewOwner = FindOwner( aHandle.SharedView().Name() );
    if ( viewOwner )
        {
        viewOwner->RemoveHandle( aHandle );
        if ( iDestructionPolicy == EDestroyViewIfNoHandles &&
             !viewOwner->HasHandles() )
            {
            TInt index = iSharedViews.Find( viewOwner );
            delete iSharedViews[index];
            iSharedViews.Remove( index );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSharedViewArray::FindOwner
// -----------------------------------------------------------------------------
//        
CSharedViewOwner* CSharedViewArray::FindOwner( const TDesC& aViewName )
    {
    CSharedViewOwner* viewOwner = NULL;
    const TInt count = iSharedViews.Count();
    for ( TInt i = 0; i < count && !viewOwner; ++i )
        {
        if ( iSharedViews[i]->View().Name().CompareC( aViewName ) == 0 )
            {
            viewOwner = iSharedViews[i];
            }
        }
    return viewOwner;
    }
    
// -----------------------------------------------------------------------------
// CViewHandle::CViewHandle
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CViewHandle::CViewHandle( CSharedViewArray& aSharedViewArray, 
        CVPbkSimContactView& aSharedView )
        :   iSharedViewArray( aSharedViewArray ),
            iSharedView( aSharedView )
    {
    }
    
// -----------------------------------------------------------------------------
// CViewHandle::~CViewHandle
// -----------------------------------------------------------------------------
//            
CViewHandle::~CViewHandle()
    {
    iSharedViewArray.RemoveHandle( *this );
    }

// -----------------------------------------------------------------------------
// CViewHandle::SharedView
// -----------------------------------------------------------------------------
//                
CVPbkSimContactView& CViewHandle::SharedView()
    {
    return iSharedView;
    }
    
// -----------------------------------------------------------------------------
// CViewHandle::Name
// -----------------------------------------------------------------------------
//                
const TDesC& CViewHandle::Name() const
    {
    return iSharedView.Name();
    }

// -----------------------------------------------------------------------------
// CViewHandle::ParentStore
// -----------------------------------------------------------------------------
//                    
MVPbkSimCntStore& CViewHandle::ParentStore() const
    {
    return iSharedView.ParentStore();
    }

// -----------------------------------------------------------------------------
// CViewHandle::OpenL
// -----------------------------------------------------------------------------
//                    
void CViewHandle::OpenL( MVPbkSimViewObserver& aObserver )
    {
    iSharedView.OpenL( aObserver );
    }

// -----------------------------------------------------------------------------
// CViewHandle::Close
// -----------------------------------------------------------------------------
//                        
void CViewHandle::Close( MVPbkSimViewObserver& aObserver )
    {
    iSharedView.Close( aObserver );
    }

// -----------------------------------------------------------------------------
// CViewHandle::CountL
// -----------------------------------------------------------------------------
//                            
TInt CViewHandle::CountL() const
    {
    return iSharedView.CountL();
    }

// -----------------------------------------------------------------------------
// CViewHandle::ContactAtL
// -----------------------------------------------------------------------------
//                                
MVPbkSimContact& CViewHandle::ContactAtL( TInt aIndex )
    {
    return iSharedView.ContactAtL( aIndex );
    }

// -----------------------------------------------------------------------------
// CViewHandle::ChangeSortOrderL
// -----------------------------------------------------------------------------
//                                    
void CViewHandle::ChangeSortOrderL( const RVPbkSimFieldTypeArray& aSortOrder )
    {
    iSharedView.ChangeSortOrderL( aSortOrder );
    }

// -----------------------------------------------------------------------------
// CViewHandle::MapSimIndexToViewIndexL
// -----------------------------------------------------------------------------
//                                        
TInt CViewHandle::MapSimIndexToViewIndexL( TInt aSimIndex )
    {
    return iSharedView.MapSimIndexToViewIndexL( aSimIndex );
    }
    
// -----------------------------------------------------------------------------
// CViewHandle::ContactMatchingPrefixL
// -----------------------------------------------------------------------------
//                                        
MVPbkSimStoreOperation* CViewHandle::ContactMatchingPrefixL(
        const MDesCArray& aFindStrings, 
        MVPbkSimViewFindObserver& aObserver )
    {
    return iSharedView.ContactMatchingPrefixL( aFindStrings, aObserver );
    }

// -----------------------------------------------------------------------------
// CViewHandle::SortOrderL
// -----------------------------------------------------------------------------
//                                        
const RVPbkSimFieldTypeArray& CViewHandle::SortOrderL() const
    {
    return iSharedView.SortOrderL();
    }
    
// -----------------------------------------------------------------------------
// CSharedViewOwner::CSharedViewOwner
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSharedViewOwner::CSharedViewOwner( CSharedViewArray& aSharedViewArray,
        CVPbkSimContactView* aSharedView )
        :   iSharedViewArray( aSharedViewArray ),
            iSharedView( aSharedView )   
    {
    }

// -----------------------------------------------------------------------------
// CSharedViewOwner::~CSharedViewOwner
// -----------------------------------------------------------------------------
//   
CSharedViewOwner::~CSharedViewOwner()
    {
    iViewHandles.Close();
    delete iSharedView;
    }

// -----------------------------------------------------------------------------
// CSharedViewOwner::View
// -----------------------------------------------------------------------------
//   
CVPbkSimContactView& CSharedViewOwner::View() const
    {
    return *iSharedView;
    }

// -----------------------------------------------------------------------------
// CSharedViewOwner::CreateNewHandleL
// -----------------------------------------------------------------------------
//           
CViewHandle* CSharedViewOwner::CreateNewHandleL()
    {
    CViewHandle* handle = 
            new( ELeave ) CViewHandle( iSharedViewArray, *iSharedView );
    CleanupStack::PushL( handle );
    iViewHandles.AppendL( handle );
    CleanupStack::Pop( handle );
    return handle;
    }

// -----------------------------------------------------------------------------
// CSharedViewOwner::RemoveHandle
// -----------------------------------------------------------------------------
//           
void CSharedViewOwner::RemoveHandle( CViewHandle& aHandle )
    {
    TInt index = iViewHandles.Find( &aHandle );
    if ( index != KErrNotFound )
        {
        iViewHandles.Remove( index );
        }
    }
    
// -----------------------------------------------------------------------------
// CSharedViewOwner::HasHandles
// -----------------------------------------------------------------------------
//   
TBool CSharedViewOwner::HasHandles() const
    {
    return iViewHandles.Count() > 0;
    }    
} // namespace VPbkSimStoreImpl
            
// End of File
