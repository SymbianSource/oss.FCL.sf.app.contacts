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
* Description:  A sim contact sorting utility
*
*/



// INCLUDE FILES
#include "CSimCntSortUtil.h"

#include "RVPbkStreamedIntArray.h"
#include "CVPbkSimContact.h"
#include "CVPbkSimCntField.h"
#include "VPbkSimStoreImplError.h"
#include <CVPbkContactViewSortPolicy.h>

namespace VPbkSimStoreImpl {

// CONSTANTS
const TInt KEqual = 0;
const TInt KLeftAfterRight = 1;
const TInt KLeftBeforeRight = -1;

// ============================= LOCAL FUNCTIONS ===============================

namespace {

// -----------------------------------------------------------------------------
// GetKeyType
// Get sort key field type. Japanese reading use ESortKeyPronounciation.
// -----------------------------------------------------------------------------
//
inline TVPbkSortKeyType GetKeyType( TVPbkSimCntFieldType aFieldType )
    {
    if ( aFieldType == EVPbkSimReading )
        {
        return EVPbkSortKeyPronounciation;
        }
    return EVPbkSortKeyBasic;
    }
    
/**
 * Fallback for sort policy if there is no
 * sort policy plugin installed to the system.
 */
NONSHARABLE_CLASS(TSortPolicy) : public MVPbkContactViewSortPolicy
    {
    public:
        /**
         * Constructor.
         */
        TSortPolicy() {}

    private: // From MVPbkContactViewSortPolicy
        void SetSortOrderL(
                const MVPbkFieldTypeList& /* aSortOrder */) {}
        void SortStartL() {}
        void SortCompleted() {}
        TInt CompareContacts(
                const MVPbkViewContact& /* aLhs */,
                const MVPbkViewContact& /* aRhs */) { return 0; }
        TInt CompareItems(const MVPbkSortKeyArray& aLhs, 
                          const MVPbkSortKeyArray& aRhs) const;
    };

TInt TSortPolicy::CompareItems(const MVPbkSortKeyArray& aLhs, 
                          const MVPbkSortKeyArray& aRhs) const
    {
    const TInt lhsCount = aLhs.Count();
    const TInt rhsCount = aRhs.Count();
    for ( TInt i( 0 ); i < lhsCount && i < rhsCount; ++i )
        {
        const TVPbkSortKey lhsKey( aLhs.SortKeyAt( i ) );
        const TVPbkSortKey rhsKey( aRhs.SortKeyAt( i ) );
        const TInt result = lhsKey.Text().CompareC( rhsKey.Text() );
        if ( result != 0 )
            {
            return result;
            }
        }
    return lhsCount - rhsCount;
    }
        
} // unnamed namespace

// ============================ MEMBER FUNCTIONS ===============================

/**
* An implementation for sort key array that is needed by sortutil component
*/
NONSHARABLE_CLASS(CSimCntSortUtil::CSortKeyArray) : public CBase,
                                                    public MVPbkSortKeyArray
    {
    public: // Construction and destruction

        /**
        * Two-phased constructor.
        * @param aMaxKeyCount the maximun amount of keys
        * @return a new instance of this class
        */
        static CSortKeyArray* NewL( TInt aMaxKeyCount );

        // Destructor
        ~CSortKeyArray();

    public: // New functions

        /**
        * Resets the sort key array
        */
        void Reset();

        /**
        * Appends a new sort key to the array
        * @param aSortKey the new sort key
        */
        void Append( TVPbkSortKey aSortKey );

    public:    // Functions from base classes

        /**
        * From MVPbkSortKeyArray
        */ 
        TInt Count() const;

        /**
        * From MVPbkSortKeyArray
        */
        TVPbkSortKey SortKeyAt( TInt aIndex ) const;

    private:    // Construction
        
        /**
        * C++ constructor
        */
        CSortKeyArray( TInt aMaxKeyCount );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// An array for keys
        RArray<TVPbkSortKey> iKeyArray;
    };

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSortKeyArray::CSortKeyArray
// -----------------------------------------------------------------------------
//
CSimCntSortUtil::CSortKeyArray::CSortKeyArray( TInt aMaxKeyCount )
:   iKeyArray( aMaxKeyCount )
    {
    }

// Destructor
CSimCntSortUtil::CSortKeyArray::~CSortKeyArray()
    {
    iKeyArray.Close();
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSortKeyArray::NewL
// -----------------------------------------------------------------------------
//
CSimCntSortUtil::CSortKeyArray* CSimCntSortUtil::CSortKeyArray::NewL( 
    TInt aMaxKeyCount )
    {
    CSortKeyArray* self = new( ELeave ) CSortKeyArray( aMaxKeyCount );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSortKeyArray::ConstructL
// -----------------------------------------------------------------------------
//
void CSimCntSortUtil::CSortKeyArray::ConstructL()
    {
    // Reserver enough memory here so that memory
    // doesn't run out in Append.

    // insert a dummy item to make array to allocate memory
    iKeyArray.AppendL( TVPbkSortKey( KNullDesC, EVPbkSortKeyBasic ) );
    // remove the dummy item
    iKeyArray.Remove( 0 );
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSortKeyArray::SortKeyCount
// -----------------------------------------------------------------------------
//
TInt CSimCntSortUtil::CSortKeyArray::Count() const
    {
    return iKeyArray.Count();
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSortKeyArray::SortKeyAt
// -----------------------------------------------------------------------------
//
TVPbkSortKey CSimCntSortUtil::CSortKeyArray::SortKeyAt( TInt aIndex ) const
    {
    return iKeyArray[aIndex];
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSortKeyArray::Reset
// -----------------------------------------------------------------------------
//
void CSimCntSortUtil::CSortKeyArray::Reset()
    {
    // Use Remove instead of Reset to keep the array memory allocated
    TInt count = iKeyArray.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        iKeyArray.Remove( i );
        }
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSortKeyArray::Append
// -----------------------------------------------------------------------------
//
void CSimCntSortUtil::CSortKeyArray::Append( TVPbkSortKey aSortKey )
    {
#ifdef _DEBUG
    TInt result = iKeyArray.Append( aSortKey );
    __ASSERT_DEBUG( result == KErrNone,
        Panic( EMemoryRunOutInSortKeyArrayAppend ) );
#else
    // Must not fail because memory was reserved in ConstructL
    iKeyArray.Append( aSortKey );
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::CSimCntSortUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSimCntSortUtil::CSimCntSortUtil( const RVPbkSimFieldTypeArray& aSortOrder )
:   iSortOrder( aSortOrder )
    {
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimCntSortUtil::ConstructL()
    {
    TInt count = iSortOrder.Count();
    iLeftKeyArray = CSortKeyArray::NewL( count );
    iRightKeyArray = CSortKeyArray::NewL( count );
    TRAPD(err, iSortPolicy = CVPbkContactViewSortPolicy::NewL());
    if (err == KErrNotFound)
        {
        // Use fallback sort policy
        iSortPolicy = new (ELeave) TSortPolicy;
        }
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimCntSortUtil* CSimCntSortUtil::NewL( 
    const RVPbkSimFieldTypeArray& aSortOrder )
    {
    CSimCntSortUtil* self = new( ELeave ) CSimCntSortUtil( aSortOrder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CSimCntSortUtil::~CSimCntSortUtil()
    {
    delete iLeftKeyArray;
    delete iRightKeyArray;
    delete iSortPolicy;
    // iSortPolicy is an ECOM plugin so call to FinalClose after deleting it
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::Compare
// -----------------------------------------------------------------------------
//
TInt CSimCntSortUtil::Compare( const MVPbkSimContact& aLeft, 
    const MVPbkSimContact& aRight )
    {
    iLeftKeyArray->Reset();
    iRightKeyArray->Reset();

    TInt typeCount = iSortOrder.Count();
    for ( TInt i = 0; i < typeCount; ++i )
        {
        MVPbkSimContact::TFieldLookup lookup = aLeft.FindField( iSortOrder[i] );
        if ( !lookup.EndOfLookup() )
            {
            InsertKey( *iLeftKeyArray, aLeft.ConstFieldAt( lookup.Index() ) );
            }
        lookup = aRight.FindField( iSortOrder[i] );
        if ( !lookup.EndOfLookup() )
            {
            InsertKey( *iRightKeyArray, aRight.ConstFieldAt( lookup.Index() ) );
            }
        }
    
    // Return zero, if the two keys are equal
    // negative, if the left key is less than the right key
    // positive, if the left key is greater than the right key.

    if ( iLeftKeyArray->Count() == 0 && 
         iRightKeyArray->Count() == 0 )
        {
        return KEqual;
        }

    if ( iLeftKeyArray->Count() == 0 )
        {
        return KLeftAfterRight;
        }

    if ( iRightKeyArray->Count() == 0 )
        {
        return KLeftBeforeRight;
        }
    
    return iSortPolicy->CompareItems( *iLeftKeyArray, *iRightKeyArray );
    }

// -----------------------------------------------------------------------------
// CSimCntSortUtil::Compare
// -----------------------------------------------------------------------------
//
void CSimCntSortUtil::InsertKey( CSortKeyArray& aKeyArray,
    const CVPbkSimCntField& aField )
    {
    aKeyArray.Append( TVPbkSortKey( aField.Data(), 
        GetKeyType( aField.Type() ) ) );
    }
} // namespace VPbkSimStoreImpl
//  End of File  
