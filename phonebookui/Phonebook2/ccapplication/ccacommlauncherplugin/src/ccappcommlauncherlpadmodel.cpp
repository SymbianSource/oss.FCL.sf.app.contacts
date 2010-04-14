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
#include <aknlayoutscalable_apps.cdl.h>
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

#include <AknBidiTextUtils.h>

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
 * Returns index of a first VOIP field.
 */
TInt SelectVOIPIndexL(const CCmsContactField& aContactField)
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
            
            if (entry->GetProperty(prop, EPropertyServiceAttributeMask) == KErrNone)
                {
                TInt value = 0;
                prop->GetValue(value);
                
                if ( value & ESupportsInternetCall )
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
        TPtrC popupText;        
        TRAPD( error, popupText.Set( 
        		const_cast <CCCAppCommLauncherLPadModel*>(this)->TextForPopUpL( aIndex ) ) );
        if ( KErrNone == error )
            {
            textPtr.Copy( popupText );
            }
        else
            {
            textPtr.Copy( iButtonDataArray[ aIndex ].iPopupText );
            }
        
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
        if( textPtr.Length() + tempText.Length() >=
                KCCAppCommLauncherMaxButtonDataTextLength )
            {
            tempText.Append( textPtr.Left(
               KCCAppCommLauncherMaxButtonDataTextLength - tempText.Length() - 4 ) );
            }
        else
            {
            tempText.Append( textPtr );
            }
        tempText.Append( KColumnListSeparator ); 
        
        // Check presence icon
		if ( iButtonDataArray[ aIndex ].iFlags & 
				TCommLauncherButtonData::EHasPresenceIcon )
			{
			tempText.AppendNum( EPresenceIconIndex );
			}
        tempText.Append( KColumnListSeparator ); 
        
        // Check if show multi icon at the right end of second row
        if ( IfShowMultiIcon( aIndex ) )
        	{
        	tempText.AppendNum( EMultiIconIndex );
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
    CalculateLayoutSize();
    
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
            
            if ( VPbkFieldTypeSelectorFactory::EVOIPCallSelector == aContactAction )
                {
                index = SelectVOIPIndexL( aContactField );
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
        
        if( IsPhoneNumber( aContactField ) )
            {
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( buttonData.iPopupText );
            }
        
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::IsPhoneNumber
// ---------------------------------------------------------------------------
//
TBool CCCAppCommLauncherLPadModel::IsPhoneNumber( const CCmsContactField& aContactField )
    {
    TBool isPhoneNumber = EFalse;

    switch( aContactField.Type() )
        {
        case CCmsContactFieldItem::ECmsLandPhoneGeneric:
        case CCmsContactFieldItem::ECmsLandPhoneHome:
        case CCmsContactFieldItem::ECmsGroupVoice:
        case CCmsContactFieldItem::ECmsAssistantNumber:
        case CCmsContactFieldItem::ECmsDefaultTypePhoneNumber:
        case CCmsContactFieldItem::ECmsFaxNumberHome:
        case CCmsContactFieldItem::ECmsFaxNumberWork:
        case CCmsContactFieldItem::ECmsMobilePhoneGeneric:
        case CCmsContactFieldItem::ECmsMobilePhoneHome:
        case CCmsContactFieldItem::ECmsMobilePhoneWork:
        case CCmsContactFieldItem::ECmsVideoNumberGeneric:
        case CCmsContactFieldItem::ECmsVideoNumberHome:
        case CCmsContactFieldItem::ECmsVideoNumberWork:
        case CCmsContactFieldItem::ECmsVoipNumberGeneric:
        case CCmsContactFieldItem::ECmsVoipNumberHome:
        case CCmsContactFieldItem::ECmsVoipNumberWork:
        case CCmsContactFieldItem::ECmsCarPhone:
        case CCmsContactFieldItem::ECmsPagerNumber:
            isPhoneNumber = ETrue;
            break;
        default:
            isPhoneNumber = EFalse;
            break;
        }

    return isPhoneNumber;
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
   
    // The color of Icon "many items" should be adjusted to the theme background
    if( EMbmPhonebook2eceQgn_indi_many_items_add == aBmpId)
        {
        TAknsItemID skin; 
        skin.Set( EAknsMajorGeneric,  EAknsMinorGenericQgnIndiManyItemsAdd );
        TAknsItemID color;
        color.Set( EAknsMajorSkin, EAknsMinorQsnIconColors );
    
        AknsUtils::CreateColorIconLC(
            AknsUtils::SkinInstance(),skin,
            color, EAknsCIQsnIconColorsCG13,
            bmp, mask,
            KPbk2ECEIconFileName,
            aBmpId, aMaskId,
            AKN_LAF_COLOR_STATIC( 215 ) );
        }
    else
        {
        AknIconUtils::CreateIconLC(
            bmp, mask, KPbk2ECEIconFileName, aBmpId, aMaskId );
        }
   
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
    iButtonIconArray->Reset();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::ContactPresenceChangedL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::ContactPresenceChangedL(
    const CCmsContactField& aContactField )
    {
    const TInt count = aContactField.ItemCount();
    for (TUint i=0; i < count; i++)
        {
        CCmsPresenceData& presData = ( CCmsPresenceData& )aContactField.ItemL( i );
        presData.PreparePresenceDataL( iPresenceIconSize );
        TUint32 serviceType = presData.ServiceType();      
        
        if ( serviceType == CCmsContactFieldItem::ECmsPresenceChatNotification )
            {
            CFbsBitmap* mask = presData.Mask();
            CFbsBitmap* bitmap = presData.Bitmap();
            
            // Find the index for chat item
        	TInt index = KErrNotFound;
        	const TInt dataCount = iButtonDataArray.Count();
        	for ( TInt i = 0; i < dataCount; i++ )
        		{
        		if ( iButtonDataArray[ i ].iContactAction 
    					== VPbkFieldTypeSelectorFactory::EInstantMessagingSelector )
        			{
        			index = i;
        			break;
        			}
        		}
        	
        	if ( index != KErrNotFound )
        		{
        	    TBool hasPresenceIcon = iButtonDataArray[ index ].iFlags & 
			                TCommLauncherButtonData::EHasPresenceIcon;
        	
                if ( bitmap && ( NULL == bitmap->Handle() ) )
            	    {
            	    if ( hasPresenceIcon )
            		    {
            		    // Delete presence icon from icon array 
            		    iButtonDataArray[ index ].iFlags &= ~(TCommLauncherButtonData::EHasPresenceIcon);
            		    iButtonIconArray->Delete( EPresenceIconIndex );
            		    }
            	    }
                else if ( bitmap )
                    {           	       	
            	    if ( hasPresenceIcon )
            		    {
            		    // Update presence icon
                	    iButtonIconArray->At( EPresenceIconIndex )->SetBitmap(bitmap);
                	    iButtonIconArray->At( EPresenceIconIndex )->SetMask(mask);
            		    }
            	    else
            		    { 
            		    // Append presence icon to icon array
                        CGulIcon* icon = CGulIcon::NewLC();
                        icon->SetBitmap( bitmap );
                        icon->SetMask( mask );
                	    iButtonIconArray->AppendL( icon );
                	    iButtonDataArray[ index ].iFlags |= TCommLauncherButtonData::EHasPresenceIcon;
                	    CleanupStack::Pop(); // icon
            		    }
                    }
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
            // Set service bitmap size           
            AknIconUtils::SetSize( service.iBitmap, iServiceIconSize );
            AknIconUtils::SetSize( service.iMask, iServiceIconSize );
               
            //Trickiest Bitmap cloning
            //No direct way of cloning a bitmap
            aBitmap = CloneBitmapLC( iServiceIconSize, service.iBitmap );
            aMask = CloneBitmapLC( iServiceIconSize, service.iMask );
                       
            aLocalisedServiceName = service.iDisplayName.AllocL(); 
            
            CleanupStack::Pop( 2 ); //aBitmap, aMask
            break;
            }
        }            
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLPadModel::CalculateLayoutSize()
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLPadModel::CalculateLayoutSize()
    {     
    /* Calculate the layout size for Voip service icon and presence icon.
     * Since the layoutRect is relative to the layoutRect of its parent, so 
     * we calculate from the topmost-mainPane, then follow below sequence:
     * phob2_contact_card_pane
     * phob2_cc_listscroll_pane
     * phob2_cc_list_pane
     * list_double_large_graphic_phob2_cc_pane
     * list_double_large_graphic_phob2_cc_pane_g1
     */
    TRect mainPane = iPlugin.ClientRect();
    
    TAknLayoutRect listLayoutRect0;
    listLayoutRect0.LayoutRect(
            mainPane,
            AknLayoutScalable_Apps::phob2_contact_card_pane(0).LayoutLine() );
    
    TAknLayoutRect listLayoutRect1;
    listLayoutRect1.LayoutRect(
    		listLayoutRect0.Rect(),
            AknLayoutScalable_Apps::phob2_cc_listscroll_pane(0).LayoutLine() );
    
    TAknLayoutRect listLayoutRect2;
    listLayoutRect2.LayoutRect(
    		listLayoutRect1.Rect(),
    		AknLayoutScalable_Apps::phob2_cc_list_pane(0).LayoutLine() );
    
    TAknLayoutRect listLayoutRect3;
    listLayoutRect3.LayoutRect(
    		listLayoutRect2.Rect(),
    		AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane(0).LayoutLine() );
            
    TAknLayoutRect listLayoutRect4;
    listLayoutRect4.LayoutRect(
            listLayoutRect3.Rect(),
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_g1(0).LayoutLine() );
    
    TAknLayoutRect listLayoutRect5;
    listLayoutRect5.LayoutRect(
            listLayoutRect3.Rect(),
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_g2(0).LayoutLine() );
    
    // Size for service icon
    iServiceIconSize = listLayoutRect4.Rect().Size();
    
    // Size for presence icon
    iPresenceIconSize = listLayoutRect5.Rect().Size(); 
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
    TBool result = EFalse;
    if ( IfShowMultiIcon( aItemIndex ) )
        {
        TRect mainPane = iPlugin.ClientRect();
        TAknLayoutRect listLayoutRect;
        listLayoutRect.LayoutRect(
            mainPane,
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_cc_pane_g3(0).LayoutLine() );
        TInt multiIconWidth(listLayoutRect.Rect().Width());
        const TInt KCalculationErrors = 1;
        CFormattedCellListBoxData *data = listbox->ItemDrawer()->FormattedCellData();
        const CFont *font = data->Font(listbox->ItemDrawer()->Properties(aItemIndex), aSubCellNumber);
        TSize cellsize = data->SubCellSize(aSubCellNumber);
        TMargins margin = data->SubCellMargins(aSubCellNumber);
        TInt width = cellsize.iWidth - margin.iLeft - margin.iRight - 
                        multiIconWidth - KAknBidiExtraSpacePerLine - KCalculationErrors;
        TInt clipgap = data->SubCellTextClipGap(aSubCellNumber);
        result = AknTextUtils::ClipToFit(
            aBuffer, *font, width, AknTextUtils::EClipFromBeginning, width + clipgap);
        }
    else
        {
        result = AknTextUtils::ClipToFit(
        aBuffer,
        AknTextUtils::EClipFromBeginning,
        listbox,
        aItemIndex,
        aSubCellNumber);
}
    return result;
    }

// ----------------------------------------------------------
// CCCAppCommLauncherLPadModel::ResourceChangedL
// 
// ----------------------------------------------------------
void CCCAppCommLauncherLPadModel::ResourceChangedL()
	{
	// When layout variant changed, calculate the layout size which will be
	// used in listbox, eg: the size for service icon and presence icon
	CalculateLayoutSize();
	
    LoadVoipButtonInfoL();
	}
	
// End of File
