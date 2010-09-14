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
* Description:  Phonebook 2 contact view list box model.
*
*/


// INCLUDE FILES

#include "CPbk2ContactViewListBoxModel.h"

// Phonebook 2
#include "CPbk2IconArray.h"
#include "MPbk2ClipListBoxText.h"
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactNameFormatter2.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StoreViewDefinition.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2ContactUiControlExtension.h>
#include <MPbk2ContactUiControlExtension2.h>
#include <pbk2uicontrols.rsg>
#include <Pbk2PresentationUtils.h>
#include <CPbk2ContactIconsUtils.h>
#include "Pbk2NamesListExUID.h"
#include "cpbk2predictiveviewstack.h"

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkViewContact.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <MVPbkBaseContactField.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkContactFieldData.h>

// System includes
#include <eikenv.h>
#include <barsread.h>
#include <featmgr.h>

// Debugging headers
#include <Pbk2Debug.h>

//from PCS engine
#include <CPsRequestHandler.h>
#include <CPsQuery.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
const TText KSeparator = '\t';
const TText KSpace = ' ';
const TInt KNameColumn = 1;

const TInt KDefaultListFormatting =
        MPbk2ContactNameFormatter::EUseSeparator |
        MPbk2ContactNameFormatter::EPreserveLeadingSpaces;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_AppendIconIndex = 1,
    EPanicPreCond_EmptyIconArray
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2ContactViewListBoxModel");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

/**
 * Matches field type.
 *
 * @param aFieldTypeList    The field type list to use.
 * @param aField            The field whose type to match.
 * @return  Matched field type.
 */
const MVPbkFieldType* MatchFieldType
        ( const MVPbkFieldTypeList& aFieldTypeList,
        const MVPbkBaseContactField& aField )
    {
    const MVPbkFieldType* result = NULL;

    for (TInt matchPriority = 0;
        matchPriority <= aFieldTypeList.MaxMatchPriority() && !result;
        ++matchPriority)
        {
        result = aField.MatchFieldType(matchPriority);
        }

    return result;
    }

/**
 * Checks is the given field type included in
 * the given selection.
 *
 * @param aResourceId   Selector's resource id.
 * @param aField        The field whose type to check.
 * @param aManager      Contact manager.
 * @return  ETrue if field type is included.
 */
TBool IsFieldTypeIncludedL(
        const MVPbkBaseContactField& aField,
        const CVPbkContactManager& aManager,
        const TInt aResourceId )
    {
    // Get the field type
    const MVPbkFieldType* fieldType = MatchFieldType
        (aManager.FieldTypes(), aField);

    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, aResourceId);

    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL(resReader, aManager.FieldTypes());
    CleanupStack::PopAndDestroy(); // resReader

    TBool ret = selector->IsFieldTypeIncluded(*fieldType);
    delete selector;
    return ret;
    }

}  /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::CPbk2ContactViewListBoxModel
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBoxModel::CPbk2ContactViewListBoxModel
        ( CPbk2ContactViewListBoxModel::TParams& aParams ) :
            iContactManager( *aParams.iContactManager ),
            iView( aParams.iView ),
            iNameFormatter( *aParams.iNameFormatter ),
            iBuffer( iFormattingBuffer ),
            iIconArray( aParams.iIconArray ),
            iEmptyIconId( aParams.iEmptyId ),
            iDefaultIconId( aParams.iDefaultId ),
            iPreviousCount( 0 ),
            iClipListBoxText( aParams.iClipListBoxText ),
            iIconsStartAtZero( !aParams.iMultiselection ),
            iIconsOnlyOne( ETrue )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::~CPbk2ContactViewListBoxModel
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBoxModel::~CPbk2ContactViewListBoxModel()
    {
    delete iContactIcons;
    if(iFeatureManagerInitilized)
        {
        FeatureManager::UnInitializeLib();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBoxModel* CPbk2ContactViewListBoxModel::NewL
        ( CPbk2ContactViewListBoxModel::TParams& aParams )
    {
    CPbk2ContactViewListBoxModel* self =
        new ( ELeave ) CPbk2ContactViewListBoxModel( aParams );
    
	CleanupStack::PushL(self);
	self->ConstructL( aParams.iStoreProperties, aParams.iUiExtension );
	CleanupStack::Pop();    
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::ConstructL(
        CPbk2StorePropertyArray* aStoreProperties,
        MPbk2ContactUiControlExtension* aUiExtension)
    {
    if ( aUiExtension )
        {
        MPbk2ContactUiControlExtension2* ext = reinterpret_cast
            <MPbk2ContactUiControlExtension2*>( aUiExtension->
                ContactUiControlExtensionExtension(
                    KMPbk2ContactUiControlExtensionExtension2Uid ) );
        if ( ext != NULL )
            {
            CPbk2IconArray* extIconArray = const_cast<CPbk2IconArray*>(
                iIconArray );
            ext->SetIconArray( *extIconArray );
            }
        }
    iContactIcons = CPbk2ContactIconsUtils::NewL(
            *aStoreProperties,
            aUiExtension );
    // Initialize feature manager
    FeatureManager::InitializeLibL();
    iFeatureManagerInitilized = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::MdcaCount
// --------------------------------------------------------------------------
//
TInt CPbk2ContactViewListBoxModel::MdcaCount() const
    {
    TInt result = iPreviousCount;
    TRAPD(err, result = iView->ContactCountL());
    if (err != KErrNone)
        {
        HandleError(err);
        }
    iPreviousCount = result;
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC CPbk2ContactViewListBoxModel::MdcaPoint( TInt aIndex ) const
    {
    TRAPD(err, FormatBufferL(aIndex));
    switch (err)
        {
        case KErrNone:
            {
            // OK
            break;
            }

        case KErrNotFound:      // FALLTHROUGH
        case KErrAccessDenied:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2ContactViewListBoxModel::MdcaPoint(0x%x,%d), ignoring error %d"),
                this, aIndex, err);
            // Do not report these errors as they occur sometimes when
            // the view is under a massive update
            FormatEmptyBuffer();
            break;
            }

        default:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
                "CPbk2ContactViewListBoxModel::MdcaPoint(0x%x,%d), reporting error %d"),
                this, aIndex, err);
            FormatEmptyBuffer();
            // Report error
            HandleError(err);
            break;
            }
        }
    return iBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::SetUnnamedText
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::SetUnnamedText( const TDesC* aText )
    {
    iUnnamedText = aText;
    }

HBufC* CPbk2ContactViewListBoxModel::GetContactTitleLC( TInt aIndex ) const
	{
    const MVPbkViewContact& contact = iView->ContactAtL(aIndex);
    HBufC* contactName = iNameFormatter.GetContactTitleL( contact.Fields(), KDefaultListFormatting );
    CleanupStack::PushL( contactName );
    return contactName;
	}
// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::FormatBufferL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::FormatBufferL( const TInt aIndex ) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::FormatBufferL(0x%x,%d), begin"),
        this);
    const MVPbkViewContact& contact = iView->ContactAtL(aIndex);
    FormatBufferForContactL(contact, aIndex );
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::FormatBufferL(0x%x,%d), end"),
        this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::FormatEmptyBuffer
// Formats an empty listbox model text in case of an error.
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::FormatEmptyBuffer() const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::FormatEmptyBuffer(0x%x), begin"),
        this);

    TInt iconIndex = iIconArray->FindIcon(iEmptyIconId);
    if (iconIndex < 0)
        {
        iconIndex = iIconArray->FindIcon(iDefaultIconId);
        }
    iBuffer.Num(iconIndex);
    iBuffer.Append(KSeparator);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2ContactViewListBoxModel::FormatEmptyBuffer(0x%x) formatted: %S"),
        this, &iBuffer);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::FormatBufferForContactL
// Formats buffer to contain list box model text for a contact.
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::FormatBufferForContactL
        ( const MVPbkViewContact& aViewContact, TInt aIndex ) const
    {
    RArray<TPbk2IconId> ids;
    CleanupClosePushL( ids );
     // Format only name using aViewContact
     iBuffer.Zero();

     iContactIcons->GetIconIdsForContactL( aViewContact, ids );
     // ids shold contain at least one empty icon in any case
     if ( ids.Count() == 0 )
         {
         ids.Append( iEmptyIconId );
         }
     
     // Use temporary variables to avoid warnings in winscw build
     const TInt KFirstIndex ( 0 );
     // If there are icons from extensions use them
     if ( iIconsStartAtZero )
         {
         // In array 1st icon is preceding icon and rest of
         // the icons are added after the name as trailing icons
         AppendIconIndex( ids[ KFirstIndex ] );
         }
     else
         {
         //append default icon anyway, otherwise checkbox will not work
         AppendIconIndex( iDefaultIconId );
         }
     HBufC* name;
     if( FeatureManager::FeatureSupported(KFeatureIdFfContactsCompanyNames))
         {
         MPbk2ContactNameFormatter2* nameformatterExtension =
               reinterpret_cast<MPbk2ContactNameFormatter2*>(iNameFormatter.
                   ContactNameFormatterExtension( MPbk2ContactNameFormatterExtension2Uid ) );
         name =  nameformatterExtension->GetContactTitleWithCompanyNameL(aViewContact.Fields(),
                 KDefaultListFormatting);
         }
     else
         {
         // Append name
         name = iNameFormatter.GetContactTitleL(aViewContact.Fields(),
                KDefaultListFormatting);    
         }

     TPtr ptrName( name->Des() );
 
     // Replace characters that can not be displayed correctly
     Pbk2PresentationUtils::ReplaceNonGraphicCharacters
         ( ptrName, KSpace );
 
     if ( CutFromBeginningFieldL( aViewContact.Fields() ) &&
          iClipListBoxText )
         {
         iClipListBoxText->ClipFromBeginning(
             ptrName, aIndex, KNameColumn );
         }
     AppendName( ptrName );
     delete name;
     
     if ( !iIconsStartAtZero )
         {
         if ( iIconsOnlyOne )
             {
             AppendTrailingIcon( ids[KFirstIndex] );
             }
         else
             {
             AppendTrailingIcons( ids.Array(), KFirstIndex );
             }
         }
     else if ( !iIconsOnlyOne )
         {
         AppendTrailingIcons( ids.Array(), KFirstIndex + 1 );
         }
     
     CleanupStack::PopAndDestroy(); //ids
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::AppendName
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::AppendName( const TDesC& aName ) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::AppendName(0x%x), begin"),
        this);

    TPtrC text(aName);
    if (text.Length()==0 && iUnnamedText)
        {
        text.Set(*iUnnamedText);
        }
    iBuffer.Append(text);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::AppendName(0x%x), end"),
        this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::AppendIconIndex
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::AppendIconIndex(
        const TPbk2IconId& aIconId ) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::AppendIconIndex(0x%x), begin"),
        this);

    TInt iconIndex = iIconArray->FindIcon( aIconId );
    if (iconIndex < 0)
        {
        iconIndex = iIconArray->FindIcon( iDefaultIconId );
        }
    if (iconIndex < 0)
        {
        iconIndex = iIconArray->FindIcon( iEmptyIconId );
        }

    iBuffer.AppendNum( iconIndex );
    iBuffer.Append( KSeparator );

    __ASSERT_DEBUG(iconIndex > KErrNotFound,
        Panic(EPanicPostCond_AppendIconIndex));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::AppendIconIndex(0x%x), end"),
        this);
    }

void CPbk2ContactViewListBoxModel::AppendTrailingIcon(
        const TPbk2IconId& aIconId ) const
    {
    TInt iconIndex = iIconArray->FindIcon( aIconId );
    if ( iconIndex >= 0 )
        {
        iBuffer.Append( KSeparator );
        iBuffer.AppendNum( iconIndex );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::AppendTrailingIcons
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::AppendTrailingIcons
        ( const TArray<TPbk2IconId>& aIds, TInt aStartIndex ) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::AppendTrailingIcons(0x%x), begin"),
        this);

    const TInt count( aIds.Count() );
    // Start reading icon ids from index 1    
    // In array 1st icon is preceding icon and rest of icons are added after
    // the name as trailing icons.
    for ( TInt i = aStartIndex; i < count; ++i )
        {
        AppendTrailingIcon( aIds[i] );
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewListBoxModel::AppendTrailingIcons(0x%x), end"),
        this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::HandleError
// Handles any leave occuring in MdcaCount() or MdcaPoint().
// @see CEikonEnv::NotifyIdleErrorWhileRedrawing( aError)
// --------------------------------------------------------------------------
//
void CPbk2ContactViewListBoxModel::HandleError( TInt aError ) const
    {
    CEikonEnv::Static()->NotifyIdleErrorWhileRedrawing( aError );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::CutFromBeginningFieldL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactViewListBoxModel::CutFromBeginningFieldL
        ( const MVPbkBaseContactFieldCollection& aContactFields ) const
    {
    TBool ret( ETrue );
    TInt fieldCount( aContactFields.FieldCount() );
    for ( TInt i(0); i < fieldCount && ret; ++i)
        {
        const MVPbkBaseContactField& field = aContactFields.FieldAt( i );
        if ( !field.FieldData().IsEmpty() )
            {
            // Check are we dealing with a phone number field
            if ( !IsFieldTypeIncludedL( field, iContactManager,
                    R_PHONEBOOK2_PHONENUMBER_SELECTOR))
                {
                // If fields contains something else than number selection fields
                ret = EFalse;
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::LastPCSQuery
// ---------------------------------------------------------------------------
const CPsQuery* CPbk2ContactViewListBoxModel::LastPCSQuery() const
    {
    
    const CPbk2PredictiveViewStack* view = static_cast
                <const CPbk2PredictiveViewStack*>( iView );
           
    if ( view )
        {
        return view->LastPCSQuery();
        }
    
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::GetMatchingPartsL
// ---------------------------------------------------------------------------
void CPbk2ContactViewListBoxModel::GetMatchingPartsL( const TDesC& aSearchData,
                      RArray<TPsMatchLocation>& aMatchLocation)
    {
    const CPbk2PredictiveViewStack* view = static_cast
                    <const CPbk2PredictiveViewStack*>( iView );
               
    if ( view )
        {
        view->GetMatchingPartsL(aSearchData, aMatchLocation);
        }    
    }

// ---------------------------------------------------------------------------
// CPbk2ContactViewListBoxModel::PSHandler
// ---------------------------------------------------------------------------
CPSRequestHandler* CPbk2ContactViewListBoxModel::PSHandler() const
    {    
    const CPbk2PredictiveViewStack* view = static_cast
                    <const CPbk2PredictiveViewStack*>( iView );
           
    if ( view )
        {
        return view->PSHandler();
        }
    
    return NULL;
    }

//  End of File
