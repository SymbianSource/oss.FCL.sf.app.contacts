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
* Description:  Phonebook 2 contact info list box.
*
*/


// INCLUDE FILES
#include "CPbk2ContactInfoListBox.h"

// Phonebook 2
#include "CPbk2ContactInfoDataSwapper.h"
#include "CPbk2IconArray.h"
#include "CPbk2FieldListBoxModel.h"
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionIconSupport.h>
#include <Pbk2Profile.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkFieldType.h>
#include <CVPbkDefaultAttribute.h>
#include <VPbkUtils.h>
#include <CVPbkVoiceTagAttribute.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>

// System includes
#include <barsread.h>
#include <eikclbd.h>
#include <eikfrlb.h>
#include <StringLoader.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Returns CPbk2IconArray icon array from the given list box.
 *
 * @param aListBox  List box.
 * @return  Phonebook 2 icon array.
 */
inline CPbk2IconArray* IconArray(
        const CEikFormattedCellListBox& aListBox )
    {
    return static_cast<CPbk2IconArray*>(
        aListBox.ItemDrawer()->ColumnData()->IconArray() );
    }

/**
 * Sets aIconArray as aListBox'es icon array.
 *
 * @param aListBox      List box.
 * @param aIconArray    Phonebook 2 icon array.
 */
inline void SetIconArray(
        CEikFormattedCellListBox& aListBox,
        CPbk2IconArray* aIconArray )
    {
    aListBox.ItemDrawer()->ColumnData()->SetIconArray( aIconArray );
    }


#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPostCond_DoUpdateL = 1
    };

void Panic( TInt aReason )
    {
    _LIT( KPanicText, "CPbk2ContactInfoListBox" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// MODULE DATA STRUCTURES

/**
 * Listbox item drawer.
 */
NONSHARABLE_CLASS( CPbk2ContactInfoListBox::CItemDrawer ) :
        public CFormattedCellListBoxItemDrawer
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aTextListBoxModel     The model whose items will be drawn.
         * @param aFont                 The font with which the items
         *                              will be drawn.
         * @param aFormattedCellData    The column list box data.
         * @param aManager              Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2ContactInfoListBox::CItemDrawer* NewL(
                MTextListBoxModel* aTextListBoxModel,
                const CFont* aFont,
                CFormattedCellListBoxData* aFormattedCellData,
                CVPbkContactManager& aManager );

        /**
         * Destructor.
         */
        ~CItemDrawer();

    public: // Interface

        /**
         * Sets contact field collection.
         *
         * @param aFields   The field collection to set.
         */
        inline void SetFields(
                const CPbk2PresentationContactFieldCollection& aFields );

    private: // From CFormattedCellListBoxItemDrawer
        TListItemProperties Properties(
                TInt aItemIndex ) const;

    private: // Implementation
        CItemDrawer(
                MTextListBoxModel* aTextListBoxModel,
                const CFont* aFont,
                CFormattedCellListBoxData* aFormattedCellData,
                CVPbkContactManager& aManager );
        void ConstructL();

    private: // Data
        /// Ref: Array of fields
        const CPbk2PresentationContactFieldCollection* iFields;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iManager;
        /// Own: Field type selector
        CVPbkFieldTypeSelector* iPhoneNumberFieldSelector;
    };

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CItemDrawer::CItemDrawer
// --------------------------------------------------------------------------
//
inline CPbk2ContactInfoListBox::CItemDrawer::CItemDrawer(
        MTextListBoxModel* aTextListBoxModel, const CFont* aFont,
        CFormattedCellListBoxData* aFormattedCellData,
        CVPbkContactManager& aManager ) :
    CFormattedCellListBoxItemDrawer( aTextListBoxModel, aFont,
        aFormattedCellData ), iManager( aManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CItemDrawer::~CItemDrawer
// --------------------------------------------------------------------------
//
CPbk2ContactInfoListBox::CItemDrawer::~CItemDrawer()
    {
    delete iPhoneNumberFieldSelector;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CItemDrawer::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactInfoListBox::CItemDrawer*
    CPbk2ContactInfoListBox::CItemDrawer::NewL(
        MTextListBoxModel* aTextListBoxModel,
        const CFont* aFont,
        CFormattedCellListBoxData* aFormattedCellData,
        CVPbkContactManager& aManager )
    {
    CPbk2ContactInfoListBox::CItemDrawer* self =
        new (ELeave) CPbk2ContactInfoListBox::CItemDrawer(
            aTextListBoxModel, aFont, aFormattedCellData, aManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CItemDrawer::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::CItemDrawer::ConstructL()
    {
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC(
        resReader, R_PHONEBOOK2_PHONENUMBER_SELECTOR );

    iPhoneNumberFieldSelector = CVPbkFieldTypeSelector::NewL
        ( resReader, iManager.FieldTypes() );

    CleanupStack::PopAndDestroy(); // resReader
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CItemDrawer::SetFields
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoListBox::CItemDrawer::SetFields(
        const CPbk2PresentationContactFieldCollection& aFields )
    {
    iFields = &aFields;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CItemDrawer::Properties
// --------------------------------------------------------------------------
//
TListItemProperties CPbk2ContactInfoListBox::CItemDrawer::Properties(
        TInt aItemIndex ) const
    {
    TListItemProperties result =
        CFormattedCellListBoxItemDrawer::Properties( aItemIndex );

    if ( iFields && ( iFields->FieldCount() > aItemIndex ) )
        {
        const CPbk2PresentationContactField* field =
            &iFields->At( aItemIndex );

        // Get the field type
        const MVPbkFieldType* fieldType =
            VPbkUtils::MatchFieldType( iManager.FieldTypes(), *field );

        // If leave occurs, the field prompt will be not underlined.
        TRAP_IGNORE(
            {
            CVPbkDefaultAttribute* attr =
                CVPbkDefaultAttribute::NewL( EVPbkDefaultTypePhoneNumber );
            CleanupStack::PushL( attr );
            if ( iPhoneNumberFieldSelector->IsFieldTypeIncluded( *fieldType )
                && iManager.ContactAttributeManagerL().HasFieldAttributeL
                    ( *attr, field->StoreField() ) )
                {
                // Default phone number field prompt is underlined
                result.SetUnderlined( ETrue );
                }
            CleanupStack::PopAndDestroy( attr );
            }); // TRAP_IGNORE
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CPbk2ContactInfoListBox
// --------------------------------------------------------------------------
//
inline CPbk2ContactInfoListBox::CPbk2ContactInfoListBox(
        CVPbkContactManager& aManager ) :
    iManager( aManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::~CPbk2ContactInfoListBox
// --------------------------------------------------------------------------
//
CPbk2ContactInfoListBox::~CPbk2ContactInfoListBox()
    {
    delete iVoiceTagFindOperation;
    delete iResourceData.iFindEmptyText;
    delete iModel;
    delete iFieldAnalyzer;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactInfoListBox* CPbk2ContactInfoListBox::NewL(
        CCoeControl& aContainer,
        TInt aResourceId,
        CVPbkContactManager& aManager )
    {
    CPbk2ContactInfoListBox* self =
        new (ELeave) CPbk2ContactInfoListBox( aManager );
    CleanupStack::PushL( self );
    self->ConstructL( aContainer, aResourceId );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoListBox::ConstructL(
        CCoeControl& aContainer, TInt aResourceId )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoListBox::ConstructL"));

    iFieldAnalyzer = CPbk2FieldAnalyzer::NewL( iManager );

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    iResourceData.iFlags = reader.ReadUint32();         // flags

    SetContainerWindowL( aContainer );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoListBox::ConstructL about to resource construct"));

    // Let Avkon read it's share of the resources
    ConstructFromResourceL( reader );

    View()->SetListEmptyTextL( KNullDesC );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoListBox::ConstructL about to create icon array"));

    // Set icon array
    const TInt iconArrayRes = reader.ReadInt32();
    TResourceReader iconArrayreader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( iconArrayreader, iconArrayRes );
    CPbk2IconArray* iconArray = CPbk2IconArray::NewL( iconArrayreader );
    CleanupStack::PushL( iconArray );

    // Add icons from UI extensions
    CPbk2UIExtensionManager* extManager =
        CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();
    extManager->IconSupportL().AppendExtensionIconsL( *iconArray );
    CleanupStack::PopAndDestroy(); // extManager
    CleanupStack::Pop( iconArray );
    CleanupStack::PopAndDestroy(); // iconArrayreader
    SetIconArray( *this, iconArray );

    CleanupStack::PopAndDestroy(); // reader

    CreateScrollBarFrameL( ETrue );
    ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Take ownership of the default listbox model
    iModel = Model()->ItemTextArray();
    Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoListBox::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::PrepareForUpdateL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::PrepareForUpdateL(
        CPbk2ContactInfoDataSwapper& aDataSwapper,
        const CPbk2PresentationContactFieldCollection& aFieldCollection,
        MPbk2FieldPropertyArray& aFieldProperties,
        const CPbk2ContactFieldDynamicProperties* aDynamicProperties )
    {
    // cancel voice tag operation
    if (iVoiceTagFindOperation)
        {
        delete iVoiceTagFindOperation;
        iVoiceTagFindOperation = NULL;
        }

    // Create a new list box model based on the
    // new field collection
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxPrepareForUpdateLCreateListBoxModel);
    CPbk2FieldListBoxModel* model = CreateListBoxModelL(
        iManager, aFieldCollection, aFieldProperties, aDynamicProperties );
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxPrepareForUpdateLCreateListBoxModel);

    // Store the members which are to be swapped
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxPrepareForUpdateLStoreListBoxState);
    aDataSwapper.StoreListBoxState( *this );
    aDataSwapper.iListBoxModel = model;
    aDataSwapper.iFieldCollection = &aFieldCollection;
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxPrepareForUpdateLStoreListBoxState);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::DoUpdateL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::DoUpdateL(
        CPbk2ContactInfoDataSwapper& aDataSwapper,
        MVPbkStoreContact& aContact )
    {
    // Change state
    aDataSwapper.ResetListBoxState();
    aDataSwapper.Swap( iFieldCollection, aDataSwapper.iFieldCollection );
    aDataSwapper.Swap( iModel, aDataSwapper.iListBoxModel );

    // Redraw
    ItemDrawer()->SetFields( *iFieldCollection );
    Model()->SetItemTextArray( iModel );
    Reset();
    // The leaving methods have to be executed last
    UpdateScrollBarsL();

    // create asynchronous voice tag retriever
    TRAP_IGNORE(DoCreateVoiceTagRetrieverL(aContact));

    __ASSERT_DEBUG(iModel, Panic(EPanicPostCond_DoUpdateL));
    __ASSERT_DEBUG(Model()->ItemTextArray() == iModel,
            Panic(EPanicPostCond_DoUpdateL));
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::NumberOfItems
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoListBox::NumberOfItems() const
    {
    return 0;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoListBox::ContactsMarked() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CurrentItemIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoListBox::CurrentItemIndex() const
    {
    return CAknFormDoubleGraphicStyleListBox::CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::DoHandleResourceChangeL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::DoHandleResourceChangeL
        ( const TInt aType ) const
    {
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikDynamicLayoutVariantSwitch )
        {
        CPbk2IconArray* iconArray = IconArray( *this );
        if ( iconArray )
            {
            // Refresh core Phonebook2 icons
            iconArray->RefreshL();
            CPbk2UIExtensionManager* extManager =
                CPbk2UIExtensionManager::InstanceL();
            extManager->PushL();
            // Refresh extension icons
            extManager->IconSupportL().RefreshL( *iconArray );
            CleanupStack::PopAndDestroy(); // extManager
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::Flags
// --------------------------------------------------------------------------
//
TUint CPbk2ContactInfoListBox::Flags() const
    {
    return iResourceData.iFlags;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::SetScrollEventObserver
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::SetScrollEventObserver(MEikScrollBarObserver* aObserver)
    {
    iScrollEventObserver = aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::ItemDrawer
// --------------------------------------------------------------------------
//
inline CPbk2ContactInfoListBox::CItemDrawer*
        CPbk2ContactInfoListBox::ItemDrawer() const
    {
    return static_cast<CPbk2ContactInfoListBox::CItemDrawer*>(
        CAknFormDoubleGraphicStyleListBox::ItemDrawer() );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CreateItemDrawerL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::CreateItemDrawerL()
    {
    CFormattedCellListBoxData* formattedData =
        CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( formattedData );
    iItemDrawer =  CPbk2ContactInfoListBox::CItemDrawer::NewL(
        Model(), iEikonEnv->NormalFont(), formattedData, iManager );
    CleanupStack::Pop( formattedData );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::HandlePointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    TInt oldIndex = CurrentItemIndex();

    // Base class handles the pointer event
    CAknFormDoubleGraphicStyleListBox::HandlePointerEventL(
        aPointerEvent );

    switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
            TInt pointedItemIndex;
            iFocusableItemPointed = View()->XYPosToItemIndex(
                aPointerEvent.iPosition, pointedItemIndex );
            iFocusedItemPointed = iFocusableItemPointed &&
                ( pointedItemIndex == oldIndex );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::HandleScrollEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::HandleScrollEventL( CEikScrollBar* aScrollBar, 
                                                  TEikScrollEvent aEventType )
    {
    CAknFormDoubleGraphicStyleListBox::HandleScrollEventL( aScrollBar, aEventType );
    if( iScrollEventObserver )
        {
        iScrollEventObserver->HandleScrollEventL( aScrollBar, aEventType );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::ClipFromBeginning
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoListBox::ClipFromBeginning
        ( TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber )
    {
    return AknTextUtils::ClipToFit
        ( aBuffer, AknTextUtils::EClipFromBeginning, this, aItemIndex,
          aSubCellNumber );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoListBox::FocusedItemPointed()
    {
    return iFocusedItemPointed;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoListBox::FocusableItemPointed()
    {
    return iFocusableItemPointed;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoListBox::SearchFieldPointed()
    {
    return EFalse; // no search field
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::AttributePresenceOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::AttributePresenceOperationComplete(
        MVPbkContactOperationBase& aOperation, 
        MVPbkStoreContactFieldCollection* aResult)
    {
    if (&aOperation == iVoiceTagFindOperation)
        {
        // Save voice tag results to field analyser
        iFieldAnalyzer->SetVoiceTagFields(aResult);
        TRAP_IGNORE(DoRefreshListBoxContentL(aResult));

        delete iVoiceTagFindOperation;
        iVoiceTagFindOperation = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::AttributePresenceOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::AttributePresenceOperationFailed(
        MVPbkContactOperationBase& aOperation, 
        TInt /*aError*/)
    {
    if (&aOperation == iVoiceTagFindOperation)
        {
        delete iVoiceTagFindOperation;
        iVoiceTagFindOperation = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::CreateListBoxModelL
// --------------------------------------------------------------------------
//
CPbk2FieldListBoxModel* CPbk2ContactInfoListBox::CreateListBoxModelL(
        const CVPbkContactManager& aContactManager,
        const CPbk2PresentationContactFieldCollection& aFieldCollection,
        MPbk2FieldPropertyArray& aFieldProperties,
        const CPbk2ContactFieldDynamicProperties* aDynamicProperties )
    {
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLTimeFormat);
    HBufC* timeFormat = iCoeEnv->AllocReadResourceLC( R_QTN_DATE_USUAL );
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLTimeFormat);

    PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLCreateListBoxModel);
    CPbk2FieldListBoxModel::TParams params(
        aFieldCollection, aContactManager, aFieldProperties, *timeFormat,
        *IconArray( *this ), iFieldAnalyzer, aDynamicProperties, EFalse);
    CPbk2FieldListBoxModel* model = CPbk2FieldListBoxModel::NewL( params );
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLCreateListBoxModel);
    
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLFormatFields);
    model->FormatFieldsL();
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLFormatFields);

    CleanupStack::PopAndDestroy();  // timeFormat
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLSetClipper);
    model->SetClipper( *this );
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxCreateListBoxModelLSetClipper);
    return model;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::DoCreateVoiceTagRetrieverL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::DoCreateVoiceTagRetrieverL(
        MVPbkStoreContact& aContact)
    {
    const MVPbkContactStoreProperties& properties = 
        iFieldCollection->ParentStoreContact().ParentStore().StoreProperties();

    if ( properties.SupportsVoiceTags() )
        {
        if (iVoiceTagFindOperation)
            {
            delete iVoiceTagFindOperation;
            iVoiceTagFindOperation = NULL;
            }
        // initiate a asynchronous voice tag retrieval
        iVoiceTagFindOperation = iManager.ContactAttributeManagerL().FindFieldsWithAttributeL(
              CVPbkVoiceTagAttribute::Uid(), aContact, *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoListBox::DoRefreshListBoxContentL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoListBox::DoRefreshListBoxContentL(
        MVPbkStoreContactFieldCollection* aResult)
    {
    // Format fields in the list box model and draw control
    static_cast<CPbk2FieldListBoxModel*>(iModel)->FormatFieldsL();
    // convert store contact field collection to presentation fields
    // from the presentation fields we can acquire the information which
    // list box rows need to be redrawn
    const TInt count = aResult->FieldCount();
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkStoreContactField& storeField = aResult->FieldAt(i);
        const TInt redrawIndex = iFieldCollection->FindFieldIndex(
                storeField);
        View()->DrawItem(redrawIndex);
        }
    }

//  End of File
