/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for handlind the launchpad related data
*
*/


#include "ccappcommlauncherheaders.h"
#include <phonebook2ece.mbg>

#include <CPbk2ApplicationServices.h>
#include <CPbk2ServiceManager.h>

#include <aknlayoutscalable_avkon.cdl.h>
//SpSettings
#include <spsettings.h>
#include <spentry.h>
#include <mnproviderfinder.h>
#include <StringLoader.h>

#include <spproperty.h>
#include <spnotifychange.h>

//Bitmap
#include <bitdev.h> 
#include <cbsbitmap.h>
#include <AknIconUtils.h>


namespace {

#define KOneVOIPServiceAvailable    1
#define KVOIPButtonImageSet         0x1
#define KVOIPButtonTextSet          0x2
const TText KCCASpaceChar = ' ';

/**
 * Compares entry service name to scheme.
 *
 * @param aEntry entry to be analyzed
 * @param aScheme to be compared with
 */
TBool CompareService(CSPEntry& aEntry, const TDesC& aScheme)
    {
    const TDesC& name = aEntry.GetServiceName();
    if (name.CompareF(aScheme) == KErrNone)
        {
        return ETrue;
        }
    return EFalse;
    }

TPtrC ParseService(const TDesC& aData)
    {
    TPtrC result = KNullDesC();
    TInt index = aData.Locate(':');
    if (index > 0)
        {
        result.Set(aData.Left(index));
        }
    return result;
    }

/**
 * Returns index of a first IM field.
 */
TInt SelectIMIndexL(const CCmsContactField& aContactField)
    {
    TInt result = 0; // by default returns 0
    CSPSettings* settings = CSPSettings::NewLC();
    RIdArray idArray;
    TBool found = EFalse;
    CleanupClosePushL(idArray);

    TInt error = settings->FindServiceIdsL(idArray);
    if (error == KErrNone)
        {
        for (TInt i = 0; !found && i < idArray.Count(); ++i)
            {
            CSPEntry* entry = CSPEntry::NewLC();
            settings->FindEntryL(idArray[i], *entry);

            const CSPProperty* prop = NULL;
            if (entry->GetProperty(prop, ESubPropertyIMLaunchUid) == KErrNone)
                {
                for (TInt i = 0; i < aContactField.ItemCount(); ++i)
                    {
                    const CCmsContactFieldItem& item = aContactField.ItemL(i);
                    TPtrC data = item.Data();
                    TPtrC scheme = ParseService(data);
                    if (CompareService(*entry, scheme))
                        {
                        result = i;
                        found = ETrue;
                        break;
                        }
                    }
                }
            CleanupStack::PopAndDestroy(); // entry
            }
        }

    CleanupStack::PopAndDestroy(2); // idArray, settings
    return result;
    }
/**
 * Clones the Bitmap
 * This is better than Duplicating the bitmap
 */
CFbsBitmap* CloneBitmapLC(TSize aSize, CFbsBitmap* aBitmap)
    {
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    bitmap->Create( aSize, aBitmap->DisplayMode() );
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( bitmap );
    CleanupStack::PushL( bitmapDevice );
    CFbsBitGc* graphicsContext = NULL; 
    User::LeaveIfError( bitmapDevice->CreateContext( graphicsContext ) ); 
    CleanupStack::PushL( graphicsContext );
    graphicsContext->BitBlt( TPoint(0, 0), aBitmap );            
    CleanupStack::PopAndDestroy( 2 );//graphicsContext,bitmapDevice
    return bitmap;
    }
           

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::CCCAppCommLauncherLPadModel
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherLPadModel::CCCAppCommLauncherLPadModel(
    CCCAppCommLauncherContainer& aContainer, 
    CEikListBox& aListBox,
    CCCAppCommLauncherPlugin& aPlugin )
    :
    iContainer ( aContainer ),
    iPerfLauncherCalled(EFalse),
    iCoeEnv(*CCoeEnv::Static()),
    iListBox(aListBox),
    iPlugin(aPlugin)
    {
    CCA_DP( KCommLauncherLogFile, CCA_L("CCCAppCommLauncherLPadModel::CCCAppCommLauncherLPadModel"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::~CCCAppCommLauncherLPadModel
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherLPadModel::~CCCAppCommLauncherLPadModel()
    {
    CCA_DP( KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherLPadModel::~CCCAppCommLauncherLPadModel"));
    
    if(iSPNotifyChange)
        {
        iSPNotifyChange->NotifyChangeCancel();
        }
    
    delete iSPNotifyChange;    
    delete iSettings;
        
    delete iPbkCmd;
    
    iButtonDataArray.Reset();
    delete iTempText;
    iAddressFields.Close();
    
    
    delete iTextBuf;
    
    CCA_DP( KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherLPadModel::~CCCAppCommLauncherLPadModel"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::NewL
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherLPadModel* CCCAppCommLauncherLPadModel::NewL(
    CCCAppCommLauncherContainer& aContainer, CEikListBox& aListBox, CCCAppCommLauncherPlugin& aPlugin )
    {
    CCA_DP( KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherLPadModel::NewL"));
    CCCAppCommLauncherLPadModel* self =
        new( ELeave ) CCCAppCommLauncherLPadModel( aContainer, aListBox, aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CCA_DP( KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherLPadModel::NewL"));
    return self;
	}

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::ConstructL()
    {
    CCA_DP( KCommLauncherLogFile, CCA_L("CCCAppCommLauncherLPadModel::ConstructL"));
    iTempText = HBufC::NewL( KCCAppCommLauncherMaxButtonDataTextLength );
    iTextBuf = HBufC::NewL( KCCAppCommLauncherMaxButtonDataTextLength );
    iAddressFields.Close();
    iButtonIconArray = new (ELeave) CAknIconArray( 2 );
    iAddressesValidated = EFalse;
    iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin );
    iSettings = CSPSettings::NewL();    
    iSPNotifyChange = CSPNotifyChange::NewL(*this);
    RIdArray idArray;
    CleanupClosePushL(idArray);    
    User::LeaveIfError( iSettings->FindServiceIdsL(idArray) );    
    //Listen for any changes to these settings
    iSPNotifyChange->NotifyChangeL( idArray );    
    CleanupStack::PopAndDestroy(); //idArray 
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherLPadModel::MdcaCount() const
    {
    return iButtonDataArray.Count();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC CCCAppCommLauncherLPadModel::MdcaPoint( TInt aIndex ) const
    {
    TPtr tempText = iTempText->Des();
    tempText.Zero();

    if ( iButtonDataArray.Count() > aIndex )
        {
        TPtr textPtr(iTextBuf->Des());
        textPtr.Zero();
        textPtr.Copy(iButtonDataArray[ aIndex ].iPopupText);
        if ( iButtonDataArray[ aIndex ].iClipFromBegining )
        	{
        	// Clip for second row text
        	ClipFromBeginning( textPtr, aIndex, 2 );
        	}
        
        tempText.AppendNum( MapCommMethodToIcon(
            iButtonDataArray[ aIndex ].iContactAction ));
        tempText.Append( KColumnListSeparator );
        tempText.Append( iButtonDataArray[ aIndex ].iText );      
        tempText.Append( KColumnListSeparator );  
        tempText.Append( textPtr );
        tempText.Append( KColumnListSeparator ); 
        
        // TODO: Check presence icon
        tempText.Append( KColumnListSeparator ); 
        
        // Check if show multi icon at the right end of second row
        if ( IfShowMultiIcon( aIndex ) )
        	{
        	tempText.AppendNum( EMultiIconIndex );
        	tempText.Append( KColumnListSeparator );  
        	}  
        }

    return tempText;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::MapCommMethodToIcon
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherLPadModel::MapCommMethodToIcon(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction ) const
    {
    TInt iconIndex = KErrNotFound;

    switch( aContactAction )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
           iconIndex = ECallIconIndex;
           break;
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
           iconIndex = EMsgIconIndex;
           break;
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
           iconIndex = EEmailIconIndex;
           break;
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
           iconIndex = EVoipIconIndex;
           break;
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
           iconIndex = EInstMsgIconIndex;
           break;
        case VPbkFieldTypeSelectorFactory::EURLSelector:
           iconIndex = EUrlIconIndex;
           break;
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
           iconIndex = EVideocallIconIndex;
           break;
        case VPbkFieldTypeSelectorFactory::EFindOnMapSelector:        
           if ( iAddressesValidated )
               {
               iconIndex = EAddressValIconIndex;
               }
           else
               {
               iconIndex = EAddressNotValIconIndex;
               }
           break;
        default:
        break;
        }

    return iconIndex;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::FillButtonArrayL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::FillButtonArrayL()
    {
    RArray<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>&
        preferredCommMethods = iContainer.Plugin().PreferredCommMethods();//not owned
    const TInt buttonCount = preferredCommMethods.Count();

    for ( TInt i = 0; i < buttonCount; i++ )
        {
        const TInt numberOfAddresses =
            iContainer.Plugin().ContactHandler().AddressAmount(
                preferredCommMethods[i] );

        if ( numberOfAddresses )
            {
            TBool isServiceAvailable =
                iContainer.Plugin().ContactHandler().IsServiceAvailable(
                    preferredCommMethods[i] );
            if (isServiceAvailable)
                {
                if ( preferredCommMethods[i] ==
                        VPbkFieldTypeSelectorFactory::EFindOnMapSelector )
                	{

                	AddressButtonL( numberOfAddresses, i );

                	}
                else
                	{
                    TCommLauncherButtonData buttonData = 
                        TCommLauncherButtonData( preferredCommMethods[i] );
                    ButtonTextL(preferredCommMethods[i], buttonData.iText);
                    buttonData.iNumberOfAddresses = numberOfAddresses;
                    iButtonDataArray.AppendL( buttonData );
                	}
                }
            }
        }    
       
    LoadIconArrayL();                
    
    //Load Specialised Voip Icons if only one voip service is available
    //CCA spec will be updated with this necessary change
    
    //Usecase : If we have only one voip service, the voip(Internet Call)
    //button should have the Branded Icon of that Service and the label
    //must be "ServiceName" appended with "Call". 
    //eg : If we have a service named SKYPE installed in the Phone
    //and if SKYPE supports VOIP, then the VOIP Button Icon should be
    //the Branded Icon of SKYPE and the Button Label should be 
    //"SKYPE CALL". 
    //If we have more than one voip service, then the VOIP button should
    //show the default voip button EMbmPhonebook2eceQgn_prop_pb_comm_voip 
    //(Globe with Phone) and the Button label should be r_qtn_cca_voip_call 
    //as defined in the rss    
    LoadVoipButtonInfoL();

    // Update CBA with MSK "Select" if there are communication methods available.
    TBool communicationMethodsAvailable = ( iButtonDataArray.Count() > 0 );
    iContainer.Plugin().UpdateMSKinCbaL( communicationMethodsAvailable );

    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::ButtonTextL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::ButtonTextL(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction,
    TDes& aText)
    {
    TInt resId = KErrNotFound;
    switch( aContactAction )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
            resId = R_QTN_CCA_CALL;
            break;
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            resId = R_QTN_CCA_MESSAGE;
            break;
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
            resId = R_QTN_CCA_EMAIL;
            break;
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            resId = R_QTN_CCA_CHAT;
            break;
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            resId = R_QTN_CCA_VOIP_CALL;
            break;
        case VPbkFieldTypeSelectorFactory::EURLSelector:
            resId = R_QTN_CCA_URL;
            break;
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
            resId = R_QTN_CCA_VIDEO_CALL;
            break;
        case VPbkFieldTypeSelectorFactory::EFindOnMapSelector:        
        	if ( iAddressesValidated )
        		{
        	    resId = R_QTN_PHOB_COMLAUNCHER_SHOW_ON_MAP;
        		}
        	else
        		{
                resId = R_QTN_PHOB_COMLAUNCHER_FIND_ON_MAP;
        		}
            break;
        default:
            break;
        }

    if ( KErrNotFound != resId )
        {
        aText.Copy( StringLoader::LoadLC( resId, &iCoeEnv )
            ->Left( KCCAppCommLauncherMaxButtonDataTextLength ));
        CleanupStack::PopAndDestroy();
        }
    }

// -----------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::ContactFieldFetchedNotifyL()
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::ContactFieldFetchedNotifyL(
    const CCmsContactField& aContactField )
    {
    const TInt count = iButtonDataArray.Count();
    CCmsContactFieldItem::TCmsContactField dataType = aContactField.Type();
    CCCAppCommLauncherContactHandler& contactHandler =
        iContainer.Plugin().ContactHandler();//not owned

    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EVoiceCallSelector ))
        {// voice call
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL(
                i, VPbkFieldTypeSelectorFactory::EVoiceCallSelector, aContactField );
            }

        //PERFORMANCE LOGGING: 11. Phonenumber data received & consumed
        WriteToPerfLog();

        RunLaunchLogger();

        }
    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EUniEditorSelector ))
        {// unieditor
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL(
                i, VPbkFieldTypeSelectorFactory::EUniEditorSelector, aContactField );
            }
        }
    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EEmailEditorSelector ))
        {// email
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL(
                i, VPbkFieldTypeSelectorFactory::EEmailEditorSelector, aContactField );
            }
        }
    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EVOIPCallSelector ))
        {// voip
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL(
                i, VPbkFieldTypeSelectorFactory::EVOIPCallSelector, aContactField );
            }
        }
    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EInstantMessagingSelector ))
        {// im
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL(
                i, VPbkFieldTypeSelectorFactory::EInstantMessagingSelector, aContactField );
            }
        }
    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EURLSelector ))
        {// url
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL(
                i, VPbkFieldTypeSelectorFactory::EURLSelector, aContactField );
            }
        }
    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EFindOnMapSelector ))
        {// address
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL( i,
            		VPbkFieldTypeSelectorFactory::EFindOnMapSelector,
            		aContactField );
            }
        }
    if ( contactHandler.ContactFieldTypeAndContactActionMatch( dataType,
            VPbkFieldTypeSelectorFactory::EVideoCallSelector ))
            {// video call
        for ( TInt i = 0; i < count; i++ )
            {
            CheckPopupTextL(
                i, VPbkFieldTypeSelectorFactory::EVideoCallSelector, aContactField );
            }
        }

    // Presence data
    if (dataType == CCmsContactFieldItem::ECmsPresenceData)
        {// Presence is one of the most complex data types and
        // not always working. TRAPping this is just a precaution
        // to ensure that other fields are updated correctly to
        // to the screen..
        TRAP_IGNORE( ContactPresenceChangedL( aContactField ));
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::CheckPopupTextL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::CheckPopupTextL(
    const TInt aButtonIndex,
    const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction,
    const CCmsContactField& aContactField )
    {
    CCA_DP( KCommLauncherLogFile, CCA_L("CCCAppCommLauncherLPadModel::CheckPopupTextL"));
    TCommLauncherButtonData& buttonData = iButtonDataArray[ aButtonIndex ];

    if ( aContactAction == buttonData.iContactAction )
        {
        CCA_DP( KCommLauncherLogFile, CCA_L("buttonData.iContactAction: %d"), buttonData.iContactAction);

        CCCAppCommLauncherContactHandler& contactHandler =
            iContainer.Plugin().ContactHandler();//not owned
        buttonData.iNumberOfAddresses =
            contactHandler.AddressAmount( buttonData.iContactAction );

        CCA_DP( KCommLauncherLogFile, CCA_L("buttonData.iNumberOfAddresses: %d"), buttonData.iNumberOfAddresses);

        const CCmsContactFieldItem::TCmsDefaultAttributeTypes cmsDefault =
            contactHandler.MapContactorTypeToCMSDefaultType( buttonData.iContactAction );


        if ( contactHandler.ContactFieldTypeAndContactActionMatch(
                    aContactField.Type(),
                    VPbkFieldTypeSelectorFactory::EFindOnMapSelector ) )
            {// addresses case is handled here
            CheckAddressesPopupTextL(
            		aButtonIndex, aContactAction, aContactField );
            return;
            }
        
        // has default, multiple entries
        if ( aContactField.HasDefaultAttribute( cmsDefault ) && 1 < buttonData.iNumberOfAddresses )
            {// has default, multiple entries
            CCA_DP( KCommLauncherLogFile, CCA_L("aContactField.HasDefaultAttribute(): true"));

            const CCmsContactFieldItem& item = aContactField.ItemL( cmsDefault );//not own
            buttonData.iPopupText.Zero();
            buttonData.iFlags |= TCommLauncherButtonData::EDefaultSet;
            buttonData.iPopupText.Append(item.Data().Left(
                KCCAppCommLauncherMaxButtonDataTextLength ));
             
            SetTextClipDirection(aContactAction, aButtonIndex);
            }
        
        // no default, multiple entries
        else if ( 1 < buttonData.iNumberOfAddresses
                && !( buttonData.iFlags & TCommLauncherButtonData::EDefaultSet ))
            {
            CCA_DP( KCommLauncherLogFile, CCA_L("no default, multiple numbers"));

            buttonData.iPopupText.Zero();
				if ( contactHandler.IsItNumberAddress( aContactAction ))
					{
					if ( aContactAction == VPbkFieldTypeSelectorFactory::EUniEditorSelector )
						{
						// x numbers/addresses
						buttonData.iPopupText.Append(*StringLoader::LoadLC(
							R_QTN_CCA_MULTIPLE_NUMBERS_ADDRESSES, 
							buttonData.iNumberOfAddresses,
							&iCoeEnv));
						}
					else
						{
						// x numbers
						buttonData.iPopupText.Append(*StringLoader::LoadLC(
							R_QTN_CCA_MULTIPLE_NUMBERS,
							buttonData.iNumberOfAddresses,
							&iCoeEnv));
						}
					}
				else if ( aContactAction == VPbkFieldTypeSelectorFactory::EInstantMessagingSelector )
					{
					buttonData.iPopupText.Append(*StringLoader::LoadLC(
						R_QTN_CCA_MULTIPLE_ACCOUNTS,
						buttonData.iNumberOfAddresses,
						&iCoeEnv));
					}
				else
					{
					buttonData.iPopupText.Append(*StringLoader::LoadLC(
						R_QTN_CCA_MULTIPLE_ADDRESSES,
						buttonData.iNumberOfAddresses,
						&iCoeEnv));
					}
				CleanupStack::PopAndDestroy();
            }
        
        // one entry
        else if ( 1 == buttonData.iNumberOfAddresses )
            {
            CCA_DP( KCommLauncherLogFile, CCA_L("one address"));
            
            SetTextClipDirection(aContactAction, aButtonIndex);

                       
            TInt index = 0; // by default select first
            if ( aContactAction == VPbkFieldTypeSelectorFactory::EInstantMessagingSelector )
                {
                // in IM case IMPP field might contain voip and im service
                // fields. Need to filter out voip fields in case of IM
                // commmunication type             
                index = SelectIMIndexL(aContactField);
                }
            
            if ( aContactAction == VPbkFieldTypeSelectorFactory::EUniEditorSelector 
                && buttonData.iPopupText.Length() > 0 
                && aContactField.Type() != CCmsContactFieldItem::ECmsMobilePhoneGeneric
                && aContactField.Type() != CCmsContactFieldItem::ECmsMobilePhoneHome
                && aContactField.Type() != CCmsContactFieldItem::ECmsMobilePhoneWork )
                {
                // Already has a number
                }
            else
                {
                buttonData.iPopupText.Zero();
                buttonData.iPopupText.Append(
                    aContactField.ItemL( index ).Data().Left(
                        KCCAppCommLauncherMaxButtonDataTextLength ));
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::LoadIconArrayL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::LoadIconArrayL()
    {
    // todo; currently icons are just temporary/hardcoded, but they
    // could be loaded directly from resources (or dynamically based on
    // what icons are needed).

    // ECallIconIndex
    iButtonIconArray->AppendL( LoadIconLC(
            EMbmPhonebook2eceQgn_prop_pb_comm_call,
            EMbmPhonebook2eceQgn_prop_pb_comm_call_mask ));
    CleanupStack::Pop(); // icon

    // EMsgIconIndex
    iButtonIconArray->AppendL( LoadIconLC(
            EMbmPhonebook2eceQgn_prop_pb_comm_message,
            EMbmPhonebook2eceQgn_prop_pb_comm_message_mask ));
    CleanupStack::Pop(); // icon

    // EEmailIconIndex
    iButtonIconArray->AppendL( LoadIconLC(
           EMbmPhonebook2eceQgn_prop_pb_comm_email,
           EMbmPhonebook2eceQgn_prop_pb_comm_email_mask ));
    CleanupStack::Pop(); // icon

    // EVoipIconIndex
    iButtonIconArray->AppendL( LoadIconLC(
            EMbmPhonebook2eceQgn_prop_pb_comm_voip,
            EMbmPhonebook2eceQgn_prop_pb_comm_voip_mask ));
    CleanupStack::Pop(); // icon

    // EIMIconIndex
    iButtonIconArray->AppendL( LoadIconLC(
            EMbmPhonebook2eceQgn_prop_pb_comm_chat,
            EMbmPhonebook2eceQgn_prop_pb_comm_chat_mask ));
    CleanupStack::Pop(); // icon

    // EUrlIconIndex
    iButtonIconArray->AppendL( LoadIconLC(
            EMbmPhonebook2eceQgn_prop_pb_comm_url,
            EMbmPhonebook2eceQgn_prop_pb_comm_url_mask ));
    CleanupStack::Pop(); // icon

    // EVideocallIconIndex
    iButtonIconArray->AppendL( LoadIconLC(
            EMbmPhonebook2eceQgn_prop_pb_comm_vcall,
            EMbmPhonebook2eceQgn_prop_pb_comm_vcall_mask ));
    CleanupStack::Pop(); // icon
    
    // EAddressValIconIndex
	iButtonIconArray->AppendL( LoadIconLC(
	        EMbmPhonebook2eceQgn_prop_pb_comm_valid_lm,
	        EMbmPhonebook2eceQgn_prop_pb_comm_valid_lm_mask ));
	CleanupStack::Pop(); // icon
        
	// EAddressNotValIconIndex
	iButtonIconArray->AppendL( LoadIconLC(
	        EMbmPhonebook2eceQgn_prop_pb_comm_no_valid_lm,
	        EMbmPhonebook2eceQgn_prop_pb_comm_no_valid_lm_mask ));
	CleanupStack::Pop(); // icon
	
	// EMultiIconIndex
	iButtonIconArray->AppendL( LoadIconLC(
			EMbmPhonebook2eceQgn_indi_many_items_add,
			EMbmPhonebook2eceQgn_indi_many_items_add_mask ));
	CleanupStack::Pop(); // icon
    
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::LoadIconLC
// ---------------------------------------------------------------------------
//
CGulIcon* CCCAppCommLauncherLPadModel::LoadIconLC( TInt aBmpId, TInt aMaskId )
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

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::IconArray
// ---------------------------------------------------------------------------
//
CAknIconArray* CCCAppCommLauncherLPadModel::IconArray()
    {
    return iButtonIconArray;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::ButtonData
// ---------------------------------------------------------------------------
//
TCommLauncherButtonData& CCCAppCommLauncherLPadModel::ButtonData( TInt aIndex )
    {
    return iButtonDataArray[ aIndex ];
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::TextForPopUpL
// ---------------------------------------------------------------------------
//
TPtrC CCCAppCommLauncherLPadModel::TextForPopUpL( TInt aButtonIndex )
    {
    TPtr tempText = iTempText->Des();
    tempText.Zero();
    if ( iButtonDataArray[ aButtonIndex ].iContactAction
    		== VPbkFieldTypeSelectorFactory::EFindOnMapSelector
    		&& iButtonDataArray[ aButtonIndex ].iNumberOfAddresses == 1 )
    	{
    	return AddressTextForPopUpL();
    	}
    else
    	{
        tempText.Append( iButtonDataArray[ aButtonIndex ].iPopupText );
    	}
           
    return *iTempText;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::Reset
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::Reset()
    {
    iButtonDataArray.Reset();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::ReplaceWithDefaultIconL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::ReplaceWithDefaultIconL(
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aMask,
    const TUint32 aServiceType )
    {
    delete aBitmap;
    delete aMask;
    CGulIcon* icon = NULL;

    switch ( aServiceType )
        {
        /* The VOIP Button doesnt show presence CCA UI Spec will be
         * updated with this info.
         * Thats why this part of code is commeneted
        case CCmsContactFieldItem::ECmsPresenceVoIPNotification:
            icon = LoadIconLC(
                    EMbmPhonebook2eceQgn_prop_nrtyp_voip,
                    EMbmPhonebook2eceQgn_prop_nrtyp_voip_mask  );
            break;*/
        case CCmsContactFieldItem::ECmsPresenceChatNotification:
            icon = LoadIconLC(
                    EMbmPhonebook2eceQgn_prop_nrtyp_chat,
                    EMbmPhonebook2eceQgn_prop_nrtyp_chat_mask );
            break;
        default:
            // Only ECmsPresenceVoIPNotification and
            // ECmsPresenceChatNotification supported
            User::Leave( KErrArgument );
            break;
        }

    aBitmap = icon->Bitmap();
    aMask = icon->Mask();
    icon->SetBitmapsOwnedExternally( ETrue );
    CleanupStack::PopAndDestroy( icon );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::ContactPresenceChangedL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::ContactPresenceChangedL(
    const CCmsContactField& aContactField )
    {
    //Get the size of icon for Voip presence
    TRect mainPane = iPlugin.ClientRect();
    TAknLayoutRect listLayoutRect;
        listLayoutRect.LayoutRect(
            mainPane,
            AknLayoutScalable_Avkon::list_single_large_graphic_pane_g1(0).LayoutLine() );
    TSize size(listLayoutRect.Rect().Size());
    
    const TInt count = aContactField.ItemCount();
    for (TUint i=0; i < count; i++)
        {
        CCmsPresenceData& presData = ( CCmsPresenceData& )aContactField.ItemL( i );
        presData.PreparePresenceDataL( size );
        TUint32 serviceType = presData.ServiceType();
        TInt iconInd = KErrNotFound;
        switch (serviceType)
            {
            /* The VOIP Button doesnt show presence CCA UI Spec will be
             * updated with this info.
             * Thats why this part of code is commeneted
            case CCmsContactFieldItem::ECmsPresenceVoIPNotification:
                iconInd = MapCommMethodToIcon(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector );
                break;*/
            case CCmsContactFieldItem::ECmsPresenceChatNotification:
                iconInd = MapCommMethodToIcon(
                    VPbkFieldTypeSelectorFactory::EInstantMessagingSelector );
                break;
            }
        if (iconInd > 0 && iconInd < iButtonIconArray->Count())
            {
            CFbsBitmap* mask = presData.Mask();
            CFbsBitmap* bitmap = presData.Bitmap();

            if ( bitmap && NULL == bitmap->Handle() )
                {
                // There should not be a case with empty bitmaps, so
                // replace with the default icons.
                ReplaceWithDefaultIconL( bitmap, mask, serviceType );
                }

            if ( bitmap || mask )
                {
                iButtonIconArray->At(iconInd)->SetBitmap(bitmap);
                iButtonIconArray->At(iconInd)->SetMask(mask);
                }
            }
        else
            {
            delete presData.Bitmap();
            delete presData.Mask();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::AddressButtonL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::AddressButtonL(
		const TInt aNumberOfAddresses, const TInt aIndex )
    {
    RArray<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>& 
        preferredCommMethods = iContainer.Plugin().PreferredCommMethods();//not owned

    RPointerArray<CMnProvider> providers;
    CleanupClosePushL( providers );
    MnProviderFinder::FindProvidersL( providers,
        CMnProvider::EServiceMapView );
    if (providers.Count() > 0)
        {
        TCommLauncherButtonData buttonData = 
            TCommLauncherButtonData( preferredCommMethods[aIndex] );
        ButtonTextL(preferredCommMethods[aIndex], buttonData.iText);
        buttonData.iNumberOfAddresses = aNumberOfAddresses;
        iButtonDataArray.AppendL( buttonData );
        }
    providers.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &providers );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::UpdateAddressesValidationL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::UpdateAddressesValidationL(
		const CCmsContactFieldInfo& aContactFieldInfo )
    {
    TInt count = aContactFieldInfo.Fields().Count();
    if ( count )
    	{
    	TBool generalAddress = EFalse;
    	TBool homeAddress = EFalse;
    	TBool workAddress = EFalse;
    	TBool generalGeo = EFalse;
    	TBool homeGeo = EFalse;
    	TBool workGeo = EFalse;
    	for ( TInt i = 0; i < count; i++ )
    		{
    		switch ( aContactFieldInfo.Fields().operator []( i ) )
    			{
    		    case CCmsContactFieldItem::ECmsAddrPOGeneric:
    		    case CCmsContactFieldItem::ECmsAddrExtGeneric:
    		    case CCmsContactFieldItem::ECmsAddrStreetGeneric:
    		    case CCmsContactFieldItem::ECmsAddrLocalGeneric:
    		    case CCmsContactFieldItem::ECmsAddrRegionGeneric:
    		    case CCmsContactFieldItem::ECmsAddrPostcodeGeneric:
    		    case CCmsContactFieldItem::ECmsAddrCountryGeneric:
    		    	{
    		    	generalAddress = ETrue;
    		    	break;
    		    	}
    		    case CCmsContactFieldItem::ECmsAddrPOHome:
    		    case CCmsContactFieldItem::ECmsAddrExtHome:
    		    case CCmsContactFieldItem::ECmsAddrStreetHome:
    		    case CCmsContactFieldItem::ECmsAddrLocalHome:
    		    case CCmsContactFieldItem::ECmsAddrRegionHome:
    		    case CCmsContactFieldItem::ECmsAddrPostcodeHome:
    		    case CCmsContactFieldItem::ECmsAddrCountryHome:
    		        {
    		        homeAddress = ETrue;
    		        break;
    		        }
    		    case CCmsContactFieldItem::ECmsAddrPOWork:
    		    case CCmsContactFieldItem::ECmsAddrExtWork:
    		    case CCmsContactFieldItem::ECmsAddrStreetWork:
    		    case CCmsContactFieldItem::ECmsAddrLocalWork:
    		    case CCmsContactFieldItem::ECmsAddrRegionWork:
    		    case CCmsContactFieldItem::ECmsAddrPostcodeWork:
    		    case CCmsContactFieldItem::ECmsAddrCountryWork:
    		        {
    		        workAddress = ETrue;
    		        break;
    		        }
    		    case CCmsContactFieldItem::ECmsAddrGeoGeneric:
    		        {
    		        generalGeo = ETrue;
    		        break;
    		        }
    		    case CCmsContactFieldItem::ECmsAddrGeoHome:
    		        {
    		        homeGeo = ETrue;
    		        break;
    		        }
    		    case CCmsContactFieldItem::ECmsAddrGeoWork:
    		        {
    		        workGeo = ETrue;
    		        break;
    		        }
    		    default:
    		    	{
    		    	// do nothing
    		    	break;
    		    	}
    			}
    		}
    	if ( ( generalAddress && !generalGeo )
    			|| ( homeAddress && !homeGeo )
    			|| ( workAddress && !workGeo ) )
    		{
    		iAddressesValidated = EFalse;
    		}
    	else
    		{
    		iAddressesValidated = ETrue;
    		}
    	}
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::CheckAddressesPopupTextL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::CheckAddressesPopupTextL(
    const TInt aButtonIndex,
    const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
            aContactAction,
    const CCmsContactField& aContactField )
    {
    CCA_DP( KCommLauncherLogFile, CCA_L(
    		"CCCAppCommLauncherLPadModel::CheckAddressesPopupTextL" ) );
    TCommLauncherButtonData& buttonData = iButtonDataArray[ aButtonIndex ];
 
    if ( aContactAction == buttonData.iContactAction )
        {
        CCCAppCommLauncherContactHandler& contactHandler =
                iContainer.Plugin().ContactHandler();//not owned
        buttonData.iNumberOfAddresses =
                contactHandler.AddressAmount( buttonData.iContactAction );

        if ( contactHandler.ContactFieldTypeAndContactActionMatch(
        		aContactField.Type(),
        		VPbkFieldTypeSelectorFactory::EFindOnMapSelector ) )
        	{
        	if ( buttonData.iNumberOfAddresses == 1 )
        		{// one address case handled here
        		TInt index = 0; // by default select first

        		switch ( aContactField.Type() )
        	        {
        	        case CCmsContactFieldItem::ECmsAddrPOGeneric:
        	        case CCmsContactFieldItem::ECmsAddrPOHome:
        	        case CCmsContactFieldItem::ECmsAddrPOWork:
        	    	    {
        	    	    iAddressFields.InsertL( EAddressPO,
        	    	    	aContactField.ItemL( index ).Data().Left(
        	    	    	KCCAppCommLauncherMaxButtonDataTextLength ) );
        	    	    break;
        	    	    }
        	        case CCmsContactFieldItem::ECmsAddrExtGeneric:
        	        case CCmsContactFieldItem::ECmsAddrExtHome:
        	        case CCmsContactFieldItem::ECmsAddrExtWork:
        	    	    {
        	    	    iAddressFields.InsertL( EAddressExt,
        	    	        aContactField.ItemL( index ).Data().Left(
        	    	        KCCAppCommLauncherMaxButtonDataTextLength ) );
        	    	    break;
        	    	    }
        	        case CCmsContactFieldItem::ECmsAddrStreetGeneric:
        	        case CCmsContactFieldItem::ECmsAddrStreetHome:
        	        case CCmsContactFieldItem::ECmsAddrStreetWork:
        	            {
        	            iAddressFields.InsertL( EAddressStreet,
        	                aContactField.ItemL( index ).Data().Left(
        	                KCCAppCommLauncherMaxButtonDataTextLength ) );
        	            break;
        	            }
        	        case CCmsContactFieldItem::ECmsAddrLocalGeneric:
        	        case CCmsContactFieldItem::ECmsAddrLocalHome:
        	        case CCmsContactFieldItem::ECmsAddrLocalWork:
        	            {
        	            iAddressFields.InsertL( EAddressLocal,
        	                aContactField.ItemL( index ).Data().Left(
        	                KCCAppCommLauncherMaxButtonDataTextLength ) );
        	            break;
        	            }
        	        case CCmsContactFieldItem::ECmsAddrRegionGeneric:
        	        case CCmsContactFieldItem::ECmsAddrRegionHome:
        	        case CCmsContactFieldItem::ECmsAddrRegionWork:
        	            {
        	            iAddressFields.InsertL( EAddressRegion,
        	                aContactField.ItemL( index ).Data().Left(
        	                KCCAppCommLauncherMaxButtonDataTextLength ) );
        	            break;
        	            }
        	        case CCmsContactFieldItem::ECmsAddrPostcodeGeneric:
        	        case CCmsContactFieldItem::ECmsAddrPostcodeHome:
        	        case CCmsContactFieldItem::ECmsAddrPostcodeWork:
        	            {
        	            iAddressFields.InsertL( EAddressPostcode,
        	                aContactField.ItemL( index ).Data().Left(
        	                KCCAppCommLauncherMaxButtonDataTextLength ) );
        	            break;
        	            }
        	        case CCmsContactFieldItem::ECmsAddrCountryGeneric:
        	        case CCmsContactFieldItem::ECmsAddrCountryHome:
        	        case CCmsContactFieldItem::ECmsAddrCountryWork:
        	            {
        	            iAddressFields.InsertL( EAddressCountry,
        	                aContactField.ItemL( index ).Data().Left(
        	                KCCAppCommLauncherMaxButtonDataTextLength ) );
        	            break;
        	            }
        	        default:
        	    	    {
        	    	    // nothing to do
        	    	    return;
        	    	    }
        	        }
        	    }
        	else
        		{// multiple addresses case handled here
            	buttonData.iPopupText.Zero();
                buttonData.iPopupText.Append(*StringLoader::LoadLC(
                    R_QTN_CCA_MULTIPLE_ADDRESSES,
                    buttonData.iNumberOfAddresses));
                CleanupStack::PopAndDestroy();
            	}
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::AddressTextForPopUpL
// ---------------------------------------------------------------------------
//   
TPtrC CCCAppCommLauncherLPadModel::AddressTextForPopUpL()
    {
    const TInt KGranularity = 4; 
    CDesCArrayFlat* fields = new ( ELeave ) CDesCArrayFlat( KGranularity );
    CleanupStack::PushL( fields );

    TPtr tempText = iTempText->Des();
    tempText.Zero();

    HBufC* formattedText = NULL;
    TBool street = EFalse;
    TBool local = EFalse;

    TPtrC* text = iAddressFields.Find( EAddressStreet );
    if ( text )
    	{
        fields->AppendL( *text );
        street = ETrue;
    	}
    text = iAddressFields.Find( EAddressLocal );
    if ( text )
    	{
    	fields->AppendL( *text );
    	local = ETrue;
    	}

    if ( street && local )
    	{
        formattedText = StringLoader::LoadLC(
    	        R_QTN_PHOB_COMMLAUNCHER_ONELINEPREVIEW, *fields );
    	}
    else
        {
        formattedText = StringLoader::LoadLC(
    	   		R_QTN_PHOB_POPUP_INCOMPLETE_ADDRESS );
    	}

    tempText.Append( *formattedText );
    CleanupStack::PopAndDestroy( formattedText );
    CleanupStack::PopAndDestroy( fields );
           
    return *iTempText;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::LoadVoipButtonInfoL
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherLPadModel::LoadVoipButtonInfoL()
    {    
    TInt returnVal (0);
    TServiceId serviceId; //Stores the last found Service Id of VOIP Service
    TInt availableVoipServices = GetSupportedVOIPServicesL( serviceId ); 
    
    if ( KOneVOIPServiceAvailable == availableVoipServices )
        {
        HBufC* serviceName = NULL;
        CFbsBitmap* bitmap(NULL);
        CFbsBitmap* mask(NULL);
        //Now we have only 1 service which supports VOIP
        //hence load the VOIP Button Icon & corresponding label              
        LoadVoipButtonInfoFromPbkL( serviceId, bitmap, mask, serviceName );
        
        CleanupStack::PushL( serviceName );
        
        //Replace the default icons with the branded icon
        if ( bitmap || mask )
            {   
            TInt iconInd = MapCommMethodToIcon(
                                VPbkFieldTypeSelectorFactory::EVOIPCallSelector );
            //we have found the branded icon for voip button        
            iButtonIconArray->At(iconInd)->SetBitmap(bitmap);
            iButtonIconArray->At(iconInd)->SetMask(mask);
                   
            returnVal |= KVOIPButtonImageSet;
            }
        
        //Replace the default button text with the branded servicename
        for ( TInt i = 0; i < iButtonDataArray.Count() && serviceName ; i++ )
            {
            TCommLauncherButtonData& buttonData ( iButtonDataArray[i] );
            
            if ( VPbkFieldTypeSelectorFactory::EVOIPCallSelector == 
                            iButtonDataArray[i].iContactAction )
                {
                HBufC* str = StringLoader::LoadLC( R_QTN_CCA_VOIP_CALL_WITH_SERVICENAME, 
                        *serviceName,             
                        &iCoeEnv );
                
                buttonData.iText.Copy( str->Left( KCCAppCommLauncherMaxButtonDataTextLength ) );        
                CleanupStack::PopAndDestroy(); //str
                //Button Text has been set
                returnVal |= KVOIPButtonTextSet;
                break;
                }            
            }
        
        CleanupStack::PopAndDestroy(); //serviceName
        }
    
    return returnVal;
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::GetSupportedVOIPServicesL
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherLPadModel::GetSupportedVOIPServicesL( TServiceId& aServiceId )
    {  
    //Find all services which are voip enabled 
    TInt availableVoipService (0);    
    RIdArray idArray;
    CleanupClosePushL(idArray);
    
    User::LeaveIfError( iSettings->FindServiceIdsL(idArray) );
    
    for (TInt i = 0; i < idArray.Count(); ++i)
        {
        TBool supported( EFalse );
        CSPEntry* entry = CSPEntry::NewLC();
        TServiceId id = idArray[i];
        User::LeaveIfError( iSettings->FindEntryL(id, *entry) );
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
    CleanupStack::PopAndDestroy(); //idArray    
    
    return availableVoipService;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::LoadVoipButtonInfoFromPbkL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::LoadVoipButtonInfoFromPbkL( 
            TServiceId aServiceId,
            CFbsBitmap*& aBitmap, CFbsBitmap*& aMask, HBufC*& aLocalisedServiceName )
    {
    // get the XSP ServiceName 
    // CPbk2ServiceManager stores all the brandinfo
    // related to the services configured to the phone
    // use this to show uniform icon & name throughout PhoneBook    
    CPbk2ApplicationServices& appServices = iPbkCmd->ApplicationServices();
    CPbk2ServiceManager& servMan = appServices.ServiceManager();
    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();    
    for ( TInt i = 0; i < services.Count(); i++ )
        {
        const CPbk2ServiceManager::TService& service = services[i];
        //Found the appropriate service info
        if ( service.iServiceId == aServiceId )
            {
            //Calculate the Size of the Bitmap for Comm Launcher
            TRect mainPane = iPlugin.ClientRect();                            
            TAknLayoutRect listLayoutRect;
                listLayoutRect.LayoutRect(
                    mainPane,
                    AknLayoutScalable_Avkon::list_single_large_graphic_pane_g1(0).LayoutLine() );
            TSize size(listLayoutRect.Rect().Size());
                        
            //Set the size of this bitmap.
            //without this Cloning of bitmap will not happen            
            AknIconUtils::SetSize( service.iBitmap, size );
            AknIconUtils::SetSize( service.iMask, size );
               
            //Trickiest Bitmap cloning
            //No direct way of cloning a bitmap
            aBitmap = CloneBitmapLC(size, service.iBitmap);
            aMask = CloneBitmapLC(size, service.iMask);
            
            //Calculate preferred size for xsp service icons
            AknLayoutUtils::LayoutMetricsRect(
                AknLayoutUtils::EMainPane, mainPane );            
            listLayoutRect.LayoutRect(
                mainPane,
                AknLayoutScalable_Avkon::list_single_graphic_pane_g2(0).LayoutLine() );
            TSize xspIconSize(listLayoutRect.Rect().Size()); 
            AknIconUtils::SetSize( service.iBitmap, xspIconSize );
            AknIconUtils::SetSize( service.iMask, xspIconSize );
            
            aLocalisedServiceName = service.iDisplayName.AllocL(); 
            
            CleanupStack::Pop( 2 ); //aBitmap, aMask
            break;
            }
        }            
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::HandleNotifyChange
// ---------------------------------------------------------------------------
//
// from MSPNotifyChangeObserver
void CCCAppCommLauncherLPadModel::HandleNotifyChange( TUint /*aServiceId*/ )
    {
    TRAP_IGNORE ( DoHandleNotifyChangeL() );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::DoHandleNotifyChangeL
// ---------------------------------------------------------------------------
// 
void CCCAppCommLauncherLPadModel::DoHandleNotifyChangeL()
    {
    TInt buttonInfoSet = LoadVoipButtonInfoL();
    
    if (!( buttonInfoSet && KVOIPButtonImageSet ))
        {
        //VOIP Button Image has not been set
        //reason could be Branding ServerIssue or something else
        //replace with default icon if this is the case
        TInt iconInd = MapCommMethodToIcon(
                          VPbkFieldTypeSelectorFactory::EVOIPCallSelector );  
        CGulIcon* icon = LoadIconLC(
                        EMbmPhonebook2eceQgn_prop_nrtyp_voip,
                        EMbmPhonebook2eceQgn_prop_nrtyp_voip_mask  );
        
        icon->SetBitmapsOwnedExternally( ETrue );       
        
        iButtonIconArray->At(iconInd)->SetBitmap(icon->Bitmap());
        iButtonIconArray->At(iconInd)->SetMask(icon->Mask());
        
        CleanupStack::PopAndDestroy( icon );        
        }
    
    if (!( buttonInfoSet && KVOIPButtonTextSet ))
        {
        //VOIP Button Text has not been set
        //reason could be Branding ServerIssue or something else
        //replace with default Text if this is the case
        
        //Replace the default button text with the branded servicename
        for ( TInt i = 0; i < iButtonDataArray.Count(); i++ )
            {
            TCommLauncherButtonData& buttonData ( iButtonDataArray[i] );
            
            if ( VPbkFieldTypeSelectorFactory::EVOIPCallSelector == 
                            iButtonDataArray[i].iContactAction )
                {
                HBufC* str = StringLoader::LoadLC( R_QTN_CCA_VOIP_CALL, 
                        &iCoeEnv );
                
                buttonData.iText.Copy( str->Left( KCCAppCommLauncherMaxButtonDataTextLength ) );        
                CleanupStack::PopAndDestroy(); //str     
                break;
                }            
            }
        }
        
    iContainer.DrawDeferred();
    }    


// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::HandleError
// ---------------------------------------------------------------------------
//
// from MSPNotifyChangeObserver
void CCCAppCommLauncherLPadModel::HandleError( TInt /*aError*/ )
    {    
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::IfShowMultiIcon
// ---------------------------------------------------------------------------
//
TBool CCCAppCommLauncherLPadModel::IfShowMultiIcon(TInt aButtonIndex) const
	{
	TBool result = EFalse; 
	if ( iButtonDataArray[ aButtonIndex ].iNumberOfAddresses > 1 )
		{
		result = ETrue;
		}
	return result;
	}


// ----------------------------------------------------------
// CCCAppCommLauncherLPadModel::SetTextClipDirection
// 
// ----------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::SetTextClipDirection( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction,
		TInt aButtonIndex
		/*CCmsContactFieldItem::TCmsContactField aCmsFieldType*/ )
	{
	TCommLauncherButtonData& buttonData = iButtonDataArray[ aButtonIndex ];
	
	// Phone number, email address, and sip address, clip from begining
	if ( VPbkFieldTypeSelectorFactory::EVoiceCallSelector == aContactAction
	        || VPbkFieldTypeSelectorFactory::EUniEditorSelector == aContactAction
	        || VPbkFieldTypeSelectorFactory::EVOIPCallSelector == aContactAction
	        || VPbkFieldTypeSelectorFactory::EVideoCallSelector == aContactAction
	        || VPbkFieldTypeSelectorFactory::EEmailEditorSelector == aContactAction
	        || VPbkFieldTypeSelectorFactory::EInstantMessagingSelector == aContactAction )
	     {
	     buttonData.iClipFromBegining = ETrue;
	     }
	else 
		{
		buttonData.iClipFromBegining = EFalse;
		}
	}

// ----------------------------------------------------------
// CCCAppCommLauncherLPadModel::ClipFromBeginning
// 
// ----------------------------------------------------------
//
TBool CCCAppCommLauncherLPadModel::ClipFromBeginning(
    TDes& aBuffer,
    TInt aItemIndex,
    TInt aSubCellNumber) const
{
    CAknDoubleLargeStyleListBox* listbox =
        static_cast<CAknDoubleLargeStyleListBox*>(&iListBox);

    return AknTextUtils::ClipToFit(
        aBuffer,
        AknTextUtils::EClipFromBeginning,
        listbox,
        aItemIndex,
        aSubCellNumber);
}


// End of File
