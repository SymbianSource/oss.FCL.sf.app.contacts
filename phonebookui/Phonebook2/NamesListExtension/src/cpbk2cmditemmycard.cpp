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
* Description:  .
*
*/

// Includes
#include "cpbk2cmditemmycard.h"

// Pbk2
#include "CPbk2AddFavoritesVisibility.h"
#include <Pbk2Commands.hrh>
#include <Pbk2ExNamesListRes.rsg>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include "cpbk2mycard.h"
#include "Pbk2NamesListEx.hrh"
#include <TPbk2IconId.h>
#include <Pbk2InternalUID.h>
#include <MVPbkContactLink.h>
#include <spbcontentprovider.h>
#include <CPbk2ApplicationServices.h>
#include <MVPbkStoreContact.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactNameFormatter2.h>
#include <featmgr.h>

// General
#include <StringLoader.h>

#include "pbk2contactuicontroldoublelistboxextension.h"

CPbk2CmdItemMyCard* CPbk2CmdItemMyCard::NewLC( 
        CPbk2MyCard& aMyCard,
		CSpbContentProvider& aStatusProvider )
    {
    CPbk2CmdItemMyCard* self = new (ELeave) CPbk2CmdItemMyCard( 
            aMyCard, aStatusProvider );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CPbk2CmdItemMyCard::ConstructL()
    {
    iCreateMyCardText = StringLoader::LoadL( R_QTN_CONTACTS_MC_SET_UP_MY_CARD );
    iOpenMyCardText = StringLoader::LoadL( R_QTN_CONTACTS_MC_MY_CARD );
    iMyCard.SetObserverL(*this);
    }

CPbk2CmdItemMyCard::CPbk2CmdItemMyCard( 
        CPbk2MyCard& aMyCard,
        CSpbContentProvider& aStatusProvider )
: iMyCard(aMyCard),
  iStatusProvider( aStatusProvider )
    {
    }

CPbk2CmdItemMyCard::~CPbk2CmdItemMyCard()
    {
    delete iCreateMyCardText;
    delete iOpenMyCardText;
    iMyCard.RemoveObserver(*this);    
    }

TPtrC CPbk2CmdItemMyCard::NameForUi() const
    {
    TPtrC ptr(iOpenMyCardText->Des());
    if (iMyCard.MyCardState() == CPbk2MyCard::ENonExisting)
        {
        ptr.Set(iCreateMyCardText->Des());
        }
    return ptr;
    }

TBool CPbk2CmdItemMyCard::IsEnabled() const
    {
    return iEnabled;
    }

void CPbk2CmdItemMyCard::SetEnabled( TBool aEnabled )
    {
    iEnabled = aEnabled;
    NotifyVisibiltyChange(aEnabled);    
    }

TInt CPbk2CmdItemMyCard::CommandId() const
    {
    return EPbk2CmdOpenMyCard;
    }

void CPbk2CmdItemMyCard::SetVisibilityObserver(
        MPbk2CmdItemVisibilityObserver* aObserver)
    {
    iObserver = aObserver;
    NotifyVisibiltyChange(IsEnabled());
    }


TAny* CPbk2CmdItemMyCard::ControlCmdItemExtension(
                TUid aExtensionUid )
    {
    if( aExtensionUid == TUid::Uid( KPbk2ControlCmdItemExtensionUID ) ) 
        {
        return static_cast<MPbk2DoubleListboxCmdItemExtension*>( this );
        }
    return NULL;
    }

void CPbk2CmdItemMyCard::MyCardEvent( MPbk2MyCardObserver::TMyCardStatusEvent /*aEvent*/ )
    {
    NotifyVisibiltyChange( IsEnabled() );
    }

void CPbk2CmdItemMyCard::FormatDataL(
                MPbk2DoubleListboxDataElement& aDataElement )
    {
    HBufC* txt = NULL;
    TPbk2IconId iconId;
    MVPbkContactLink* link = iMyCard.MyCardLink();
    CSpbContentProvider::TSpbContentType type = CSpbContentProvider::ETypeNone; 
    
    //if link exists
    if( link )
        {
        iStatusProvider.GetContentL( *link, txt, iconId, type );
        }
    
    if( link && type != CSpbContentProvider::ETypeSocialStatus )
        {
        MVPbkStoreContact* storeContact = iMyCard.MyCardStoreContact();
        
        if( storeContact )
            {                                                                                           
            CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceL();
            MPbk2ContactNameFormatter& nameFormatter = appServices->NameFormatter();
                 
            delete txt;
            txt = NULL;                      
            
            if( FeatureManager::FeatureSupported( KFeatureIdFfContactsCompanyNames ) )
                {
                MPbk2ContactNameFormatter2* nameformatterExtension =
                        reinterpret_cast<MPbk2ContactNameFormatter2*>( nameFormatter.
                        ContactNameFormatterExtension( MPbk2ContactNameFormatterExtension2Uid ) );
                            
                txt = nameformatterExtension->GetContactTitleWithCompanyNameL( storeContact->Fields(),
                        MPbk2ContactNameFormatter::EUseSeparator |
                        MPbk2ContactNameFormatter::EPreserveLeadingSpaces );        
                }
            else
                {
                txt = nameFormatter.GetContactTitleL( storeContact->Fields(),
                        MPbk2ContactNameFormatter::EUseSeparator |
                        MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
                }
                                                    
            Release( appServices );                           
            }
        }
    
    aDataElement.SetText( MPbk2DoubleListboxDataElement::EStatusText, txt, 
        MPbk2DoubleListboxDataElement::ETypeGenericText );    
    aDataElement.SetIconId( MPbk2DoubleListboxDataElement::EThumbnail,iMyCard.MyCardIconId() );
    }

const MVPbkContactLink* CPbk2CmdItemMyCard::Link()
	{
	return iMyCard.MyCardLink();
	}



void CPbk2CmdItemMyCard::NotifyVisibiltyChange( TBool aShow )
    {
    TBool change(EFalse);    
    TBool show = aShow && iMyCard.MyCardState() != CPbk2MyCard::EUnkown;    
    
    if( show )
        {
        if( iLastNotif!=EmcLastNotifVisible )
            {
            change = ETrue;
            }
        }
    else
        {
        //no op for EmcLastNotifHidden and EmcLastNotifNone 
        if( iLastNotif==EmcLastNotifVisible )  
            {
            change = ETrue;    
            }    
        }

    if( iObserver && change )
        {
        iObserver->CmdItemVisibilityChanged(CommandId(), show);
        iLastNotif = show ? EmcLastNotifVisible : EmcLastNotifHidden;        
        }
    }

// end of file
