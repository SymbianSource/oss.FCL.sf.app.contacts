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

// CONSTANTS

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

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::CPbk2ContactViewDoubleListBoxModel
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBoxModel::CPbk2ContactViewDoubleListBoxModel(
    CPbk2ContactViewListBoxModel::TParams& aParams,
    CPbk2ThumbnailManager& aThumbManager ) :
    CPbk2ContactViewListBoxModel( aParams ),
    iThumbManager( aThumbManager )
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
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBoxModel* CPbk2ContactViewDoubleListBoxModel::NewL(
    CPbk2ContactViewListBoxModel::TParams& aParams,
    CPbk2ThumbnailManager& aThumbManager )
    {
    CPbk2ContactViewDoubleListBoxModel* self =
        new ( ELeave ) CPbk2ContactViewDoubleListBoxModel( aParams, aThumbManager );

    CleanupStack::PushL(self);
    self->ConstructL( aParams.iStoreProperties, aParams.iUiExtension );
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::FormatBufferForContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::FormatBufferForContactL(
    const MVPbkViewContact& aViewContact, TInt aIndex ) const
    {
    // Reset buffer
    iBuffer.Zero();
    
    CPbk2ContactViewDoubleListboxDataElement* element = 
        CPbk2ContactViewDoubleListboxDataElement::NewLC();

    // Fill data element cells
    
    // (1) Add thumbnail icon
    //
    // TODO: to be fixed
    //const TPbk2IconId& thumb = iThumbManager.GetPbkIconIndex( aIndex, aViewContact );
    //element->SetIconId( MPbk2DoubleListboxDataElement::EThumbnail, thumb );
    // -- temporary sollution:
    TInt index = iThumbManager.GetPbkIconIndexL( aIndex, aViewContact );
    if( index != KErrNotFound )
        {
        iBuffer.AppendNum( index );
        }
    iBuffer.Append( KListColumnSeparator );
    // --

    // (2) Add contact name
    //
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

    // (3, 4) Secondary text and icon
    //
    // -> These are empty by default
  

    // (5) Trailing icon
    //
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
    if( iDoubleListExtensionPoint )
        {
        TRAPD( err, 
            iDoubleListExtensionPoint->FormatDataL( aViewContact, *element ) );
        if( err )
            {
            // extension's errors are ignored.  
            PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
            "CPbk2ContactViewDoubleListBoxModel::FormatBufferForContactL format error %d"),
                err );
            }
        }

    // Clip secondary text if it's a phone number 
    if( MPbk2DoubleListboxDataElement::ETypePhoneNumber == 
        element->TextType( MPbk2DoubleListboxDataElement::EStatusText ) && iClipListBoxText )
        {
        TPtr secondary( element->TextPtr( 
            MPbk2DoubleListboxDataElement::EStatusText ) );
        iClipListBoxText->ClipFromBeginning( 
            secondary, aIndex, KSecondaryTextColumn );
        }
    
    // Format line buffer based on element's content
    FormatBufferFromElement( *element );
    
    CleanupStack::PopAndDestroy( element ); 
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
    //   [secondary icon] \t [trailing icon]

    // (1) Thumbnail icon
    // TODO: to be fixed
    //AppendIconIndex( 
    //    aElement.IconId( MPbk2DoubleListboxDataElement::EThumbnail ) );

    // (2) Contact name
    AppendName( aElement.TextPtr( MPbk2DoubleListboxDataElement::EName ) );
    iBuffer.Append( KListColumnSeparator );
    
    // (3) Secondary text
    TPtr status( aElement.TextPtr( MPbk2DoubleListboxDataElement::EStatusText ) );
    AppendText( status );
       
#if 0	// Service icon is removed for now.
    // (4) Secondary icon
    AppendIconIndexIfFound( 
        aElement.IconId( MPbk2DoubleListboxDataElement::EServiceIcon ) );
#endif
    
    // (5) Trailing icon
    AppendIconIndexIfFound( 
        aElement.IconId( MPbk2DoubleListboxDataElement::EMainIcon ) );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::AppendText
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBoxModel::AppendText( TDes& aText ) const
    {
    // replace non-allowed characters with ' '
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters(
        aText, KGraphicReplaceCharacter );
    AknTextUtils::ReplaceCharacters(
        aText, KAknReplaceListControlChars, KGraphicReplaceCharacter );
    
    iBuffer.Append( aText );
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
