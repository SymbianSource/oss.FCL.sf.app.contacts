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
* Description:  Phonebook 2 contact view sort policy.
*
*/


#include "CPbk2ContactViewSortPolicy.h"

// Phonebook2
#include "CPbk2SortKeyArray.h"
#include "RPbk2LocalizedResourceFile.h"
#include "Pbk2DataCaging.hrh"
#include <pbk2uipolicyres.rsg>

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <MVPbkViewContact.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldFilter.h>
#include <CVPbkSortOrder.h>

// System includes
#include <coemain.h>
#include <barsread.h>
#include <SortUtil.h>

// Debugging headers
#include <Pbk2Profile.h>

#include <vpbkeng.rsg>

// Greater than any practical top contact number string
// which is in form like '0000000001'.
_LIT(KBigNumStr, "9999999999");


/// Unnamed namespace for local definitions
namespace {

_LIT(KResourceFile, "Pbk2UIPolicyRes.rsc");
const TInt KNegative = -1;
const TInt KPositive = 1;

/**
 * Sort key array.
 */
class TPbk2SortKeyArray : public MSortKeyArray
    {
    public: // Interface

        /**
         * Constructor.
         *
         * @param aArray    Array.
         */
        TPbk2SortKeyArray(
                const MVPbkSortKeyArray& aArray );

        /**
         * Returns the count of the keys.
         *
         * @return  Count of the keys.
         */
        TInt SortKeyCount() const
            {
            return iArray.Count();
            }

        /**
         * Returns the key at aIndex.
         *
         * @param aIndex    Index to query.
         * @return  Key at queried index.
         */
        TSortKey SortKeyAt(
                TInt aIndex ) const;

    private: // Data
        /// Ref: Array
        const MVPbkSortKeyArray& iArray;

    };

// --------------------------------------------------------------------------
// TPbk2SortKeyArray::TPbk2SortKeyArray
// --------------------------------------------------------------------------
//
TPbk2SortKeyArray::TPbk2SortKeyArray( const MVPbkSortKeyArray& aArray ) :
        iArray( aArray )
    {
    }

// --------------------------------------------------------------------------
// TPbk2SortKeyArray::SortKeyAt
// --------------------------------------------------------------------------
//
TSortKey TPbk2SortKeyArray::SortKeyAt( TInt aIndex ) const
    {
    TSortKeyType sortKeyType( ESortKeyBasic );
    if ( iArray.SortKeyAt( aIndex ).Type() == EVPbkSortKeyPronounciation )
        {
        sortKeyType = ESortKeyPronounciation;
        }

    return TSortKey( iArray.SortKeyAt( aIndex ).Text(), sortKeyType );
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::CPbk2ContactViewSortPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactViewSortPolicy::CPbk2ContactViewSortPolicy()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::~CPbk2ContactViewSortPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactViewSortPolicy::~CPbk2ContactViewSortPolicy()
    {
    delete iPronunciationFieldSelector;
    delete iRightSortKeyArray;
    delete iLeftSortKeyArray;
    delete iSortUtil;
    delete iSortOrder;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactViewSortPolicy* CPbk2ContactViewSortPolicy::NewL
        ( TParam* aParam )
    {
    CPbk2ContactViewSortPolicy* self =
        new ( ELeave ) CPbk2ContactViewSortPolicy;
    CleanupStack::PushL( self );
    self->ConstructL( aParam );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSortPolicy::ConstructL( TParam* aParam )
    {
    iSortUtil = CSortUtil::NewL();

    if ( aParam )
        {
        iSortPolicyParameterReceived = ETrue;

        iLeftSortKeyArray = CPbk2SortKeyArray::NewL();
        iRightSortKeyArray = CPbk2SortKeyArray::NewL();

        iSortOrder = CVPbkSortOrder::NewL(aParam->iSortOrder);
        iFieldMapper.SetSortOrder(*iSortOrder);

        iTopContactId = GetTopContactFieldIndex();

        RPbk2LocalizedResourceFile resFile(aParam->iFs);
        resFile.OpenLC( KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR,
            KResourceFile );

        HBufC8* selectorBuf = resFile.AllocReadLC
            ( R_PRONUNCIATION_FIELD_SELECTOR );
        TResourceReader reader;
        reader.SetBuffer( selectorBuf );
        iPronunciationFieldSelector = CVPbkFieldTypeSelector::NewL
            ( reader, aParam->iFieldTypes );

        CleanupStack::PopAndDestroy( 2 ); // buffer, resFile

        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::SetSortOrderL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSortPolicy::SetSortOrderL
        ( const MVPbkFieldTypeList& aSortOrder )
    {
    if ( !iSortPolicyParameterReceived )
        {
        // This class has been initialized without TParam
        User::Leave( KErrNotSupported );
        }

    CVPbkSortOrder* sortOrder = CVPbkSortOrder::NewL( aSortOrder );
    delete iSortOrder;
    iSortOrder = sortOrder;
    iFieldMapper.SetSortOrder( *iSortOrder );

    iTopContactId = GetTopContactFieldIndex();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::SortStartL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSortPolicy::SortStartL()
    {
    if ( !iSortPolicyParameterReceived )
        {
        // This class has been initialized without TParam
        User::Leave( KErrNotSupported );
        }

    iLeftSortKeyArray->Reset();
    iRightSortKeyArray->Reset();

    const TInt typeCount( iSortOrder->FieldTypeCount() );
    for ( TInt i( 0 ); i < typeCount; ++i )
        {
        const MVPbkFieldType& fieldType = iSortOrder->FieldTypeAt( i );
        if ( iPronunciationFieldSelector->IsFieldTypeIncluded( fieldType ) )
            {
            iLeftSortKeyArray->AppendL(
                TSortKey( KNullDesC, ESortKeyPronounciation ) );
            iRightSortKeyArray->AppendL(
                TSortKey( KNullDesC, ESortKeyPronounciation ) );
            }
        else
            {
            iLeftSortKeyArray->AppendL(
                TSortKey( KNullDesC, ESortKeyBasic ) );
            iRightSortKeyArray->AppendL(
                TSortKey( KNullDesC, ESortKeyBasic ) );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::SortCompleted
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSortPolicy::SortCompleted()
    {
    iLeftSortKeyArray->Reset();
    iRightSortKeyArray->Reset();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::CompareContacts
// --------------------------------------------------------------------------
//
TInt CPbk2ContactViewSortPolicy::CompareContacts
        ( const MVPbkViewContact& aLhs, const MVPbkViewContact& aRhs )
    {
    TInt ret = 0;

    if ( iSortPolicyParameterReceived )
        {
        // Change the text in the sortkeys to correspond
        // fields in the contacts

        TInt i = 0;

PBK2_PROFILE_START( Pbk2Profile::ESortPolicyCompareContacts );

        iFieldMapper.SetContactFields( aLhs.Fields() );
        const TInt lhsCount = iFieldMapper.FieldCount();
        for ( i = 0; i < lhsCount; ++i )
            {
            SetSortKey( iLeftSortKeyArray, iFieldMapper.FieldAt(i), i );
            }

        iFieldMapper.SetContactFields( aRhs.Fields() );
        const TInt rhsCount = iFieldMapper.FieldCount();
        for ( i = 0; i < rhsCount; ++i )
            {
            SetSortKey( iRightSortKeyArray, iFieldMapper.FieldAt(i), i );
            }

PBK2_PROFILE_END( Pbk2Profile::ESortPolicyCompareContacts );


PBK2_PROFILE_START( Pbk2Profile::ESortPolicySortUtil );

        TInt result( iSortUtil->Interface()->CompareItems
            ( *iLeftSortKeyArray, *iRightSortKeyArray ) );

PBK2_PROFILE_END( Pbk2Profile::ESortPolicySortUtil );

        ret = PostProcessResult( result );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::CompareItems
// --------------------------------------------------------------------------
//
TInt CPbk2ContactViewSortPolicy::CompareItems
        ( const MVPbkSortKeyArray& aLhs,
          const MVPbkSortKeyArray& aRhs ) const
    {
    const TPbk2SortKeyArray lhsKeyArray( aLhs );
    const TPbk2SortKeyArray rhsKeyArray( aRhs );
    return iSortUtil->Interface()->CompareItems( lhsKeyArray, rhsKeyArray );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::PostProcessResult
// --------------------------------------------------------------------------
//
TInt CPbk2ContactViewSortPolicy::PostProcessResult( TInt aSortUtilResult )
    {
    // With this function we correct sort util result because it does not
    // give right answers if contact is "Unnamed".
    // So here is clarified how this function corrects the result.
    // If rightside has no data left is less (-1) otherwise greater (1).
    // "<Unnamed>"  vs. "<Unnamed>" = -1,   leftside comes before rightside
    // "a"          vs. "<Unnamed>" = -1,   leftside comes before rightside
    // "<Unnamed>"  vs. "a"         = 1     rightside comes before leftside
    //
    TInt result = aSortUtilResult;
    const TInt rhsCount( iRightSortKeyArray->SortKeyCount() );
    TBool rhsHasData( EFalse );
    for ( TInt i = 0; i < rhsCount; ++i )
        {
        TSortKey key = iRightSortKeyArray->SortKeyAt( i );
        if ( key.Text().Length() > 0 )
            {
            rhsHasData = ETrue;
            break;
            }
        }

    if ( !rhsHasData )
        {
        // Here we have situation "something" vs "<Unnamed>" .
        // We want leftside to come before rightside.
        result = KNegative;
        }
    else
        {
        TBool lhsHasData( EFalse );
        const TInt lhsCount( iLeftSortKeyArray->SortKeyCount() );
        for ( TInt i = 0; i < lhsCount; ++i )
            {
            TSortKey key = iLeftSortKeyArray->SortKeyAt( i );
            if ( key.Text().Length() > 0 )
                {
                lhsHasData = ETrue;
                break;
                }
            }

        if ( !lhsHasData )
            {
            // Here we have situation "<Unnamed>" vs. "something".
            // We want rightside to come before leftside.
            result = KPositive;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::GetTopContactFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactViewSortPolicy::GetTopContactFieldIndex()
    {
    TInt index = KErrNotFound;
    for ( TInt i=0; i<iSortOrder->FieldTypeCount(); i++ )
        {
        // Resource id of top contact field is R_VPBK_FIELD_TYPE_TOPCONTACT.
        if ( iSortOrder->FieldTypeAt( i ).FieldTypeResId() == 
                R_VPBK_FIELD_TYPE_TOPCONTACT )
            {
            index = i;
            break;
            }
        }
    return index;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSortPolicy::SetSortKey
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSortPolicy::SetSortKey(
        CPbk2SortKeyArray* aSortKeyArray,
        const MVPbkBaseContactField* aField,
        TInt aIndex )
    {
    TInt textLen = 0;
    if ( aField )
        {
        TPtrC text = MVPbkContactFieldTextData::Cast
            ( aField->FieldData() ).Text();
        aSortKeyArray->SetText( text, aIndex );
        textLen = text.Length();
        }
    if ( textLen == 0 )
        {
        if ( aIndex == iTopContactId )
            {
            // For some variants such as Chinese variant, 
            // empty string is less than not empty string. 
            // This will make topic contacts which have substantial top contact
            // id strings after those with empty id strings.
            // To prevent this, use a big number string instead of empty string 
            // and make sure top contacts in front of other contacts.
            aSortKeyArray->SetText( KBigNumStr, aIndex );
            }
        else
            {
            aSortKeyArray->SetText( KNullDesC, aIndex );
            }
        }
    }

//  End of File
