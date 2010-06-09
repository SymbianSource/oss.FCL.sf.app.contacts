/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact view double list box model.
*
*/


// INCLUDE FILES
#include <featmgr/featmgr.h>
#include <MVPbkViewContact.h>

#include "MPbk2ClipListBoxText.h"
#include "cpbk2contactviewdoublelistboxmodel.h"
#include "CPbk2ThumbnailManager.h"
#include "Pbk2PresentationUtils.h"
#include "pbk2contactuicontroldoublelistboxextension.h"
#include "pbk2contactviewdoublelistboxdataelement.h"
#include "Pbk2Debug.h"

#include <MPbk2ContactNameFormatter2.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactUiControlExtension.h>
#include <CPbk2ContactIconsUtils.h>
#include <CPbk2IconArray.h>
#include <Pbk2InternalUID.h>

//Virtual phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactViewBase.h>

// CONSTANTS
namespace {
// Character used to replace invalid characters for UI
const TText KGraphicReplaceCharacter    = ' ';

// Character used to separate listbox columns
const TText KListColumnSeparator        = '\t';

// Index of contact name column
const TInt KNameColumn                  = 1;

// Index of secondary text column
const TInt KSecondaryTextColumn         = 2;

// Default formating used for contacts
const TInt KDefaultListFormatting =
        MPbk2ContactNameFormatter::EUseSeparator |
        MPbk2ContactNameFormatter::EPreserveLeadingSpaces;

// iBuffer max size is EMaxListBoxText = 256
// -> max length for name data is 100 and max lenght for status data is 100
// rest 56 are reserved for icon data
const TInt KMaxTxtLength = 100;

const TInt KDataElementCacheSize = 20;
}

/**
 * Cache for data elements, cache is made to avoid performance problems that 
 * caused in cases when avkon implementation calls multiple times MdcaPoint function with same index
 * Cached items must be deleted when there is any change in contact list.
 */
NONSHARABLE_CLASS( CDataElementCache ) : 
        public CBase,
        public MVPbkContactViewObserver,
        public MPbk2FilteredViewStackObserver
    {
public:
    static CDataElementCache* NewL(MPbk2FilteredViewStack& aView);
    ~CDataElementCache();

    void AppendL( CPbk2ContactViewDoubleListboxDataElement* aDataElement );
    CPbk2ContactViewDoubleListboxDataElement* Element( TInt aListIndex );
    
private:    // From MVPbkContactViewObserver
     void ContactViewReady(
             MVPbkContactViewBase& aView );
     void ContactViewUnavailable(
             MVPbkContactViewBase& aView );
     void ContactAddedToView(
             MVPbkContactViewBase& aView,
             TInt aIndex,
             const MVPbkContactLink& aContactLink );
     void ContactRemovedFromView(
             MVPbkContactViewBase& aView,
             TInt aIndex,
             const MVPbkContactLink& aContactLink );
     void ContactViewError(
             MVPbkContactViewBase& aView,
             TInt aError,
             TBool aErrorNotified );

private:    // From MPbk2FilteredViewStackObserver
      void TopViewChangedL(
              MVPbkContactViewBase& aOldView );
      void TopViewUpdatedL();
      void BaseViewChangedL();
      void ViewStackError(
              TInt aError );
      void ContactAddedToBaseView( 
              MVPbkContactViewBase& aBaseView,
              TInt aIndex,
              const MVPbkContactLink& aContactLink ); 
     
private:
    inline CDataElementCache( MPbk2FilteredViewStack& aView );
    inline void ConstructL();
    inline void Reset();

private:    
    RPointerArray<CPbk2ContactViewDoubleListboxDataElement> iCache;
    MPbk2FilteredViewStack& iView;
    };

// --------------------------------------------------------------------------
// CDataElementCache::NewL
// --------------------------------------------------------------------------
//
CDataElementCache* CDataElementCache::NewL( MPbk2FilteredViewStack& aView )
    {
    CDataElementCache* self = new (ELeave) CDataElementCache(aView);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CDataElementCache::CDataElementCache
// --------------------------------------------------------------------------
//
inline CDataElementCache::CDataElementCache( MPbk2FilteredViewStack& aView )
: iCache( KDataElementCacheSize ), 
  iView( aView )
    {
    }

// --------------------------------------------------------------------------
// CDataElementCache::ConstructL
// --------------------------------------------------------------------------
//
inline void CDataElementCache::ConstructL()
    {
    iView.AddObserverL(*this);
    iView.AddStackObserverL(*this);
    }

// --------------------------------------------------------------------------
// CDataElementCache::~CDataElementCache
// --------------------------------------------------------------------------
//
CDataElementCache::~CDataElementCache()
    {
    Reset();
    iView.RemoveObserver(*this);
    iView.RemoveStackObserver(*this);
    }

// --------------------------------------------------------------------------
// CDataElementCache::AppendL
// --------------------------------------------------------------------------
//
void CDataElementCache::AppendL( 
        CPbk2ContactViewDoubleListboxDataElement* aDataElement )
    {
    iCache.InsertL( aDataElement, 0 );
    const TInt cacheItemCount( iCache.Count() );
    if( cacheItemCount > KDataElementCacheSize )
        {
        delete iCache[cacheItemCount - 1];
        iCache.Remove(cacheItemCount - 1);
        }
    }

// --------------------------------------------------------------------------
// CDataElementCache::Reset
// --------------------------------------------------------------------------
//
inline void CDataElementCache::Reset()
    {
    iCache.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CDataElementCache::Element
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListboxDataElement* 
CDataElementCache::Element( TInt aListIndex )
    {
    const TInt cacheItemCount( iCache.Count() );
    for( TInt i = 0 ; i < cacheItemCount ; ++i )
        {
        CPbk2ContactViewDoubleListboxDataElement* p = iCache[i];
        if( p->ListIndex() == aListIndex )
            {
            return p;
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CDataElementCache::ContactViewReady
// --------------------------------------------------------------------------
//
void CDataElementCache::ContactViewReady(
        MVPbkContactViewBase& /*aView*/ )
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CDataElementCache::ContactViewUnavailable(
        MVPbkContactViewBase& /*aView*/ )
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::ContactAddedToView
// --------------------------------------------------------------------------
//
void CDataElementCache::ContactAddedToView(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CDataElementCache::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::ContactViewError
// --------------------------------------------------------------------------
//
void CDataElementCache::ContactViewError(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aError*/,
        TBool /*aErrorNotified*/ )
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::TopViewChangedL
// --------------------------------------------------------------------------
//
void CDataElementCache::TopViewChangedL(
        MVPbkContactViewBase& /*aOldView*/ )
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::TopViewUpdatedL
// --------------------------------------------------------------------------
//
void CDataElementCache::TopViewUpdatedL()
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::BaseViewChangedL
// --------------------------------------------------------------------------
//
void CDataElementCache::BaseViewChangedL()
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::ViewStackError
// --------------------------------------------------------------------------
//
void CDataElementCache::ViewStackError(
        TInt /*aError*/ )
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CDataElementCache::ContactAddedToBaseView
// --------------------------------------------------------------------------
//
void CDataElementCache::ContactAddedToBaseView( 
        MVPbkContactViewBase& /*aBaseView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ ) 
    {
    Reset();
    }


// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::CPbk2ContactViewDoubleListBoxModel
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBoxModel::CPbk2ContactViewDoubleListBoxModel(
    CPbk2ContactViewListBoxModel::TParams& aParams,
    CPbk2ThumbnailManager& aThumbManager,
    MPbk2FilteredViewStack& aFilteredViewStack ) :
    CPbk2ContactViewListBoxModel( aParams ),
    iThumbManager( aThumbManager ),
    iFilteredViewStack( aFilteredViewStack )
    {
    TAny* object = aParams.iUiExtension->ContactUiControlExtensionExtension
            ( TUid::Uid( KPbk2ContactUiControlExtensionExtensionUID ) );
    iDoubleListExtensionPoint =
            static_cast<MPbk2ContactUiControlDoubleListboxExtension*>( object );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::~CPbk2ContactViewDoubleListBoxModel
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBoxModel::~CPbk2ContactViewDoubleListBoxModel()
    {
    delete iDataElementCache;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBoxModel* CPbk2ContactViewDoubleListBoxModel::NewL(
    CPbk2ContactViewListBoxModel::TParams& aParams,
    CPbk2ThumbnailManager& aThumbManager,
    MPbk2FilteredViewStack& aFilteredViewStack )
    {
    CPbk2ContactViewDoubleListBoxModel* self =
        new ( ELeave ) CPbk2ContactViewDoubleListBoxModel( 
                aParams, aThumbManager, aFilteredViewStack );

    CleanupStack::PushL(self);
    self->ConstructL( aParams.iStoreProperties, aParams.iUiExtension );
    self->iDataElementCache = CDataElementCache::NewL( aFilteredViewStack );
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::FormatBufferL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::FormatBufferL( const TInt aIndex ) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewDoubleListBoxModel::FormatBufferL(0x%x,%d), begin"),
        this);
    CPbk2ContactViewDoubleListboxDataElement* element = 
            iDataElementCache->Element( aIndex );
    if( element )
        {
        FormatBufferForElementDataL(*element, aIndex);
        }
    else
        {
        const MVPbkViewContact& contact = iView->ContactAtL(aIndex);
        FormatBufferForContactL(contact, aIndex );
        }
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewDoubleListBoxModel::FormatBufferL(0x%x,%d), end"),
        this);
    }


// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::FormatBufferForElementDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::FetchDataFromExtension(
        CPbk2ContactViewDoubleListboxDataElement& aDataElement,
        TInt aIndex ) const
    {
    // Get element data from extension
    // 
    if( iDoubleListExtensionPoint )
        {
        TRAPD( err, 
            iDoubleListExtensionPoint->FormatDataL( 
                    *(aDataElement.ContactLink()), aDataElement ) );
        if( err )
            {
            // extension's errors are ignored.  
            PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
            "CPbk2ContactViewDoubleListBoxModel::FormatBufferForElementDataL format error %d"),
                err );
            }
        }

    // Clip secondary text if it's a phone number 
    if( MPbk2DoubleListboxDataElement::ETypePhoneNumber == 
        aDataElement.TextType( MPbk2DoubleListboxDataElement::EStatusText ) && 
        iClipListBoxText )
        {
        TPtr secondary( aDataElement.TextPtr( 
            MPbk2DoubleListboxDataElement::EStatusText ) );
        iClipListBoxText->ClipFromBeginning( 
            secondary, aIndex, KSecondaryTextColumn );
        }    
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::FormatBufferForElementDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::FormatBufferForElementDataL(
    CPbk2ContactViewDoubleListboxDataElement& aDataElement, 
    TInt aIndex ) const
    {
    FetchDataFromExtension(aDataElement, aIndex);

    // start fill the new buffer for the avkon list
    iBuffer.Zero();

    AppendThumbnailL( aDataElement, aIndex );
    
    // The cached element needs to be updated when the presence icon is changed
    RArray<TPbk2IconId> ids;
    CleanupClosePushL( ids );    
    const MVPbkViewContact& contact = iView->ContactAtL( aIndex );
    iContactIcons->GetIconIdsForContactL( contact, ids );

    // If no icon was found, append an empty icon
    if( ids.Count() == 0 )
        {
        ids.Append( iEmptyIconId );
        }
    
    aDataElement.SetIconId( MPbk2DoubleListboxDataElement::EMainIcon, ids[ 0 ] );
    CleanupStack::PopAndDestroy( &ids );
    
    // Format line buffer based on element's content
    FormatBufferFromElement( aDataElement );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::FormatBufferForContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::FormatBufferForContactL(
    const MVPbkViewContact& aViewContact, 
    TInt aIndex ) const
    {
    MVPbkContactLink* link = aViewContact.CreateLinkLC();
    CPbk2ContactViewDoubleListboxDataElement* element = 
        CPbk2ContactViewDoubleListboxDataElement::NewL( link, aIndex );
    if( link )
        {
        CleanupStack::Pop();    // link
        }
    CleanupStack::PushL( element );

    // get data for element
    // get user name
    HBufC* name = NULL;
    if( FeatureManager::FeatureSupported( KFeatureIdFfContactsCompanyNames ))
        {
        MPbk2ContactNameFormatter2* nameformatterExtension =
            reinterpret_cast<MPbk2ContactNameFormatter2*>(iNameFormatter.
            ContactNameFormatterExtension( MPbk2ContactNameFormatterExtension2Uid ) );
        name = nameformatterExtension->GetContactTitleWithCompanyNameL(
            aViewContact.Fields(), KDefaultListFormatting );
        }
    else
        {
        name = iNameFormatter.GetContactTitleL(
            aViewContact.Fields(), KDefaultListFormatting );
        }
    CleanupStack::PushL( name );
    TPtr namePtr( name->Des() );

    // Replace characters that can not be displayed correctly
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters(
        namePtr, KGraphicReplaceCharacter );
    
    if( CutFromBeginningFieldL( aViewContact.Fields() ) && iClipListBoxText )
        {
        iClipListBoxText->ClipFromBeginning( namePtr, aIndex, KNameColumn );
        }

    // takes ownership of 'name'
    element->SetText( MPbk2DoubleListboxDataElement::EName, name, 
        MPbk2DoubleListboxDataElement::ETypeGenericText );
    CleanupStack::Pop( name );


    // get icons
    RArray<TPbk2IconId> ids;
    CleanupClosePushL( ids );
    iContactIcons->GetIconIdsForContactL( aViewContact, ids );

    // ids should contain at least one empty icon in any case
    if( ids.Count() == 0 )
        {
        ids.Append( iEmptyIconId );
        }
    
    element->SetIconId( MPbk2DoubleListboxDataElement::EMainIcon, ids[0] );
    CleanupStack::PopAndDestroy( &ids );

    // Get element data from extension
    FetchDataFromExtension( *element, aIndex );

    // start format data for the avkon list
    iBuffer.Zero();
    
    AppendThumbnailL( *element, aIndex );
    
    // Format line buffer based on element's content
    FormatBufferFromElement( *element );
    
    iDataElementCache->AppendL( element );
    CleanupStack::Pop( element ); 
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::FormatBufferFromElement
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::FormatBufferFromElement(
    const CPbk2ContactViewDoubleListboxDataElement& aElement ) const
    {

    // List model format:
    //   [thumbnail icon] \t [contact name] \t [secondary text] \t
    //   [trailing icon]

    // (2) Contact name
    AppendName( aElement.TextPtr( 
            MPbk2DoubleListboxDataElement::EName ).Left( KMaxTxtLength ) );
    iBuffer.Append( KListColumnSeparator );
    
    // (3) Secondary text
    TPtr status( aElement.TextPtr( MPbk2DoubleListboxDataElement::EStatusText ) );
    AppendText( status );
    
    // (4) Trailing icon
    AppendIconIndexIfFound( 
        aElement.IconId( MPbk2DoubleListboxDataElement::EMainIcon ) );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::AppendThumbnailL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::AppendThumbnailL( 
        CPbk2ContactViewDoubleListboxDataElement& aDataElement, 
        TInt aIndex ) const
    {
    // (1) Add thumbnail icon
    //
    TInt index = iThumbManager.GetPbkIconIndexL( 
            aIndex, *(aDataElement.ContactLink()) );
    if( index != KErrNotFound )
        {
        iBuffer.AppendNum( index );
        }
    iBuffer.Append( KListColumnSeparator );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::AppendText
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::AppendText( TDes& aText ) const
    {
	//Convert numbers to some languages specific version. E.g.: Arabic.
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aText );
    
    // replace non-allowed characters with ' '
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters(
        aText, KGraphicReplaceCharacter );
    AknTextUtils::ReplaceCharacters(
        aText, KAknReplaceListControlChars, KGraphicReplaceCharacter );
    
    iBuffer.Append( aText.Left( KMaxTxtLength ) );
    iBuffer.Append( KListColumnSeparator );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::AppendIconIndexIfFound
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::AppendIconIndexIfFound(  const TPbk2IconId& aIconId ) const
	{
	TInt iconIndex = iIconArray->FindIcon( aIconId );
	// if icon is found
	if( iconIndex != KErrNotFound )
		{
		iBuffer.AppendNum( iconIndex );
		iBuffer.Append( KListColumnSeparator );
		}
	}


//  End of File
