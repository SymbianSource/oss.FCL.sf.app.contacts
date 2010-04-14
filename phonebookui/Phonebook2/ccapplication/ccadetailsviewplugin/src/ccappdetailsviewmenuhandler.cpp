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
* Description:  Implementation of ccappdetailsview menuhandler
*  Version     : %version: be1neux1#31.1.17 %
*
*/

#include <AiwServiceHandler.h>
#include <AiwContactAssignDataTypes.h>

#include <aknnotewrappers.h>

#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactStoreProperties.h>
#include <Pbk2Commands.hrh>
#include <CPbk2ContactRelocator.h>
#include <CPbk2CommandHandler.h>
#include <CPbk2ApplicationServices.h>
#include <CVPbkContactManager.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2FieldPropertyArray.h>
#include <pbk2mapcommands.hrh>

#include "ccappdetailsviewmenuhandler.h"
#include "ccappdetailsviewlistboxmodel.h"
#include "ccappdetailsviewcontainer.h"
#include "ccappdetailsviewplugin.h"
#include "ccappdetailsviewdefs.h"

#include <mccaparameter.h>

#include <ccappdetailsviewpluginrsc.rsg>
#include "ccappdetailsview.hrh"

#include <mnproviderfinder.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>

using namespace AiwContactAssign;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::NewL
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewMenuHandler* CCCAppDetailsViewMenuHandler::NewL(
    CCCAppDetailsViewPlugin& aPlugin)
{
    CCCAppDetailsViewMenuHandler* self = new(ELeave) CCCAppDetailsViewMenuHandler(aPlugin);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::~CCCAppDetailsViewMenuHandler
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewMenuHandler::~CCCAppDetailsViewMenuHandler()
{
    delete iAiwServiceHandler;
    delete iCommandHandler;
    delete iMapViewProvider;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::CCCAppDetailsViewMenuHandler()
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewMenuHandler::CCCAppDetailsViewMenuHandler(
    CCCAppDetailsViewPlugin& aPlugin)
:
    iPlugin(aPlugin)
{
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::ConstructL()
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewMenuHandler::ConstructL()"));

    iCommandHandler = CPbk2CommandHandler::NewL();

    RPointerArray<CMnProvider> providers;
    CleanupClosePushL( providers );

    MnProviderFinder::FindProvidersL( providers,
        CMnProvider::EServiceMapView );

    if (providers.Count() > 0)
    {
        iMapViewProvider = providers[0];
        providers.Remove(0);
    }

    providers.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &providers );

    PrepareAiwMenusL();

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewMenuHandler::ConstructL()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DynInitMenuPaneL(TInt aResourceId,
    CEikMenuPane* aMenuPane)
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewMenuHandler::DynInitMenuPaneL()"));

    if (iAiwServiceHandler->HandleSubmenuL(*aMenuPane))
    {
        return;
    }

    switch (aResourceId)
    {
    case R_CCAPPDETAILSVIEW_MENUPANE:
    case R_DETAILSVIEW_DETAIL_MENUPANE:
        {
            TInt pos = 0;

            const CCCAppDetailsViewContainer& container =
                static_cast<const CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

            TBool isNumberField  = IsFocusedFieldTypeL(R_DETAILSVIEW_PHONENUMBER_SELECTOR);
            TBool isOkForVoiceCall = IsFocusedFieldTypeL(R_DETAILSVIEW_CALL_SELECTOR);
            TBool isOkForVoipCall = EFalse;  //voip calls not provided 
            TBool isAddressField =
                IsFocusedFieldTypeL(R_DETAILSVIEW_EL_ADDRESS_SELECTOR) ||
                IsFocusedFieldTypeL(R_DETAILSVIEW_URL_SELECTOR);

            if (aMenuPane->MenuItemExists(ECCAppDetailsViewSetTopContactCmd,    pos) &&
                aMenuPane->MenuItemExists(ECCAppDetailsViewRemoveTopContactCmd, pos)
                )
            {
                TBool isTopContact = container.ListBoxModel().IsTopContact();

                aMenuPane->SetItemDimmed(ECCAppDetailsViewSetTopContactCmd,     isTopContact);
                aMenuPane->SetItemDimmed(ECCAppDetailsViewRemoveTopContactCmd, !isTopContact);
            }

            if (aMenuPane->MenuItemExists(ECCAppDetailsViewCopyDetailCmd, pos))
            {
                aMenuPane->SetItemDimmed(ECCAppDetailsViewCopyDetailCmd,
                    (isNumberField || isAddressField));
            }

            if (aMenuPane->MenuItemExists(ECCAppDetailsViewNumberCmd,  pos)/* &&
                aMenuPane->MenuItemExists(ECCAppDetailsViewAddressCmd, pos)*/
                )
            {
                aMenuPane->SetItemDimmed(ECCAppDetailsViewNumberCmd,  !isNumberField);
                // aMenuPane->SetItemDimmed(ECCAppDetailsViewAddressCmd, !isAddressField);
            }
            if (aMenuPane->MenuItemExists(ECCAppDetailsViewAddressCmd,  pos)/* &&
              //  aMenuPane->MenuItemExists(ECCAppDetailsViewAddressCmd, pos)*/
                )
            {
                aMenuPane->SetItemDimmed(ECCAppDetailsViewAddressCmd,  !isAddressField); 
            }
            if (aMenuPane->MenuItemExists(ECCAppDetailsViewCopyNumberCmd, pos))
            {
                aMenuPane->SetItemDimmed(ECCAppDetailsViewCopyNumberCmd, !isNumberField);
            }

            if (aMenuPane->MenuItemExists(ECCAppDetailsViewCopyAddressCmd, pos))
            {
                aMenuPane->SetItemDimmed(ECCAppDetailsViewCopyAddressCmd, !isAddressField);
            }
            
            if (aMenuPane->MenuItemExists(ECCAppDetailsViewImageCmd, pos))
            {
                TBool isImageField = IsFocusedFieldTypeL(R_DETAILSVIEW_IMAGE_FIELDS_SELECTOR);

                aMenuPane->SetItemDimmed(ECCAppDetailsViewImageCmd, !isImageField);
            }

            if (aMenuPane->MenuItemExists(KAiwCmdAssign, pos))
            {
                TBool isReadOnlyContact = container.ListBoxModel().IsReadOnlyContact();
                if (isReadOnlyContact)
                {
                    aMenuPane->SetItemDimmed(KAiwCmdAssign, ETrue);
                    aMenuPane->SetItemDimmed(ECCAppDetailsViewDeleteCmd, ETrue);
                }
            }

            if (aMenuPane->MenuItemExists(ECCAppDetailsViewPrependCmd, pos))
            {
                aMenuPane->SetItemDimmed(ECCAppDetailsViewPrependCmd, !isNumberField);
            }

            if (aMenuPane->MenuItemExists(ECCAppDetailsViewCallCmd, pos))
            {
                aMenuPane->SetItemDimmed(ECCAppDetailsViewCallCmd, !(isOkForVoiceCall || isOkForVoipCall));
            }            

            if ( aMenuPane->MenuItemExists(
                ECCAppDetailsViewFindOnMapCmd, pos )
                && aMenuPane->MenuItemExists(
                    ECCAppDetailsViewShowOnMapCmd, pos ) )
            {
                if ( iMapViewProvider && IsFocusedAddressFieldL())
                {
                    if ( IsAddressValidatedL() )
                    {
                        aMenuPane->SetItemDimmed(
                            ECCAppDetailsViewFindOnMapCmd, ETrue );
                    }
                    else
                    {
                        aMenuPane->SetItemDimmed(
                            ECCAppDetailsViewShowOnMapCmd, ETrue );
                    }
                }
                else
                {
                    aMenuPane->SetItemDimmed(
                        ECCAppDetailsViewFindOnMapCmd, ETrue );
                    aMenuPane->SetItemDimmed(
                        ECCAppDetailsViewShowOnMapCmd, ETrue );
                }
            }

            TBool isSdnContact = container.ListBoxModel().IsSdnContact();

            if( isNumberField && !isSdnContact )
            {
                CPbk2ContactRelocator* contactRelocator = CPbk2ContactRelocator::NewL();
                CleanupStack::PushL( contactRelocator );
                TBool phoneMemoryInConfiguration =
                    contactRelocator->IsPhoneMemoryInConfigurationL();
                CleanupStack::PopAndDestroy();//contactRelocator

                // Weed out phone memory specific commands, if there is no
                // phone memory in configuration
                if (!phoneMemoryInConfiguration)
                {
                    if (aMenuPane->MenuItemExists(ECCAppDetailsViewAssignSpeedDialCmd, pos))
                    {
                        aMenuPane->SetItemDimmed(ECCAppDetailsViewAssignSpeedDialCmd, ETrue);
                    }
                }

                TInt focusIndex = container.ListBoxModel().FocusedFieldIndex();
                TBool hasSpeedDial = container.ListBoxModel().HasSpeedDialL(focusIndex);

                // Filtering is based on the speed dial existence
                if ( !hasSpeedDial )
                {
                    if (aMenuPane->MenuItemExists(ECCAppDetailsViewUnassignSpeedDialCmd, pos))
                    {
                        aMenuPane->SetItemDimmed
                            ( ECCAppDetailsViewUnassignSpeedDialCmd, ETrue );
                    }
                }
                else
                {
                    if (aMenuPane->MenuItemExists(ECCAppDetailsViewAssignSpeedDialCmd, pos))
                    {
                        aMenuPane->SetItemDimmed
                            ( ECCAppDetailsViewAssignSpeedDialCmd, ETrue );
                    }
                }
            }
            else
            {
                if( aMenuPane->MenuItemExists(ECCAppDetailsViewAssignSpeedDialCmd, pos) )
                {
                    aMenuPane->SetItemDimmed( ECCAppDetailsViewAssignSpeedDialCmd, ETrue );
                }

                if( aMenuPane->MenuItemExists(ECCAppDetailsViewUnassignSpeedDialCmd, pos) )
                {
                    aMenuPane->SetItemDimmed( ECCAppDetailsViewUnassignSpeedDialCmd, ETrue );
                }
            }



            if (iAiwServiceHandler->IsAiwMenu(aResourceId))
            {
                iAiwServiceHandler->InitializeMenuPaneL( *aMenuPane,
                    aResourceId, ECCAppDetailsViewAiwBaseCmd,
                    iAiwServiceHandler->InParamListL() );
            }
            break;
        }

    default:
        break;
    }

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewMenuHandler::DynInitMenuPaneL()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::HandleNotifyL
// ---------------------------------------------------------------------------
//
TInt CCCAppDetailsViewMenuHandler::HandleNotifyL(
    TInt aCmdId,
    TInt aEventId,
    CAiwGenericParamList& aEventParamList,
    const CAiwGenericParamList& /*aInParamList*/)
{
    CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewMenuHandler::HandleNotifyL() eventid = %d"), aEventId);

    TInt result = 0;
    TInt index = 0;
    
    const TAiwGenericParam* errParam = aEventParamList.FindFirst(index,
            EGenericParamError);
    
    if (errParam)
        {
        TInt32 err;
        errParam->Value().Get(err);
        if (err == (TInt32)KErrInUse)
            {
            CCoeEnv::Static()->HandleError(KErrInUse);
            }
        }

    if (aCmdId == KAiwCmdAssign)
    {
        if (aEventId == KAiwEventCompleted)
        {
            index = 0;
            const TAiwGenericParam* param = aEventParamList.FindFirst(index,
                EGenericParamContactLinkArray);
            const CCCAppDetailsViewContainer& container =
                static_cast<const CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

            if (param)
            {
                TPtrC8 contactLink = param->Value().AsData();

                CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC(
                    contactLink, container.ContactManager().ContactStoresL() );

                if ( !links->At(0).RefersTo(*(container.ListBoxModel().StoreContact())) )
                {
                    // Update the CCA parameter contact data
                    HBufC16* link16 = HBufC16::NewLC( contactLink.Length() );
                    link16->Des().Copy( contactLink );
                    iPlugin.AppEngine()->Parameter().SetContactDataL(link16->Des());
                    CleanupStack::PopAndDestroy(1); // link16

                    // Inform the CCApp engine of the contact relocation
                    iPlugin.AppEngine()->CCAppContactEventL();

                    // Update the details view UI
                    container.ListBoxModel().SetLinks(links);
                    CleanupStack::Pop(links);
                }
                else
                {
                    CleanupStack::PopAndDestroy(); // links
                }
            }
            index = 0;
            const TAiwGenericParam* indexparam = aEventParamList.FindFirst(index,
                EGenericParamContactItem);

            if (indexparam)
            {
                TInt focus = indexparam->Value().AsTInt32();

                container.ListBoxModel().SetInitialFocusIndex(focus);
            }

            container.DrawDeferred();

        }
        else if (aEventId == KAiwEventCanceled)
        {
            /*
             iControl->ListBoxModel().SetLinks(NULL);
             iControl->DrawDeferred();
             */
        }
        else if (aEventId == KAiwEventOutParamCheck)
        {
        }
        else if (aEventId == KAiwEventQueryExit)
        {
            // Send an exit command in order to close CCA and Phonebook2
            CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi() );

            if ( appUi )
            {
                appUi->ProcessCommandL( EAknCmdExit );
            }

            // allow pbk editor AIW service to close whole app chain
            result = ETrue;
        }
    }

    CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewMenuHandler::HandleNotifyL()"));

    return result;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::Reset
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::Reset()
    {
    if ( iAiwServiceHandler != NULL )
        {
        iAiwServiceHandler->Reset();
        }
    }

// --------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::Activate
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::Activate()
    {
    if ( iAiwServiceHandler != NULL )
        {
        TRAP_IGNORE( iAiwServiceHandler->AttachL(
            R_DETAILSVIEW_AIW_SERVICE_INTERESTS) );
        }
    }

// --------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::IsFocusedFieldTypeL
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewMenuHandler::IsFocusedFieldTypeL(TInt aSelectorResId)
{
    const CCCAppDetailsViewContainer& container =
        static_cast<const CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    TBool ret = container.ListBoxModel().IsFocusedFieldTypeL(aSelectorResId);

    return ret;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::HasContactFieldTypeL
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewMenuHandler::HasContactFieldTypeL(TInt aSelectorResId)
{
    const CCCAppDetailsViewContainer& container =
        static_cast<const CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    TBool ret = container.ListBoxModel().HasContactFieldTypeL(aSelectorResId);

    return ret;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::PrepareAiwMenusL
// Prepares plugin for AIW-menu usage
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::PrepareAiwMenusL()
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewMenuHandler::PrepareAiwMenusL()"));

    // Initialisation of AIW Servicehandler cannot be in
    // ConstructL, since the menus of plugin are given
    // later via ProvideApplicationResourcesL(..)
    iAiwServiceHandler = CAiwServiceHandler::NewL();

    iAiwServiceHandler->AttachL(
        R_DETAILSVIEW_AIW_SERVICE_INTERESTS);

    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdCall, R_DETAILSVIEW_DETAIL_MENUPANE,
            R_DETAILSVIEW_CALLUI_AIW_INTEREST, ETrue );


    /*iAiwServiceHandler->AttachMenuL(
     R_CCACOMMLAUNCHER_MENUPANE,
     R_CCACOMMLAUNCHER_AIW_MENU_INTERESTS );*/

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewMenuHandler::PrepareAiwMenusL()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::HandleCommandL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::HandleCommandL(TInt aCommand)
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewMenuHandler::HandleCommandL()"));

    // CCApp handles the "Exit"- and "Back"-commands

    // Quick guide:
    // - iParameter contains the launching information like contact id/link
    // - iEnabledFields contains the fields contact has available
    // - iContactFieldsArray contains the fields already fetched from pbk

    // Check and handle the Aiw commands 1st
    /*
     TODO: NEEDED?
     TInt aiwCmd = iAiwServiceHandler->ServiceCmdByMenuCmd(aCommand);
     if(aiwCmd)
     {
     DoAiwCommandL(aCommand, aiwCmd);
     return;// aCommand should not match with non-Aiw commands, but just to be sure
     }
     */
    // Then check if command is normal application command
    switch (aCommand)
    {
    case ECCAppDetailsViewCallCmd:
        {
        CCCAppDetailsViewContainer& container =
                static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());        
        container.HandleLaunchCallRequestL();        
        }		
        break;

    case EAknSoftkeyEdit:
    case KAiwCmdAssign: // AIW service cmd for Editing
    case ECCAppDetailsViewEditItemCmd:
        DoEditCmdL( aCommand );
        break;

    case ECCAppDetailsViewImageCmd:
        DoViewImageCmdL();
        break;

    case ECCAppDetailsViewSendBusinessCardCmd:
        DoSendBusinessCardCmdL();
        break;

    case ECCAppDetailsViewSetTopContactCmd:
    case ECCAppDetailsViewRemoveTopContactCmd:
        DoTopContactCmdL();
        break;

    case ECCAppDetailsViewDeleteCmd:
        DoDeleteCmdL();
        break;

    case ECCAppDetailsViewCopyCmd:
        DoCopyCmdL();
        break;

    case ECCAppDetailsViewCopyDetailCmd:
        DoCopyDetailCmdL();
        break;
    case ECCAppDetailsViewCopyNumberCmd:
        DoCopyNumberCmdL();
        break;

    case ECCAppDetailsViewCopyAddressCmd:
        DoCopyAddressCmdL();
        break;
    case ECCAppDetailsViewAssignSpeedDialCmd:
        DoAssignSpeeddialCmdL();
        break;

    case ECCAppDetailsViewUnassignSpeedDialCmd:
        DoUnassignSpeeddialCmdL();
        break;

    case ECCAppDetailsViewSetAsDefaultCmd:
        DoSetDefaultCmdL();
        break;

    case ECCAppDetailsViewPrependCmd:
        DoPrependCmdL();
        break;

    case ECCAppDetailsViewFindOnMapCmd:
    case ECCAppDetailsViewShowOnMapCmd:
        DoShowOnMapCmdL();
        break;

    default:
        break;
    }

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewMenuHandler::HandleCommandL()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoEditCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoEditCmdL(  TInt aCommand )
{
    const CCCAppDetailsViewContainer& container =
        static_cast<const CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

#if 0
    container.ListBoxModel().OpenEditorL();
#else
    CAiwGenericParamList& inParamList = iAiwServiceHandler->InParamListL();

    inParamList.AppendL(TAiwGenericParam(
        EGenericParamContactAssignData,
        TAiwVariant(TAiwSingleContactAssignDataV1Pckg(
            TAiwSingleContactAssignDataV1().SetFlags(EskipInfoNotes)))));

    HBufC8* packedLinks = container.ListBoxModel().Links()->PackLC();

    inParamList.AppendL(TAiwGenericParam(EGenericParamContactLinkArray,
        TAiwVariant( *packedLinks)));
    
    TInt focusIndex = container.ListBoxModel().FocusedFieldIndex();
    
    TBool itemFocused = iPlugin.MenuBar()->ItemSpecificCommandsEnabled();
    TBool isAddressField = EFalse;
    MVPbkBaseContactField* focusedField = container.ListBoxModel().FocusedFieldLC();
    if ( focusedField )
        {
        const MVPbkFieldType* type = focusedField->BestMatchingFieldType();
        const TArray<TVPbkFieldVersitProperty> propArr = type->VersitProperties();
        if(propArr.Count())
            {
            const TVPbkFieldVersitProperty prop = propArr[0];
            if(prop.Name() == EVPbkVersitNameADR)
                {
                isAddressField = ETrue;
                }
            }
        }
    CleanupStack::PopAndDestroy( focusedField );
    
    // this is only for the case, if there is only address field( address field is in first 
    // item in details view), when select "Edit" from options menu, if the item is not focused,
    // contact eidtor view should be opened instead of address editor view.
    // if the address field is focused or or select the list item directly, address editor view
    // should be opened insdead of contact eidtor view.
    // set focusIndex to KErrNotFound in order to open contact editor view.
    if ( isAddressField && !itemFocused && aCommand != ECCAppDetailsViewEditItemCmd )
        {
        focusIndex = KErrNotFound;
        }

    CCA_DP(KDetailsViewLogFile, CCA_L("CCCAppDetailsViewMenuHandler::DoEditCmdL() focusIndex = %d"), focusIndex);

    // TODO: use int instead of buf.
    TBuf<8> focusIndexBuf;
    focusIndexBuf.Num(focusIndex);
    const TAiwVariant focusIndexVariant(focusIndexBuf);

    inParamList.AppendL(TAiwGenericParam(EGenericParamContactItem, focusIndexVariant));

    iAiwServiceHandler->ExecuteServiceCmdL(
        KAiwCmdAssign,
        inParamList,
        iAiwServiceHandler->OutParamListL(),
        NULL,
        this);

    CleanupStack::PopAndDestroy(packedLinks);
#endif
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoViewImageCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoViewImageCmdL()
{
    CAknInformationNote* informationNote = new(ELeave) CAknInformationNote;
    informationNote->ExecuteLD(_L("Viewing image not supported yet"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoSendBusinessCardCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoSendBusinessCardCmdL()
{
    CCCAppDetailsViewContainer& container =
        static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    iCommandHandler->HandleCommandL(EPbk2CmdSend, container, NULL);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoTopContactCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoTopContactCmdL()
{
    CAknInformationNote* informationNote = new(ELeave) CAknInformationNote;
    informationNote->ExecuteLD(_L("Setting/dropping as top contact not supported yet"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoDeleteCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoDeleteCmdL()
{
    CCCAppDetailsViewContainer& container =
        static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    iCommandHandler->HandleCommandL(EPbk2CmdDeleteMe, container, NULL);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoCopyCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoCopyCmdL()
{
    CCCAppDetailsViewContainer& container =
        static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    iCommandHandler->HandleCommandL(EPbk2CmdCopy, container, NULL);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoCopyDetailCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoCopyDetailCmdL()
{
    //EPbk2CmdCopyDetail
    CCCAppDetailsViewContainer& container =
            static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

        iCommandHandler->HandleCommandL(EPbk2CmdCopyDetail, container, NULL);
           
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoCopyNumberCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoCopyNumberCmdL()
{
    //EPbk2CmdCopyNumber
    CCCAppDetailsViewContainer& container =
            static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

        iCommandHandler->HandleCommandL(EPbk2CmdCopyNumber, container, NULL);
           
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoCopyDetailCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoCopyAddressCmdL()
{
    //EPbk2CmdCopyAddress
	
	CCCAppDetailsViewContainer& container =
            static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

        iCommandHandler->HandleCommandL(EPbk2CmdCopyAddress, container, NULL);
           
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoAssignSpeeddialCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoAssignSpeeddialCmdL()
{
    CCCAppDetailsViewContainer& container =
        static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    iCommandHandler->HandleCommandL( EPbk2CmdAssignSpeedDial, container, NULL );
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoUnassignSpeeddialCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoUnassignSpeeddialCmdL()
{
    CCCAppDetailsViewContainer& container =
        static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    iCommandHandler->HandleCommandL( EPbk2CmdRemoveSpeedDial, container, NULL );
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoSetDefaultCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoSetDefaultCmdL()
{
    CAknInformationNote* informationNote = new(ELeave) CAknInformationNote;
    informationNote->ExecuteLD(_L("Default not supported yet"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoPrependCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoPrependCmdL()
{
    CCCAppDetailsViewContainer& container =
        static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());
    iCommandHandler->HandleCommandL(EPbk2CmdPrepend, container, NULL);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::DoShowOnMapCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewMenuHandler::DoShowOnMapCmdL()
{
    CCCAppDetailsViewContainer& container =
        static_cast<CCCAppDetailsViewContainer&>(iPlugin.GetContainer());

    iCommandHandler->HandleCommandL(
        EPbk2ExtensionShowOnMap, container, NULL);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::IsFocusedAddressFieldL
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewMenuHandler::IsFocusedAddressFieldL()
{
    TBool addressField = EFalse;

    const CCCAppDetailsViewContainer& container =
        static_cast<const CCCAppDetailsViewContainer&>( iPlugin.GetContainer() );

    MVPbkBaseContactField* field =  container.ListBoxModel().FocusedFieldLC();
    const MVPbkFieldType* fieldType = field->BestMatchingFieldType();
    if ( fieldType )
        {
        TInt countProps = fieldType->VersitProperties().Count();
        TArray<TVPbkFieldVersitProperty> props =
            fieldType->VersitProperties();
        for (TInt ii = 0; ii < countProps; ii++ )
            {
            if ( props[ii].Name() == EVPbkVersitNameADR )
                {
                addressField = ETrue;
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy(field);
    return addressField;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewMenuHandler::IsAddressValidatedL
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewMenuHandler::IsAddressValidatedL()
{
    TBool generalAddress = ETrue;
    TBool homeAddress = ETrue;
    TBool workAddress = ETrue;
    TBool geoField = EFalse;
    TVPbkFieldTypeParameter focusedField = EVPbkVersitParamOther;

    if ( IsFocusedFieldTypeL( R_DETAILSVIEW_HOME_ADDRESS_SELECTOR ) )
    {
        focusedField = EVPbkVersitParamHOME;
    }
    else if ( IsFocusedFieldTypeL( R_DETAILSVIEW_WORK_ADDRESS_SELECTOR ) )
    {
        focusedField = EVPbkVersitParamWORK;
    }
    else if ( IsFocusedFieldTypeL( R_DETAILSVIEW_GENERAL_ADDRESS_SELECTOR ) )
    {
        focusedField = EVPbkVersitParamPREF;
    }

    const CCCAppDetailsViewContainer& container =
        static_cast<const CCCAppDetailsViewContainer&>( iPlugin.GetContainer() );
    TInt countFields =
        container.FocusedStoreContact()->Fields().FieldCount();

    if ( focusedField == EVPbkVersitParamOther )
    {
        for ( TInt i = 0; i < countFields; i++ )
        {
            const MVPbkStoreContactField& field =
                container.FocusedStoreContact()->Fields().FieldAt( i );
            TInt countProps =
                field.BestMatchingFieldType()->VersitProperties().Count();
            TArray<TVPbkFieldVersitProperty> props =
                field.BestMatchingFieldType()->VersitProperties();
            for ( TInt ii = 0; ii < countProps; ii++ )
            {
                if ( props[ ii ].Name() == EVPbkVersitNameADR )
                {
                    if ( props[ ii ].Parameters().Contains(
                        EVPbkVersitParamHOME ) )
                    {
                        homeAddress = EFalse;
                    }
                    else if ( props[ ii ].Parameters().Contains(
                        EVPbkVersitParamWORK ) )
                    {
                        workAddress = EFalse;
                    }
                    else
                    {
                        generalAddress = EFalse;
                    }
                }
            }
        }
    }

    for ( TInt i = 0; i < countFields; i++ )
    {
        const MVPbkStoreContactField& field =
            container.FocusedStoreContact()->Fields().FieldAt( i );
        TInt countProps =
            field.BestMatchingFieldType()->VersitProperties().Count();
        TArray<TVPbkFieldVersitProperty> props =
            field.BestMatchingFieldType()->VersitProperties();
        for ( TInt ii = 0; ii < countProps; ii++ )
        {
            if ( props[ ii ].Name() == EVPbkVersitNameGEO )
            {
                if ( props[ ii ].Parameters().Contains(
                    EVPbkVersitParamHOME ) )
                {
                    if ( focusedField == EVPbkVersitParamHOME )
                    {
                        return ETrue;
                    }
                    homeAddress = ETrue;
                }
                else if ( props[ ii ].Parameters().Contains(
                    EVPbkVersitParamWORK ) )
                {
                    if ( focusedField == EVPbkVersitParamWORK )
                    {
                        return ETrue;
                    }
                    workAddress = ETrue;
                }
                else
                {
                    if ( focusedField == EVPbkVersitParamPREF )
                    {
                        return ETrue;
                    }
                    generalAddress = ETrue;
                }
            }
        }
    }

    if ( focusedField == EVPbkVersitParamOther
        && generalAddress && homeAddress && workAddress )
    {
        geoField = ETrue;
    }

    return geoField;
}

// End of File
