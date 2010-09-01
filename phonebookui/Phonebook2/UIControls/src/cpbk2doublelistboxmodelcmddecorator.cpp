/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Model decorator for double listbox (Social phonebook)
*
*/
// class header
#include "cpbk2doublelistboxmodelcmddecorator.h"


// includes
#include <MPbk2UiControlCmdItem.h>
#include "pbk2contactviewdoublelistboxdataelement.h"
#include <pbk2doublelistboxcmditemextension.h>
#include <CPbk2IconArray.h>
#include <TPbk2IconId.h>
#include <Pbk2PresentationUtils.h>
#include <AknUtils.h>
#include <Pbk2InternalUID.h>

const TText KSeparator = '\t';
// Character used to replace invalid characters for UI
const TText KGraphicReplaceCharacter    = ' ';

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::NewL
// --------------------------------------------------------------------------
//
CPbk2DoubleListboxModelCmdDecorator* CPbk2DoubleListboxModelCmdDecorator::NewL(
		const CPbk2IconArray& aIconArray,
		TPbk2IconId aEmptyIconId,
		TPbk2IconId aDefaultIconId )
	{
	CPbk2DoubleListboxModelCmdDecorator* self = 
				new(ELeave)CPbk2DoubleListboxModelCmdDecorator( 
				        aIconArray, aEmptyIconId, aDefaultIconId);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
	
// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::CPbk2DoubleListboxModelCmdDecorator
// --------------------------------------------------------------------------
//	
CPbk2DoubleListboxModelCmdDecorator::CPbk2DoubleListboxModelCmdDecorator( 
		const CPbk2IconArray& aIconArray,
		TPbk2IconId aEmptyIconId,
		TPbk2IconId aDefaultIconId )
: CPbk2ListboxModelCmdDecorator( aIconArray, aEmptyIconId, aDefaultIconId )
	{
	}

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::~CPbk2DoubleListboxModelCmdDecorator
// --------------------------------------------------------------------------
//
CPbk2DoubleListboxModelCmdDecorator::~CPbk2DoubleListboxModelCmdDecorator()
	{
	delete iElement;
	}

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::ConstructL
// --------------------------------------------------------------------------
//  
void CPbk2DoubleListboxModelCmdDecorator::ConstructL()
    {
    CPbk2ListboxModelCmdDecorator::ConstructL();
    iElement = CPbk2ContactViewDoubleListboxDataElement::NewLC();
    CleanupStack::Pop( iElement );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBoxModel::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC CPbk2DoubleListboxModelCmdDecorator::MdcaPoint( TInt aIndex ) const
	{
	TPtrC result;
	const TInt enabledCommandCount = NumberOfEnabledCommands();
	if ( aIndex < enabledCommandCount )
		{
		// get correct command item
		MPbk2UiControlCmdItem& item = EnabledCommand( aIndex );
		
		// empty icon
		TInt iconIndex = iIconArray.FindIcon( iEmptyIconId );
		
		// get command name
		TPtrC name( item.NameForUi() );
		
		//check if we have a mycard command item
		TAny* object = item.ControlCmdItemExtension( 
		        TUid::Uid( KPbk2ControlCmdItemExtensionUID ) );
		if(  object )
			{
			MPbk2DoubleListboxCmdItemExtension* extension = 
					static_cast<MPbk2DoubleListboxCmdItemExtension*>( object );
			// if extension exists
			if( extension )
				{
				// List model format:
				//   [thumbnail icon] \t [contact name] \t [secondary text] \t
				//   [trailing icon]
				
				// fetch status text
				iElement->SetText( 
				        MPbk2DoubleListboxDataElement::EStatusText, 
				        NULL, 
				        MPbk2DoubleListboxDataElement::ETypeGenericText );
                TRAPD( err, extension->FormatDataL( *iElement ) );
				if( !err )
				    {
					// get icon index
					iconIndex = iIconArray.FindIcon( iElement->IconId( 
					        MPbk2DoubleListboxDataElement::EThumbnail ) );
					// if icon is not found, use default
					if( iconIndex < 0 )
						{
						iconIndex = iIconArray.FindIcon( iDefaultIconId );
						}
					iBuffer.Num( iconIndex );
					iBuffer.Append( KSeparator );
					iBuffer.Append( name );
				
					// status
                    TPtr status( iElement->TextPtr( 
                            MPbk2DoubleListboxDataElement::EStatusText ) );
                    //if status text exists
                    if( status.Compare( KNullDesC ) != 0 )
                        {
                        // status text
                        iBuffer.Append( KSeparator );
                        AppendText( status );
                        }
				    }
				}
			}
		// format normally
		else
			{
			iBuffer.Num( iconIndex );
			iBuffer.Append( KSeparator );
			iBuffer.Append( name );
			}
		result.Set( iBuffer );
		}
	else
		{
		// It is a contact that is to be shown.
		result.Set(
			iDecoratedModel->MdcaPoint( aIndex - enabledCommandCount) );
		}
	
	return result;
	}

// --------------------------------------------------------------------------
// CPbk2DoubleListboxModelCmdDecorator::AppendText
// --------------------------------------------------------------------------
//
void CPbk2DoubleListboxModelCmdDecorator::AppendText( TDes& aText ) const
    {
    // replace non-allowed characters with ' '
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters(
        aText, KGraphicReplaceCharacter );
    AknTextUtils::ReplaceCharacters(
        aText, KAknReplaceListControlChars, KGraphicReplaceCharacter );

    // iBuffer max size is EMaxListBoxText = 256
    // don't allow set too long status txt to iBuffer
    const TInt KMaxTxtLength = 100;
    iBuffer.Append( aText.Left( KMaxTxtLength ) );
    }
