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
* Description:  Phonebook 2 memory entry defaults dialog.
*
*/


// INCLUDE FILES
#include "CPbk2MemoryEntryDefaultsDlg.h"

// Phonebook 2
#include "CPbk2FieldListBoxModel.h"
#include "CPbk2DefaultAttributeProcess.h"
#include "CPbk2FieldSelector.h"
#include <CPbk2MemoryEntryDefaultsAssignDlg.h>
#include <CPbk2IconArray.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2StorePropertyArray.h>
#include <Phonebook2PrivateCRKeys.h>
#include <TPbk2DestructionIndicator.h>
#include <phonebook2ece.mbg>
#include <CPbk2ApplicationServices.h>
#include <CPbk2ServiceManager.h>
#include <MPbk2ApplicationServices.h>
#include <ccappcommlauncherpluginrsc.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkDefaultAttribute.h>
#include <CVPbkFieldFilter.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkFieldType.h>
#include <VPbkFieldType.hrh>
#include <TVPbkStoreContactAnalyzer.h>
#include <VPbkVariant.hrh>

// System includes
#include <aknlists.h>   // EikControlFactory
#include <barsread.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <centralrepository.h>
#include <gulicon.h>
#include <AknIconArray.h>
#include <eikclbd.h>
#include <aknlists.h>
#include <aknPopup.h>
#include <aknlayoutscalable_avkon.cdl.h>

//SpSettings
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>


// For checking mailbox accounts
#include <emailinterfacefactory.h>
#include <memailmailbox.h>

// Debugging headers
#include <Pbk2Debug.h>

// LOCAL CONSTANTS AND MACROS

const TInt KGranularity = 4;

const TUid KCRUidCCACommLauncher = {0x20019548};
const TInt KCommunicationMethodOrder  = 0x00000001;
const TInt KCCAppCommLauncherMaxOrderBufLength = 128;

_LIT( KPbk2ECEIconFileName, "\\resource\\apps\\phonebook2ece.mif" );

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPostCond_ConstructL,
    EPanicPreCond_FixIndex,
    EPanicPostCond_FixIndex,
    EPanicLogic_DefaultFieldL
    };

static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2MemoryEntryDefaultsDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

// Defaults settings map
struct TDefaultsItem
    {
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector selectorId;
    TInt titleResId;
    TInt iconId;
    TInt iconMaskId;
    };

/**
 * Defines Defaults appearance.
 */
const TDefaultsItem KDefaultsSettingsTable[] = {
    { VPbkFieldTypeSelectorFactory::EVoiceCallSelector, R_QTN_PHOB_SETI_CALL_DEFAULT,
    EMbmPhonebook2eceQgn_prop_pb_comm_call_large, EMbmPhonebook2eceQgn_prop_pb_comm_call_large_mask },
    { VPbkFieldTypeSelectorFactory::EEmailEditorSelector, R_QTN_PHOB_SETI_EMAIL_DEFAULT, 
    EMbmPhonebook2eceQgn_prop_pb_comm_email_large, EMbmPhonebook2eceQgn_prop_pb_comm_email_large_mask },
    { VPbkFieldTypeSelectorFactory::EInstantMessagingSelector, R_QTN_PHOB_SETI_CHAT_DEFAULT,
    EMbmPhonebook2eceQgn_prop_pb_comm_chat_large, EMbmPhonebook2eceQgn_prop_pb_comm_chat_large_mask },
    { VPbkFieldTypeSelectorFactory::EVOIPCallSelector, R_QTN_PHOB_SETI_VOIP_DEFAULT,
    EMbmPhonebook2eceQgn_prop_pb_comm_voip_large, EMbmPhonebook2eceQgn_prop_pb_comm_voip_large_mask},
    { VPbkFieldTypeSelectorFactory::EURLSelector, R_QTN_PHOB_SETI_OPENLINK_DEFAULT, 
    EMbmPhonebook2eceQgn_prop_pb_comm_url_large, EMbmPhonebook2eceQgn_prop_pb_comm_url_large_mask },
    { VPbkFieldTypeSelectorFactory::EVideoCallSelector, R_QTN_PHOB_SETI_VIDEO_DEFAULT, 
    EMbmPhonebook2eceQgn_prop_pb_comm_vcall_large, EMbmPhonebook2eceQgn_prop_pb_comm_vcall_large_mask },
    { VPbkFieldTypeSelectorFactory::EPocSelector, R_QTN_PHOB_SETI_POC_DEFAULT,
            EMbmPhonebook2eceQgn_prop_nrtyp_poc, EMbmPhonebook2eceQgn_prop_nrtyp_poc_mask },
    { VPbkFieldTypeSelectorFactory::EUniEditorSelector, R_QTN_PHOB_SETI_MESSAGE_DEFAULT,
    EMbmPhonebook2eceQgn_prop_pb_comm_message_large, EMbmPhonebook2eceQgn_prop_pb_comm_message_large_mask}
    };

const TInt KDefaultsSettingsTableLength = sizeof( KDefaultsSettingsTable ) / sizeof( TDefaultsItem );

// LOCAL FUNCTIONS

/**
 * Returns the default property at given selector id.
 *
 * @param aSelectorID           The selector id of the used default.
 * @param aDefaultProperties    Array of properties at given index on return.
 */
void PropertiesAtIndexL( const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID,
        CArrayFixFlat<TVPbkDefaultType>& aDefaultProperties )
    {
    switch ( aSelectorID )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
            {
            aDefaultProperties.AppendL( EVPbkDefaultTypePhoneNumber );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
            {
            aDefaultProperties.AppendL( EVPbkDefaultTypeVideoNumber );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdMMS))
                {
                aDefaultProperties.AppendL( EVPbkDefaultTypeMms );
                }
            else if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                aDefaultProperties.AppendL( EVPbkDefaultTypeEmailOverSms );
                }
            else
                {
                aDefaultProperties.AppendL( EVPbkDefaultTypeSms );
                }
            break;
            }
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
            {
            aDefaultProperties.AppendL( EVPbkDefaultTypeEmail );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            {
            aDefaultProperties.AppendL( EVPbkDefaultTypeVoIP );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EPocSelector:
            {
            aDefaultProperties.AppendL( EVPbkDefaultTypePOC );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            {
            aDefaultProperties.AppendL( EVPbkDefaultTypeChat );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EURLSelector:
            {
            aDefaultProperties.AppendL( EVPbkDefaultTypeOpenLink );
            break;
            }
        default:
            {
            User::Leave( KErrNotFound );
            break;
            }
        }
    }

/**
 * Returns field selector.
 * 
 * @param aFieldTypes       Fieldtype list to use in selectors
 * @param aDefaultIndex     The index of the default to use.
 * @return  Field selector which is left to cleanupstack.
 */
CPbk2FieldSelector* ConstructFieldSelectorLC( 
        const MVPbkFieldTypeList& aFieldTypes,
        const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID )
    {
     // Create composite field selector
    CPbk2FieldSelector* selector = CPbk2FieldSelector::NewLC();        

    // Construct a filtered store contact field collection
    CVPbkFieldTypeSelector* fieldTypeSelector = 
        VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL( aSelectorID , aFieldTypes );

    // takes ownership, no need to push to cleanupstack
    selector->AddIncludedSelector( fieldTypeSelector );
        
    return selector;
    }

/**
 * Creates a filed filter.
 * 
 * @param aFieldsCollection A collection of fields.
 * @param aSelectorID       The selector id of the current default.
 * @param aManager          Reference to contact manager.
 * @return  Returns a field filter.
 */
CVPbkFieldFilter* CreateFilterL
        ( MVPbkStoreContactFieldCollection& aFieldsCollection,
          const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID,
          const CVPbkContactManager& aManager )
    {
    CPbk2FieldSelector* selector = ConstructFieldSelectorLC( 
            aManager.FieldTypes(), 
            aSelectorID );

    const CVPbkFieldFilter::TConfig config
        ( const_cast<MVPbkStoreContactFieldCollection&>
            ( aFieldsCollection ), selector, NULL );
    CVPbkFieldFilter* fieldFilter = CVPbkFieldFilter::NewL( config );

    CleanupStack::PopAndDestroy( selector );

    return fieldFilter;
    }
    
/**
 * Returns fields applicable to be set as the selected default.
 * 
 * @param aContact          The contact whose fields to show.
 * @param aSelectorId       The selector id of the default to use.
 * @param aManager          Reference to contact manager.
 * @return  Collection of fields applicable to use as default.
 *          Leaves two objects on to cleanup stack. 
 */
CPbk2PresentationContactFieldCollection* ApplicableFieldsCXL
        ( CPbk2PresentationContact& aContact,
          VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorId,
          const CVPbkContactManager& aManager )
    {
    CVPbkFieldFilter* fieldFilter = 
            CreateFilterL( aContact.Fields(),
                           aSelectorId, aManager );

    CleanupStack::PushL( fieldFilter );

    CPbk2PresentationContactFieldCollection* fields =
        CPbk2PresentationContactFieldCollection::NewL
            ( aContact.PresentationFields().FieldProperties(),
              *fieldFilter, aContact );

    CleanupStack::PushL( fields );
    return fields;
    }

/**
 * Checks whether the passed contact field candidate is of
 * correct field type and has the correct attribute assigned.
 *
 * @param aCandidate        Contact field candidate.
 * @param aSelectorId       Selector id.
 * @param aAttribute        Contact attribute.
 * @param aManager          Virtual Phonebook contact manager.
 * @return  ETrue if the candidate fulfills all the requirements.
 */
TBool CandidateApprovedL( MVPbkStoreContactField& aCandidate,
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorId,
        CVPbkDefaultAttribute& aAttribute,
        CVPbkContactManager& aManager )
    {
    TBool ret = EFalse;
    
    // Verify the field is of correct type
    TVPbkStoreContactAnalyzer analyzer( aManager, NULL );
    if ( analyzer.IsFieldTypeIncludedL( aCandidate, aSelectorId ) )
        {
        // Check if field has default attribute defaultType
        if ( aManager.ContactAttributeManagerL().
             HasFieldAttributeL( aAttribute, aCandidate ) )
            {
            ret = ETrue;
            }
        }

    return ret;
    }

/**
 * Finds default field identified by given index.
 *
 * @param aContact      The contact where to find the default from.
 * @param aSelectorId   The selector id of the default to use.
 * @param aManager      A reference to contact manager.
 * @return  Store contact field which has the appropriate default assigned,
 *          or NULL if not found.
 */
MVPbkStoreContactField* FindDefaultFieldL
        ( MVPbkStoreContact& aContact,
          const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorId,
          CVPbkContactManager& aManager )
    {
    MVPbkStoreContactField* field = NULL;

    // First find all the default properties matching the default type
    CArrayFixFlat<TVPbkDefaultType>* defaultProperties =
        new( ELeave ) CArrayFixFlat<TVPbkDefaultType>( KGranularity );
    CleanupStack::PushL( defaultProperties );
    PropertiesAtIndexL
        ( aSelectorId, *defaultProperties );

    // Check if the contact has any of the default properties defined
    for ( TInt i = 0; i < defaultProperties->Count() && !field; ++i )
        {
        // Loop through contact's fields and find the specified field
        CVPbkDefaultAttribute* attr =
            CVPbkDefaultAttribute::NewL( defaultProperties->At( i ) );
        CleanupStack::PushL( attr );

        const TInt fieldCount = aContact.Fields().FieldCount();
        for ( TInt j = 0; j < fieldCount && !field ; ++j )
            {
            MVPbkStoreContactField* candidate =
                aContact.Fields().FieldAtLC( j );

            if ( CandidateApprovedL
                ( *candidate, aSelectorId, *attr, aManager ) )
                {
                field = candidate;
                CleanupStack::Pop(); // candidate
                }
            else
                {
                CleanupStack::PopAndDestroy(); // candidate
                }
            }

        CleanupStack::PopAndDestroy ( attr );
        }

    CleanupStack::PopAndDestroy( defaultProperties );

    return field;
    }

/**
 * Returns default field identified by aDefaultIndex from aContact
 * or NULL if the default is not set.
 *
 * @param aContact          Reference to a contact.
 * @param aSelectorId       The selector id of the default to use.
 * @param aManager          Reference to contact manager.
 * @return  The correct default field.
 */
CPbk2PresentationContactField* DefaultFieldL
        ( CPbk2PresentationContact& aContact,
          const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorId,
          CVPbkContactManager& aManager )
    {
    CPbk2PresentationContactField* result = NULL;

    // Find the correct default field
    MVPbkStoreContactField* defaultField = FindDefaultFieldL
        ( aContact.StoreContact(), aSelectorId, aManager );

    if ( defaultField )
        {
        // Find the corresponding presentation field
        CPbk2PresentationContactFieldCollection& presentationFields =
            aContact.PresentationFields();

        for (TInt i = 0; i < presentationFields.FieldCount(); ++i)
            {
            CPbk2PresentationContactField& victim =
                presentationFields.At( i );
            if ( victim.IsSame( *defaultField ) )
                {
                result = &victim;    // temporary reference is ok
                break;
                }
            }
        }
    delete defaultField;

    return result;
    }

/**
 * Returns the default UI text for default field identified by aDefaultIndex.
 *
 * @param aSelectorId   Selector id of the default.
 * @return  Text for the default.
 */
HBufC* DefaultFieldTextLC( const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorId )
    {
    for( TInt idx = 0; idx < KDefaultsSettingsTableLength; idx++ )
        {
        TDefaultsItem tmpItem = KDefaultsSettingsTable[ idx ];
        if( tmpItem.selectorId == aSelectorId )
            {
            return StringLoader::LoadLC( tmpItem.titleResId );
            }
        }
    
    User::Leave(KErrNotFound);
    return NULL;
    } 

/**
 * Loads and creates an icon from given indexes.
 *
 * @param aBmpId    Index of icon.
 * @param aMaskId   Index of icon mask.
 * @return          An icon.
 */   
CGulIcon* LoadIconLC( TInt aBmpId, TInt aMaskId )
    {
    CFbsBitmap* bmp = NULL;
    CFbsBitmap* mask = NULL;
    CGulIcon* icon = CGulIcon::NewLC();
    
    AknIconUtils::CreateIconLC( 
        bmp, mask, KPbk2ECEIconFileName, aBmpId, aMaskId );
                               
    icon->SetBitmap( bmp );
    icon->SetMask( mask );
    CleanupStack::Pop( 2 ); // bmp, mask

    return icon;
    }

/**
 * Loads and creates an icon from given indexes.
 *
 * @param aBmpId    Index of icon.
 * @param aMaskId   Index of icon mask.
 * @return          An icon.
 */   
CGulIcon* MapSelectorId2IconLC( const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorId )
    {
    for( TInt idx = 0; idx < KDefaultsSettingsTableLength; idx++ )
        {
        TDefaultsItem tmpItem = KDefaultsSettingsTable[ idx ];
        if( tmpItem.selectorId == aSelectorId )
            {
            return LoadIconLC( tmpItem.iconId, tmpItem.iconMaskId );
            }
        }
        
    User::Leave( KErrNotFound );
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::CPbk2MemoryEntryDefaultsDlg
// --------------------------------------------------------------------------
//
inline CPbk2MemoryEntryDefaultsDlg::CPbk2MemoryEntryDefaultsDlg
        ( CPbk2PresentationContact& aContact,
          CVPbkContactManager& aManager ):
            iContact( aContact ), iManager( aManager ), iLVFlags( 0 ), iServiceName( NULL )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::~CPbk2MemoryEntryDefaultsDlg
// --------------------------------------------------------------------------
//
CPbk2MemoryEntryDefaultsDlg::~CPbk2MemoryEntryDefaultsDlg()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg destructor start"));
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }
    delete iListBox;
    delete iAttributeProcess;
    delete iDefaultsTable;
    delete iServiceName;

    FeatureManager::UnInitializeLib();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg destructor end"));
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2MemoryEntryDefaultsDlg* CPbk2MemoryEntryDefaultsDlg::NewL
        ( CPbk2PresentationContact& aContact, CVPbkContactManager& aManager )
    {
    CPbk2MemoryEntryDefaultsDlg* self = new(ELeave)
        CPbk2MemoryEntryDefaultsDlg(aContact, aManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2MemoryEntryDefaultsDlg::ConstructL()
    {
    // Initialize feature manager
    FeatureManager::InitializeLibL();

    // Read local variation flags
    CRepository* key = CRepository::NewL(TUid::Uid(KCRUidPhonebook));
    TInt err = key->Get(KPhonebookLocalVariationFlags, iLVFlags);
    if (err != KErrNone)
        {
        // If there were problems reading the flags, assume everything is off
        iLVFlags = 0;
        }
    delete key;

    // Create a list box
    iListBox = static_cast<CEikFormattedCellListBox*>
        (EikControlFactory::CreateByTypeL
                (EAknCtDoubleLargeGraphicPopupMenuListBox).iControl);


	CAknPopupList::ConstructL
		(iListBox, R_PBK2_SOFTKEYS_OK_BACK_OK,
		AknPopupLayouts::EMenuDoubleLargeGraphicWindow);
      
    // Init list box
    iListBox->ConstructL( this, CEikListBox::ELeftDownInViewRect );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    
    // Set title of pop up list
    HBufC* heading = StringLoader::LoadLC
        ( R_QTN_PHOB_TITLE_DEFAULT_SETTINGS );
    SetTitleL( *heading );
    CleanupStack::PopAndDestroy( heading );
    
    iDefaultsTable 
       = new (ELeave) CArrayFixFlat<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>( KGranularity );
        
    CreateTableOfDefaultsL();
    
    CreateLinesL();

    __ASSERT_DEBUG(iListBox && !iDestroyedPtr,
        Panic(EPanicPostCond_ConstructL));
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2MemoryEntryDefaultsDlg::ExecuteLD()
    {
    CAknPopupList::ExecuteLD();
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::ProcessCommandL( TInt aCommandId )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg::ProcessCommandL start"));

    switch ( aCommandId )
        {
        case EAknSoftkeyOk:
            {
            // Equal item order in iDefaultsTable and iListBox
            TInt currentItemIndex( iListBox->CurrentItemIndex() );
            // iListBox and iDefaultsTable have the same lenght. But its
            // possible that further changes could break that assumption.
            if( currentItemIndex < iDefaultsTable->Count() )
                DefaultsAssignPopupL( iDefaultsTable->At( currentItemIndex ) );
            break;
            }

        default:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("About to call CAknPopupList::ProcessCommandL"));
            CAknPopupList::ProcessCommandL( aCommandId );
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("Called CAknPopupList::ProcessCommandL"));
            break;
            }
        }
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg::ProcessCommandL end"));
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::HandleListBoxEventL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::HandleListBoxEventL
        ( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    // Respond to events from listbox item
    if ( aListBox == iListBox )
        {
        // Handle pointer events - copied source from 
        // CCCAppCommLauncherContainer::HandleListBoxEventL 
        TBool executeContactAction = EFalse;
        if ( aEventType == EEventItemDraggingActioned )
            {
            iHasBeenDragged = ETrue;
            }

        else if ( aEventType == EEventItemSingleClicked )

        	{
            if (!iHasBeenDragged)
                {
                executeContactAction = ETrue;
                }
            else
                {
                iHasBeenDragged = EFalse;
                }
            }
        else if ( aEventType == EEventEnterKeyPressed)
            {
            executeContactAction = ETrue;
            iHasBeenDragged = EFalse;
            }
        
        if ( executeContactAction )
            {
            ProcessCommandL( EAknSoftkeyOk );     
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::AttributeProcessCompleted
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::AttributeProcessCompleted()
    {
    delete iField;
    iField = NULL;

    // Redraw
    TRAP_IGNORE( CreateLinesL() );
    iListBox->DrawDeferred();
    DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::AttributeProcessFailed
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::AttributeProcessFailed( TInt aErrorCode )
    {
    delete iField;
    iField = NULL;

    iCoeEnv->HandleError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::CreateLinesL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::CreateLinesL() const
    {
    CDesCArray* lines = static_cast<CDesCArray*>
        (iListBox->Model()->ItemTextArray());
    lines->Reset();

    // int = icon id, string = first text, string = second text
    _LIT(KFormat, "%d\t%S\t%S\t" );
    HBufC* noDefault = StringLoader::LoadLC( R_QTN_PHOB_SETI_NO_DEFAULT );
    
    for ( TInt idx = 0; idx < iDefaultsTable->Count(); ++idx )
        {
        HBufC* defaultFieldText = DefaultFieldTextLC( iDefaultsTable->At(idx) );
        const CPbk2PresentationContactField* field =
            DefaultFieldL( iContact, iDefaultsTable->At(idx), iManager );
        TPtrC label;
        if( field )
            {
            label.Set( field->FieldLabel() );
            }
        else
            {
            label.Set( *noDefault );
            }

        // Allocate and format the listbox line
        HBufC* lineBuf = NULL;
        // If only one Voip service avaliable, use voip service name on internet call item
        if ( iServiceName && 
        		( iDefaultsTable->At( idx ) == VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) )
        	{
        	// Use Voip service name
            HBufC* str = StringLoader::LoadLC( R_QTN_CCA_VOIP_CALL_WITH_SERVICENAME, 
                        *iServiceName,             
                        iCoeEnv );
        	lineBuf = HBufC::NewLC( KFormat().Length()
        	            + str->Length() + label.Length() );
        	TPtr line( lineBuf->Des() );
        	line.Format( KFormat, idx, str, &label);
        	lines->AppendL( line ) ;
        	CleanupStack::PopAndDestroy(2); // lineBuf, str
        	}
        else
        	{
        	// Use default text
        	lineBuf = HBufC::NewLC( KFormat().Length()
        	            + defaultFieldText->Length() + label.Length() );
        	TPtr line( lineBuf->Des() );
        	line.Format( KFormat, idx, defaultFieldText, &label);
        	lines->AppendL( line ) ;
        	CleanupStack::PopAndDestroy(); // lineBuf
        	}
        
        // Cleanup
        CleanupStack::PopAndDestroy(); // defaultFieldText        
        }
   
    CleanupStack::PopAndDestroy(); // noDefault
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::DefaultsAssignPopupL
// --------------------------------------------------------------------------
//
TBool CPbk2MemoryEntryDefaultsDlg::DefaultsAssignPopupL
        ( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID )
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator
        ( &thisDestroyed, iDestroyedPtr );

    TBool result = EFalse;

    // Construct a presentation contact field collection,
    // containing fields that apply to the field type requirements
    // of the chosen default
    CPbk2PresentationContactFieldCollection* fields =
        ApplicableFieldsCXL( iContact, aSelectorID, iManager );

    // Construct and launch the assign dialog
    CPbk2MemoryEntryDefaultsAssignDlg* assignDlg =
        CPbk2MemoryEntryDefaultsAssignDlg::NewL();
   const TInt index = assignDlg->ExecuteLD(*fields, iManager,
          iContact.PresentationFields().FieldProperties(), aSelectorID);

    if (!thisDestroyed)
        {
        // Next find the field index is referencing
        MVPbkStoreContactField* field = NULL;
        const TInt fieldCount = fields->FieldCount();
        // Notice that the last index of the listbox
        // is bound to 'no default' selection, hence the index
        // must be smaller than field count.
        if (index > KErrNotFound && index < fieldCount)
            {
            // Set the new default
            field = &fields->At(index);
            if (field)
                {
                SetDefaultL(aSelectorID, *field);
                }
            }

        else if (index == fieldCount)
            {
            // User selected the 'no default' option
            RemoveDefaultL( aSelectorID );
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // ApplicableFieldsLCX
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::IsSupported
// --------------------------------------------------------------------------
//
TBool CPbk2MemoryEntryDefaultsDlg::IsSupported
        ( const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID ) const
    {
    TBool ret(ETrue);

    // Skip the line if Email field not supported or having no mailbox
    if ( aSelectorID == VPbkFieldTypeSelectorFactory::EEmailEditorSelector )
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdEmailUi ) )
        	{
        	ret = EFalse;
        	}
        }
    // Do not add video fields if they are not enabled
    else if ( ( aSelectorID == VPbkFieldTypeSelectorFactory::EVideoCallSelector ) &&
              !( iLVFlags & EVPbkLVAddVideoTelephonyFields ) )
        {
        ret = EFalse;
        }
    // Do not add Voip fields if they are not enabled
    else if ( aSelectorID == VPbkFieldTypeSelectorFactory::EVOIPCallSelector &&
              !FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        ret = EFalse;
        }
    // Do not add POC fields if they are not enabled
    else if ( ( aSelectorID == VPbkFieldTypeSelectorFactory::EPocSelector ) &&
              !( iLVFlags & EVPbkLVPOC ) )
        {
        ret = EFalse;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::HasMailboxAccountsL
// --------------------------------------------------------------------------
//
TBool CPbk2MemoryEntryDefaultsDlg::HasMailboxAccountsL() const
	{
	using namespace EmailInterface;

	TBool result = EFalse;
	CEmailInterfaceFactory* factory = CEmailInterfaceFactory::NewL();
	CleanupStack::PushL( factory );
	MEmailInterface* ifPtr = factory->InterfaceL( KEmailClientApiInterface );
	MEmailClientApi* clientApi = static_cast<MEmailClientApi*>( ifPtr );
	CleanupReleasePushL( *clientApi );
	
	// Get mailboxs
	RMailboxPtrArray mailboxes;
	clientApi->GetMailboxesL( mailboxes );
	TInt count = mailboxes.Count();	
	if ( count > 0 )
		{
		result = ETrue;
		}
	
	// Release mailboxs before releasing clientapi
	for ( TInt i=0; i<count; i++ )
		{
		MEmailMailbox* mailbox = mailboxes[i];
		mailbox->Release();
		}	
	mailboxes.Close();	
	CleanupStack::PopAndDestroy( 2 ); // clientApi and factory
	
	return result;
	}
// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::IsAvailable
// --------------------------------------------------------------------------
//
TBool CPbk2MemoryEntryDefaultsDlg::IsAvailableL
        ( const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID ) const
    {
    CVPbkFieldFilter* filter = CreateFilterL(
              iContact.Fields(), aSelectorID, iManager );
  
    TInt nbrOfEntries = filter->FieldCount();
    
    delete filter;
    
    return ( 0 < nbrOfEntries );
    }

void CPbk2MemoryEntryDefaultsDlg::CreateTableOfDefaultsL()
    {
    TRAPD( err, ReadDefaultsFromRepositoryL() )
    if( err != KErrNone )
        {
        //add defaults without order
        iDefaultsTable->Reset();
        iDefaultsTable->AppendL( VPbkFieldTypeSelectorFactory::EVoiceCallSelector );
        iDefaultsTable->AppendL( VPbkFieldTypeSelectorFactory::EVOIPCallSelector );
        iDefaultsTable->AppendL( VPbkFieldTypeSelectorFactory::EUniEditorSelector );
        iDefaultsTable->AppendL( VPbkFieldTypeSelectorFactory::EEmailEditorSelector );
        iDefaultsTable->AppendL( VPbkFieldTypeSelectorFactory::EInstantMessagingSelector );
        iDefaultsTable->AppendL( VPbkFieldTypeSelectorFactory::EURLSelector );
        iDefaultsTable->AppendL( VPbkFieldTypeSelectorFactory::EVideoCallSelector );
        }
    
    TInt idx;
    
    // Filter services
    // Only show supported and contact-related items
    for( idx = 0; idx < iDefaultsTable->Count(); idx++ )
        {        
        if( !IsSupported( iDefaultsTable->At( idx ) ) ||
            !IsAvailableL( iDefaultsTable->At( idx ) ) 
          )
            {
            // remove item at current position from array
            iDefaultsTable->Delete( idx, 1 );
            idx--;
            }
        }
    
    // Load icon of each RELEVANT communication 
    // methode and add it to the listbox.
    CAknIconArray* iconArray = new (ELeave) CAknIconArray( KGranularity );
    CleanupStack::PushL( iconArray );
    for( idx = 0; idx < iDefaultsTable->Count(); idx++ )
        { 
        CGulIcon* icon = MapSelectorId2IconLC( iDefaultsTable->At( idx ) );
        
        // Important: iconArray positions matchs
        // with iDefaultsTable positions
        iconArray->AppendL( icon );
        
        // If only one Voip service avaliable, use brand icon on internet call item
        TServiceId serviceId;
        if ( ( iDefaultsTable->At( idx ) == VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) &&
        		( SupportedVOIPServicesL( serviceId ) == 1 ) )
        	{
            CFbsBitmap* bitmap (NULL);
            CFbsBitmap* mask (NULL);

            // Load Voip brand bitmap and mask              
            LoadVoipServiceInfoL( serviceId, bitmap, mask );
            
            if ( bitmap || mask )
                {          
                iconArray->At(idx)->SetBitmap(bitmap);
                iconArray->At(idx)->SetMask(mask);
                }
        	}
        
        CleanupStack::Pop(1); // icon
        }  
    // iListBox takes ownership of iconArray
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );
    CleanupStack::Pop(1); // iconArray
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::ReadDefaultsFromRepositoryL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::ReadDefaultsFromRepositoryL()
    {
    //*** read item order out of repository ***
    CRepository* repository = CRepository::NewLC( KCRUidCCACommLauncher );
    TBuf<KCCAppCommLauncherMaxOrderBufLength> orderBuf;
    TInt error = repository->Get( KCommunicationMethodOrder, orderBuf );
    CleanupStack::PopAndDestroy( repository );
    
    TLex orderLex( orderBuf );
    TPtrC numToken;
    TLex numLex;
    TInt commMethodType;
    const TInt bufLength = orderBuf.Length() - 1;

    while ( orderLex.Offset() < bufLength )
        {
        numToken.Set( orderLex.NextToken() );
        numLex.Assign( numToken );
        // if defined value is string instead of a number,
        // use the default array
        User::LeaveIfError( numLex.Val( commMethodType ));
        // if defined value is outside of the possible comm method
        // values, use the default array
        TBool validMethod(EFalse);    
        for( TInt idx = 0; idx < KDefaultsSettingsTableLength; idx++ )
            {
            if( KDefaultsSettingsTable[ idx ].selectorId == commMethodType )
                {
                validMethod = ETrue;
                break;
                }
            }
        User::LeaveIfError( !validMethod ? KErrArgument : 0 );
        
        // If commMethodType invalid, function leaves with KErrNotFound
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector selectorID =
        VPbkFieldTypeSelectorFactory::CreateActionTypeSelectorIdL( commMethodType );
 
        iDefaultsTable->AppendL( selectorID );  
        }    

    // if no comm methods defined, use the default array
    User::LeaveIfError( iDefaultsTable->Count() ? 0 : KErrNotFound );
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::SetDefaultL
// Sets aField as the default (identified by aDefaultIndex) field
// for the contact.
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::SetDefaultL
        ( const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID,
          MVPbkStoreContactField& aField )
    {
    delete iField;
    iField = NULL;

    // Find actual store contact field from presentation
    // contact's store contact
    for (TInt i = 0; i < iContact.StoreContact().Fields().FieldCount(); ++i)
        {
        MVPbkStoreContactField* actualField =
            iContact.StoreContact().Fields().FieldAtLC( i );
        if ( aField.IsSame( *actualField ) )
            {
            iField = actualField;
            CleanupStack::Pop(); // actualField
            break;
            }
        else
            {
            CleanupStack::PopAndDestroy(); // actualField
            }
        }

    if ( iField )
        {
        // Init attribute set/remove process object
        delete iAttributeProcess;
        iAttributeProcess = NULL;
        iAttributeProcess = CPbk2DefaultAttributeProcess::NewL
            ( iManager, iContact.StoreContact(), *this );

        CArrayFixFlat<TVPbkDefaultType>* defaultProperties =
            new( ELeave ) CArrayFixFlat<TVPbkDefaultType>( KGranularity );
        CleanupStack::PushL( defaultProperties );
        PropertiesAtIndexL
            ( aSelectorID, *defaultProperties );
        CleanupStack::Pop( defaultProperties );
        iAttributeProcess->SetDefaultsL( defaultProperties, *iField );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::RemoveDefaultL
// Removes default (identified by aDefaultIndex) from aContact.
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::RemoveDefaultL
        ( const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID )
    {
    delete iAttributeProcess;
    iAttributeProcess = NULL;
    iAttributeProcess = CPbk2DefaultAttributeProcess::NewL
        ( iManager, iContact.StoreContact(), *this );

    CArrayFixFlat<TVPbkDefaultType>* defaultProperties =
        new( ELeave ) CArrayFixFlat<TVPbkDefaultType>( KGranularity );
    CleanupStack::PushL( defaultProperties );
    PropertiesAtIndexL
        ( aSelectorID, *defaultProperties );
    CleanupStack::Pop( defaultProperties );
    iAttributeProcess->RemoveDefaultsL( defaultProperties );
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::SupportedVOIPServicesL
// Get supported Voip services
// --------------------------------------------------------------------------
//
TInt CPbk2MemoryEntryDefaultsDlg::SupportedVOIPServicesL( TServiceId& aServiceId )
    {   
    TInt availableVoipService = 0;    
    RIdArray idArray;
    CleanupClosePushL(idArray);
    
    CSPSettings* settings = CSPSettings::NewL();
    CleanupStack::PushL( settings );
    
    User::LeaveIfError( settings->FindServiceIdsL(idArray) ); 
    
    const TInt count = idArray.Count();   
    for (TInt i = 0; i < count; ++i)
        {
        TBool supported( EFalse );
        CSPEntry* entry = CSPEntry::NewLC();
        TServiceId id = idArray[i];
        User::LeaveIfError( settings->FindEntryL(id, *entry) );
        const CSPProperty* property = NULL;
        
        if (entry->GetProperty(property, EPropertyServiceAttributeMask) == KErrNone)
            {
            TInt value = 0;
            property->GetValue(value);
            supported = value & ESupportsInternetCall; 
            }
        
        if ( supported )
            {
            availableVoipService++;            
            aServiceId = id;
            }
        CleanupStack::PopAndDestroy(); // entry
        }
    CleanupStack::PopAndDestroy(2); // settings, idArray    
    
    return availableVoipService;
    }

// ---------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::LoadVoipServiceInfoL
// Load Voip service info
// ---------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::LoadVoipServiceInfoL( 
            TServiceId aServiceId,
            CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {   
    CPbk2ApplicationServices* appServices =
        CPbk2ApplicationServices::InstanceLC();
    
    CPbk2ServiceManager& servMan = appServices->ServiceManager();
    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();
    
    const TInt count = services.Count();   
    for ( TInt i = 0; i < count; i++ )
        {
        const CPbk2ServiceManager::TService& service = services[i];

        if ( service.iServiceId == aServiceId )
            {
            // Calculate preferred size for xsp service icon 
            TRect mainPane;
            AknLayoutUtils::LayoutMetricsRect(
                AknLayoutUtils::EMainPane, mainPane );
            TAknLayoutRect listLayoutRect;
            listLayoutRect.LayoutRect(
                mainPane,
                AknLayoutScalable_Avkon::list_double_large_graphic_pane_g1(0).LayoutLine() );
            TSize size(listLayoutRect.Rect().Size());
            
            AknIconUtils::SetSize(
                    service.iBitmap,
                    size );
            AknIconUtils::SetSize(
                    service.iMask,
                    size );                               
    		
            aBitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL( aBitmap );
            aBitmap->Duplicate( service.iBitmap->Handle() );
            
            aMask = new (ELeave) CFbsBitmap;
            CleanupStack::PushL( aMask );
            aMask->Duplicate( service.iMask->Handle() );                    
                     
            iServiceName = service.iDisplayName.AllocL();
            CleanupStack::Pop( 2 ); // aMask, aBitmap
            
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // appServices
    }

//  End of File
