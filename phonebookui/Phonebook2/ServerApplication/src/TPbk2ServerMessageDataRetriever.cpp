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
* Description:  Phonebook 2 server application server message
*              : data retriever.
*
*/


#include "TPbk2ServerMessageDataRetriever.h"

// Phonebook 2
#include <Pbk2InternalUID.h>
#include <Pbk2IPCPackage.h>
#include <Pbk2MimeTypeHandler.h>
#include <CPbk2StoreConfiguration.h>
#include <Pbk2UIControls.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <s32mem.h>
#include <barsread.h>
#include <AiwContactSelectionDataTypes.h>
#include <AiwContactAssignDataTypes.h>

using namespace AiwContactAssign;

/// Unnamed namespace for local definitions
namespace {

// RMessage IPC-argument slot order and positions
const TInt KConfigurationPackageSlot( 0 );
const TInt KDataPackageSlot( 1 );
const TInt KAttributeDataSlot( 1 );
const TInt KAssignInstructionsSlot( 2 );
const TInt KFetchInstructionsSlot( 1 );

// Data package positions
const TInt KMimeTypePosition( 0 );
const TInt KDataBufferPosition( 1 );
const TInt KIndexBufferPosition( 2 );

// Configuration package positions
const TInt KStoreUrisPosition( 0 );
const TInt KPreselectedContactsPosition( 1 );
const TInt KAddressSelectFilterBuffer( 2 );
const TInt KContactViewFilterBuffer( 3 );
const TInt KTitlePaneTextPosition( 4 );
const TInt KStatusPaneIdPosition( 5 );
const TInt KDefaultPrioritiesPosition( 2 );
const TInt KOrientationType( 5 );


_LIT(KPanicText, "TPbk2ServerMessageDataRetriever");

enum TPanicCode
    {
    EInvalidFetchType = 1
    };

/**
 * Goes through link array and adds missing store URIs to URI array.
 *
 * @param aStoreUriArray    Array of store URIs.
 * @param aContactLinks     Contact link array.
 */
void AddStoreUrisFromContactLinksL
        ( CVPbkContactStoreUriArray& aStoreUriArray,
          MVPbkContactLinkArray& aContactLinks )
    {
    TInt count = aContactLinks.Count();

    for ( TInt i = 0; i < count; ++i )
        {
        const MVPbkContactLink& contactLink = aContactLinks.At(i);
        const TVPbkContactStoreUriPtr& storeUri =
            contactLink.ContactStore().StoreProperties().Uri();
        if ( !aStoreUriArray.IsIncluded( storeUri ) )
            {
            aStoreUriArray.AppendL( storeUri );
            }
        }
    }


} /// namespace


// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::TPbk2ServerMessageDataRetriever
// --------------------------------------------------------------------------
//
TPbk2ServerMessageDataRetriever::TPbk2ServerMessageDataRetriever()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::GetContactStoreUriArrayL
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray*
    TPbk2ServerMessageDataRetriever::GetContactStoreUriArrayL
        ( const RMessage2& aMessage,
          const CPbk2StoreConfiguration& aStoreConfiguration,
          MVPbkContactLinkArray* aPreselectedLinks,
          TBool& aCurrentConfiguration ) const
    {
    CVPbkContactStoreUriArray* storeUriArray = NULL;
    aCurrentConfiguration = EFalse;

    if ( aPreselectedLinks && aPreselectedLinks->Count() > 0 )
        {
        // Stores in contact links are enough
        storeUriArray  = CVPbkContactStoreUriArray::NewL();
        CleanupStack::PushL( storeUriArray );
        AddStoreUrisFromContactLinksL( *storeUriArray, *aPreselectedLinks );
        CleanupStack::Pop( storeUriArray );
        }

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( !storeUriArray && length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr, KStoreUrisPosition );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        // Store URIs
        HBufC8* storeUriBuffer = NULL;
        Pbk2IPCPackage::InternalizeL( storeUriBuffer, readStream );
        if ( storeUriBuffer )
            {
            CleanupStack::PushL( storeUriBuffer );
            TPtr8 storeUriPtr = storeUriBuffer->Des();

            storeUriArray = CVPbkContactStoreUriArray::NewLC( storeUriPtr );

            CleanupStack::Pop(); // storeUriArray
            CleanupStack::PopAndDestroy( storeUriBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }

    if ( !storeUriArray )
        {
        storeUriArray = aStoreConfiguration.CurrentConfigurationL();
        aCurrentConfiguration = ETrue;
        }

    return storeUriArray;
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::GetPreselectedContactLinksL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
    TPbk2ServerMessageDataRetriever::GetPreselectedContactLinksL
        ( const RMessage2& aMessage,
          const CVPbkContactManager& aContactManager ) const
    {
    MVPbkContactLinkArray* preselectedContacts = NULL;

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();
        HBufC8* packedLinksBuffer = NULL;
        Pbk2IPCPackage::InternalizeL
            ( packedLinksBuffer, readStream, KPreselectedContactsPosition );
        if ( packedLinksBuffer )
            {
            CleanupStack::PushL( packedLinksBuffer );
            TPtr8 packedLinksPtr = packedLinksBuffer->Des();
            preselectedContacts = aContactManager.CreateLinksLC( packedLinksPtr );
            CleanupStack::Pop(); // preselectedContacts
            CleanupStack::PopAndDestroy( packedLinksBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }

    if ( !preselectedContacts )
        {
        preselectedContacts = CVPbkContactLinkArray::NewL();
        }

    return preselectedContacts;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetAddressSelectFilterL
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector*
    TPbk2ServerMessageDataRetriever::GetAddressSelectFilterL
        ( const RMessage2& aMessage,
          const MVPbkFieldTypeList& aFieldTypeList ) const
    {
    CVPbkFieldTypeSelector* selector = NULL;

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        HBufC8* addressSelectFilterBuffer = NULL;
        Pbk2IPCPackage::InternalizeL
            ( addressSelectFilterBuffer, readStream,
              KAddressSelectFilterBuffer );

        if ( addressSelectFilterBuffer )
            {
            CleanupStack::PushL( addressSelectFilterBuffer );

            TResourceReader reader;
            reader.SetBuffer( addressSelectFilterBuffer );
            selector = CVPbkFieldTypeSelector::NewL
                ( reader, aFieldTypeList );

            CleanupStack::PopAndDestroy( addressSelectFilterBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }

    return selector;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetAddressSelectFilterBufferL
// --------------------------------------------------------------------------
//
HBufC8* TPbk2ServerMessageDataRetriever::GetAddressSelectFilterBufferL
        ( const RMessage2& aMessage ) const
    {
    HBufC8* addressSelectFilterBuffer = NULL;

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        Pbk2IPCPackage::InternalizeL
            ( addressSelectFilterBuffer, readStream,
              KAddressSelectFilterBuffer );

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }

    return addressSelectFilterBuffer;
    }


// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetAddressSelectType
// --------------------------------------------------------------------------
//
TAiwAddressSelectType TPbk2ServerMessageDataRetriever::GetAddressSelectTypeL
        ( const RMessage2& aMessage ) const
    {
    TAiwAddressSelectType ret = EAiwAllItemsSelect;

    TInt length = aMessage.GetDesLengthL( KFetchInstructionsSlot );
    if ( length > 0 )
        {
        HBufC8* dataBuf = HBufC8::NewLC( length );
        TPtr8 ptr = dataBuf->Des();
        aMessage.ReadL( KFetchInstructionsSlot, ptr );

        TAiwContactSelectionDataType dataType =
            TAiwContactSelectionDataBase::SelectionDataTypeFromBuffer( ptr );

        switch ( dataType )
            {
            case EAiwSingleItemSelectionV1:
                {
                TAiwSingleItemSelectionDataV1Pckg data;
                data.Copy( ptr );
                ret = data().AddressSelectType();
                break;
                }

            case EAiwSingleItemSelectionV2:
                {
                // Deprecated
                break;
                }

            case EAiwSingleItemSelectionV3:
                {
                TAiwSingleItemSelectionDataV3Pckg data;
                data.Copy( ptr );
                ret = data().AddressSelectType();
                break;
                }

            case EAiwMultipleItemSelectionV1:
                {
                TAiwMultipleItemSelectionDataV1Pckg data;
                data.Copy( ptr );
                ret = data().AddressSelectType();
                break;
                }

            default:
                {
                break;
                }
            }

        CleanupStack::PopAndDestroy( dataBuf );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetCommAddressSelectType
// --------------------------------------------------------------------------
//
TAiwCommAddressSelectType TPbk2ServerMessageDataRetriever::
    GetCommAddressSelectTypeL( const RMessage2& aMessage ) const
    {
    TAiwCommAddressSelectType ret = EAiwCommEmpty;

    TInt length = aMessage.GetDesLengthL( KFetchInstructionsSlot );
    if ( length > 0 )
        {
        HBufC8* dataBuf = HBufC8::NewLC( length );
        TPtr8 ptr = dataBuf->Des();
        aMessage.ReadL( KFetchInstructionsSlot, ptr );

        TAiwContactSelectionDataType dataType =
            TAiwContactSelectionDataBase::SelectionDataTypeFromBuffer( ptr );

        switch ( dataType )
            {
            case EAiwSingleItemSelectionV1:
                {
                TAiwSingleItemSelectionDataV1Pckg data;
                data.Copy( ptr );
                ret = data().CommAddressSelectType();
                break;
                }

            case EAiwSingleItemSelectionV2:
                {
                // Deprecated
                break;
                }

            case EAiwSingleItemSelectionV3:
                {
                TAiwSingleItemSelectionDataV3Pckg data;
                data.Copy( ptr );
                ret = data().CommAddressSelectType();
                break;
                }

            default:
                {
                break;
                }
            }

        CleanupStack::PopAndDestroy( dataBuf );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetContactViewFilterL
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector*
    TPbk2ServerMessageDataRetriever::GetContactViewFilterL
        ( const RMessage2& aMessage,
          const MVPbkFieldTypeList& aFieldTypeList ) const
    {
    CVPbkFieldTypeSelector* selector = NULL;

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        HBufC8* viewFilterBuffer = NULL;
        Pbk2IPCPackage::InternalizeL
            ( viewFilterBuffer, readStream, KContactViewFilterBuffer );
        if ( viewFilterBuffer )
            {
            CleanupStack::PushL( viewFilterBuffer );
            TPtr8 viewFilterPtr = viewFilterBuffer->Des();
            selector = CVPbkFieldTypeSelector::NewL( aFieldTypeList );
            CleanupStack::PushL( selector );
            selector->InternalizeL( viewFilterPtr );
            CleanupStack::Pop(); // selector
            CleanupStack::PopAndDestroy( viewFilterBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }

    return selector;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetContactViewFilterForAttributeAssignL
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector*
    TPbk2ServerMessageDataRetriever::GetContactViewFilterForAttributeAssignL
        ( const RMessage2& aMessage,
          const MVPbkFieldTypeList& aFieldTypeList ) const
    {
    CVPbkFieldTypeSelector* selector = NULL;

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        HBufC8* filterSelectorBuffer = NULL;
        Pbk2IPCPackage::InternalizeL
            ( filterSelectorBuffer, readStream, KContactViewFilterBuffer );
        if ( filterSelectorBuffer )
            {
            CleanupStack::PushL( filterSelectorBuffer );

            TResourceReader reader;
            reader.SetBuffer( filterSelectorBuffer );
            selector = CVPbkFieldTypeSelector::NewL
                ( reader, aFieldTypeList );

            CleanupStack::PopAndDestroy( filterSelectorBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }

    return selector;
    }

// --------------------------------------------------------------------------
// CPbk2ContactAttributeAssigner::GetTitlePaneTextL
// --------------------------------------------------------------------------
//
HBufC* TPbk2ServerMessageDataRetriever::GetTitlePaneTextL
        ( const RMessage2& aMessage ) const
    {
    HBufC* titlePaneText = NULL;

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        Pbk2IPCPackage::InternalizeL
            ( titlePaneText, readStream, KTitlePaneTextPosition );

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        
        }

    return titlePaneText;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetAttributeDataL
// --------------------------------------------------------------------------
//
TPbk2AttributeAssignData TPbk2ServerMessageDataRetriever::GetAttributeDataL
        ( const RMessage2& aMessage ) const
    {
    TPbk2AttributeAssignData attributeData;
    attributeData.iAttributeUid = TUid::Uid( KEPOCNullUID );
    attributeData.iAttributeValue = KErrNotFound;

    TPckg<TPbk2AttributeAssignData> attributePckg( attributeData );
    aMessage.ReadL( KAttributeDataSlot, attributePckg );

    return attributeData;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetDataBufferL
// --------------------------------------------------------------------------
//
HBufC* TPbk2ServerMessageDataRetriever::GetDataBufferL
        ( const RMessage2& aMessage ) const
    {
    HBufC* dataBuffer = NULL;

    TInt length = aMessage.GetDesLengthL( KDataPackageSlot );
    if ( length > 0 )
        {
        HBufC8* dataPackage = HBufC8::NewLC( length );
        TPtr8 ptr = dataPackage->Des();
        aMessage.ReadL( KDataPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        Pbk2IPCPackage::InternalizeL
            ( dataBuffer, readStream, KDataBufferPosition );

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( dataPackage );
        }

    return dataBuffer;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetFocusIndexL
// --------------------------------------------------------------------------
//
TInt TPbk2ServerMessageDataRetriever::GetFocusIndexL
        ( const RMessage2& aMessage ) const
    {
    TInt index = KErrNotFound;

    TInt length = aMessage.GetDesLengthL( KDataPackageSlot );
    if ( length > 0 )
        {
        HBufC8* dataPackage = HBufC8::NewLC( length );
        TPtr8 ptr = dataPackage->Des();
        aMessage.ReadL( KDataPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        HBufC* indexBuffer = NULL;
        Pbk2IPCPackage::InternalizeL
            ( indexBuffer, readStream, KIndexBufferPosition );
        if ( indexBuffer )
            {
            CleanupStack::PushL( indexBuffer );
            TLex16 indexer(*indexBuffer);
            TInt err = indexer.Val(index);
            CleanupStack::PopAndDestroy( indexBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( dataPackage );
        }

    return index;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetMimeTypeL
// --------------------------------------------------------------------------
//
TInt TPbk2ServerMessageDataRetriever::GetMimeTypeL
        ( const RMessage2& aMessage ) const
    {
    TInt mimeType = Pbk2MimeTypeHandler::EMimeTypeNotSupported;

    TInt length = aMessage.GetDesLengthL( KDataPackageSlot );
    if ( length > 0 )
        {
        HBufC8* dataPackage = HBufC8::NewLC( length );
        TPtr8 ptr = dataPackage->Des();
        aMessage.ReadL( KDataPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        // MIME type
        HBufC8* mimeTypeBuffer = NULL;
        Pbk2IPCPackage::InternalizeL
            ( mimeTypeBuffer, readStream, KMimeTypePosition );
        if ( mimeTypeBuffer )
            {
            CleanupStack::PushL( mimeTypeBuffer );
            mimeType = Pbk2MimeTypeHandler::MapMimeTypeL( *mimeTypeBuffer );
            CleanupStack::PopAndDestroy( mimeTypeBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( dataPackage );
        }

    return mimeType;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::SingleContactAssignFlagsL
// --------------------------------------------------------------------------
//
TUint TPbk2ServerMessageDataRetriever::SingleContactAssignFlagsL
        ( const RMessage2& aMessage ) const
    {
    TAiwSingleContactAssignDataV1Pckg data;
    aMessage.ReadL( KAssignInstructionsSlot, data );
    return data().Flags();
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::MultipleContactAssignFlagsL
// --------------------------------------------------------------------------
//
TUint TPbk2ServerMessageDataRetriever::MultipleContactAssignFlagsL
        ( const RMessage2& aMessage ) const
    {
    TAiwMultipleContactAssignDataV1Pckg data;
    aMessage.ReadL( KAssignInstructionsSlot, data );
    return data().Flags();
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::FetchFlagsL
// --------------------------------------------------------------------------
//
TUint TPbk2ServerMessageDataRetriever::FetchFlagsL
        ( const RMessage2& aMessage ) const
    {
    TUint ret = 0;

    TInt length = aMessage.GetDesLengthL( KFetchInstructionsSlot );
    if ( length > 0 )
        {
        HBufC8* data = HBufC8::NewLC( length );
        TPtr8 ptr = data->Des();
        aMessage.ReadL( KFetchInstructionsSlot, ptr );

        TAiwContactSelectionDataType dataType =
            TAiwContactSelectionDataBase::SelectionDataTypeFromBuffer( ptr );

        switch ( dataType )
            {
            case EAiwSingleEntrySelectionV1:
                {
                TAiwSingleEntrySelectionDataV1Pckg data;
                data.Copy( ptr );
                ret = data().Flags();
                break;
                }
            case EAiwSingleEntrySelectionV2:
                {
                TAiwSingleEntrySelectionDataV2Pckg data;
                data.Copy( ptr );
                ret = data().Flags();
                break;
                }
            case EAiwMultipleEntrySelectionV1:
                {
                TAiwMultipleEntrySelectionDataV1Pckg data;
                data.Copy( ptr );
                ret = data().Flags();
                break;
                }
            case EAiwMultipleEntrySelectionV2:
                {
                TAiwMultipleEntrySelectionDataV2Pckg data;
                data.Copy( ptr );
                ret = data().Flags();
                break;
                }
            case EAiwSingleItemSelectionV1:
                {
                TAiwSingleItemSelectionDataV1Pckg data;
                data.Copy( ptr );
                ret = data().Flags();
                break;
                }
            case EAiwSingleItemSelectionV2:
                {
                // Deprecated
                break;
                }
            case EAiwSingleItemSelectionV3:
                {
                TAiwSingleItemSelectionDataV3Pckg data;
                data.Copy( ptr );
                ret = data().Flags();
                break;
                }
            case EAiwMultipleItemSelectionV1:
                {
                TAiwMultipleItemSelectionDataV1Pckg data;
                data.Copy( ptr );
                ret = data().Flags();
                break;
                }
            default:
                {
                aMessage.Panic( KPanicText, EInvalidFetchType );
                break;
                }
            }

        CleanupStack::PopAndDestroy( data );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetAttributeRemovalIndicatorValueL
// --------------------------------------------------------------------------
//
TBool TPbk2ServerMessageDataRetriever::GetAttributeRemovalIndicatorValueL
        ( const RMessage2& aMessage ) const
    {
    TAiwContactAttributeAssignDataV1Pckg attributeInstructions;
    aMessage.ReadL( KAssignInstructionsSlot, attributeInstructions );
    return attributeInstructions().RemoveAttribute();
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetEditorHelpContextL
// --------------------------------------------------------------------------
//
TCoeHelpContext TPbk2ServerMessageDataRetriever::GetEditorHelpContextL
        ( const RMessage2& aMessage ) const
    {
    TAiwSingleContactAssignDataV1Pckg data;
    aMessage.ReadL( KAssignInstructionsSlot, data );
    return data().EditorHelpContext();
    }

// --------------------------------------------------------------------------
// CPbk2EntryFetchHandler::FetchDlgResourceL
// --------------------------------------------------------------------------
//
TInt TPbk2ServerMessageDataRetriever::GetFetchDialogResourceL
        ( const RMessage2& aMessage ) const
    {
    TInt result = 0;

    TInt length = aMessage.GetDesLengthL( KFetchInstructionsSlot );
    if ( length > 0 )
        {
        HBufC8* data = HBufC8::NewLC( length );
        TPtr8 ptr = data->Des();
        aMessage.ReadL( KFetchInstructionsSlot, ptr );

        TAiwContactSelectionDataType dataType =
            TAiwContactSelectionDataBase::SelectionDataTypeFromBuffer( ptr );

        switch ( dataType )
            {
            case EAiwSingleEntrySelectionV1:    // FALLTHROUGH
            case EAiwSingleEntrySelectionV2:
                {
                result = R_PBK2_SINGLE_ENTRY_FETCH_DLG;
                break;
                }

            case EAiwMultipleEntrySelectionV1:
                {
                result = R_PBK2_MULTIPLE_ENTRY_FETCH_DLG;
                TAiwMultipleEntrySelectionDataV1Pckg data;
                data.Copy( ptr );
                if ( data().Flags() & EExcludeGroupsView )
                    {
                    result = R_PBK2_MULTIPLE_ENTRY_FETCH_NO_GROUPS_DLG;
                    }
                break;
                }
            case EAiwMultipleEntrySelectionV2:
                {
                result = R_PBK2_MULTIPLE_ENTRY_FETCH_DLG;
                TAiwMultipleEntrySelectionDataV2Pckg data;
                data.Copy( ptr );
                if ( data().Flags() & EExcludeGroupsView )
                    {
                    result = R_PBK2_MULTIPLE_ENTRY_FETCH_NO_GROUPS_DLG;
                    }
                break;
                }

            case EAiwSingleItemSelectionV1: // FALLTHROUGH
            default:
                {
                break;
                }
            }
        CleanupStack::PopAndDestroy( data );
        }

    return result;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::FetchDefaultPrioritiesL
// --------------------------------------------------------------------------
//
RVPbkContactFieldDefaultPriorities
    TPbk2ServerMessageDataRetriever::FetchDefaultPrioritiesL
        ( const RMessage2& aMessage ) const
    {
    RVPbkContactFieldDefaultPriorities priorities;

    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* instructions = HBufC8::NewLC( length );
        TPtr8 ptr = instructions->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        HBufC8* prioritiesBuffer = NULL;

        Pbk2IPCPackage::InternalizeL
            ( prioritiesBuffer, readStream, KDefaultPrioritiesPosition );

        if ( prioritiesBuffer )
            {
            CleanupStack::PushL( prioritiesBuffer );
            TPtr8 prioPtr = prioritiesBuffer->Des();
            TInt prioPtrLength = prioPtr.Length();
            if ( prioPtrLength > 0 )
                {
                priorities.InternalizeL( prioPtr );
                }
            CleanupStack::PopAndDestroy( prioritiesBuffer );
            }

        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( instructions );
        }

    return priorities;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetOrietationTypeL
// --------------------------------------------------------------------------
//
TInt TPbk2ServerMessageDataRetriever::GetOrietationTypeL( const RMessage2& aMessage ) const
    {
    TInt orientationType = 0;
    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();
        
        HBufC8* orientationBuffer = NULL;
        Pbk2IPCPackage::InternalizeL
        ( orientationBuffer, readStream,
                KOrientationType );
        
        if ( orientationBuffer )
            {
            CleanupStack::PushL( orientationBuffer );
            TPtr8 ptr = orientationBuffer->Des();
            TLex8 lex( ptr );
            lex.Val( orientationType );
            CleanupStack::PopAndDestroy( orientationBuffer );
            }
        
        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }
    return orientationType;
    }

// --------------------------------------------------------------------------
// TPbk2ServerMessageDataRetriever::GetStatusPaneIdL
// --------------------------------------------------------------------------
//
TInt TPbk2ServerMessageDataRetriever::GetStatusPaneIdL
        ( const RMessage2& aMessage ) const
    {
    TInt statusPaneId( 0 );
    HBufC8* statusPaneIdInText = NULL;
    TInt length = aMessage.GetDesLengthL( KConfigurationPackageSlot );
    
    if ( length > 0 )
        {
        HBufC8* configuration = HBufC8::NewLC( length );
        TPtr8 ptr = configuration->Des();
        aMessage.ReadL( KConfigurationPackageSlot, ptr );
        RDesReadStream readStream( ptr );
        readStream.PushL();

        TRAPD( error, Pbk2IPCPackage::InternalizeL
            ( statusPaneIdInText, readStream, KStatusPaneIdPosition ) );        
        
        if ( KErrNone == error && statusPaneIdInText )
            {
            TLex8 aLex(*statusPaneIdInText);        
            error = aLex.Val( statusPaneId );
            delete statusPaneIdInText;
            }
            
        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( configuration );
        }

    return statusPaneId;
    }
    
// End of File
