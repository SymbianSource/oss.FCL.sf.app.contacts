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
* Description:  Implementation of details view plugin
*  Version     : %version: be1s60pr#23.1.1.2.18 %
*
*/


#include <bautils.h>
#include <AknIconUtils.h>
#include <AiwServiceHandler.h>
#include <AknsUtils.h>
#include <aknViewAppUi.h>

#include <mccapppluginparameter.h>
#include "ccaparameter.h"
#include "mccaparameter.h"

#include <CVPbkContactManager.h>
#include <CPbk2StoreConfiguration.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactLinkArray.h>
#include <CPbk2ApplicationServices.h>
#include <CPbk2StoreManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkContactStoreUris.h>
#include <mccappengine.h>
#include <ccacontactorservice.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStore.h>

#include <Pbk2DataCaging.hrh>

#include "ccappdetailsviewplugin.h"
#include "ccappdetailsviewlistboxmodel.h"
#include "ccappdetailsviewcontainer.h"
#include "ccappdetailsviewmenuhandler.h"
#include "ccappdetailsviewdefs.h"
#include "ccappdetailsviewpluginuids.hrh"
#include "ccappdetailsview.hrh"

#include <ccappdetailsviewpluginrsc.rsg>
#include <ccappdetailsviewplugin.mbg>

#include <CVPbkContactManager.h>

// cms engine
#include "cmscontactfield.h"
#include "cmscontactfieldinfo.h"
#include "cmscontactfielditem.h"
#include "cmssession.h"
#include "cmscontact.h"
#include "ccacontactorservice.h"
#include <VPbkFieldTypeSelectorFactory.h>
#include "cmspresencedata.h"

#include "ccacmscontactfetcherwrapper.h"

// Consts
_LIT(KCCAppDetailsViewResourceFileName, "\\resource\\ccappdetailsviewpluginrsc.rsc");
_LIT(KCCAppDetailsViewIconFileName,     "\\resource\\apps\\ccappdetailsviewplugin.mif");

_LIT(KPbk2CommandsDllResFileName,   "Pbk2Commands.rsc");
_LIT(KPbk2UiControlsDllResFileName, "Pbk2UiControls.rsc");
_LIT(KPbk2CommonUiDllResFileName,   "Pbk2CommonUi.rsc"  );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::NewL
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewPlugin* CCCAppDetailsViewPlugin::NewL()
{
    CCCAppDetailsViewPlugin* self = new(ELeave) CCCAppDetailsViewPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::~CCCAppDetailsViewPlugin
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewPlugin::~CCCAppDetailsViewPlugin()
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewPlugin::~CCCAppDetailsViewPlugin()"));
    
    //Delete the Contact Link
    delete iLink;

    //BG task for checking the visibility of this plugin
    if (iBgTaskForCheckingVisibility)
    {
        iBgTaskForCheckingVisibility->Cancel();
        delete iBgTaskForCheckingVisibility;
    }

    if (AppEngine())
    {
        AppEngine()->RemoveObserver(*this);
    }

    if (iAppServices)
    {
        iAppServices->StoreManager().DeregisterStoreEvents(*this);
    }

    delete iMenuHandler;
    delete iContactorService;

    Release(iAppServices);

    iCommandsResourceFile.Close();
    iUiControlsResourceFile.Close();
    iCommonUiResourceFile.Close();

    if(iCmsWrapper)
        {
        iCmsWrapper->Release();
        }

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewPlugin::~CCCAppDetailsViewPlugin()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::CCCAppDetailsViewPlugin()
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewPlugin::CCCAppDetailsViewPlugin() :
    iFocusedListIndex(0),
    iCommandsResourceFile(*iCoeEnv),
    iUiControlsResourceFile(*iCoeEnv),
    iCommonUiResourceFile(*iCoeEnv),
    iVisible(EFalse) // By default this plugin is not visible
{
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::ConstructL()
{
    CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewPlugin::ConstructL()"));

    // ConstructL should be as light as possible.

    /*
    CPbk2ApplicationServices& pbk2AppServices = GetPbk2AppServicesL();
    // Add phone memory to cnt manager loaded stores even if current configuration
    // contains only SIM memory so that top contacts can be opened
    // from e.g. home screen widget.
    pbk2AppServices.ContactManager().LoadContactStoreL(
        VPbkContactStoreUris::DefaultCntDbUri() );
    */

    iCmsWrapper = CCCAppCmsContactFetcherWrapper::InstanceL(); 
    // Create the BG Task which will help us in checking the visibility of this plugin
    iBgTaskForCheckingVisibility = CIdle::NewL(CActive::EPriorityStandard);

    CCA_DP(KDetailsViewLogFile, CCA_L("iFocusedListIndex = %d"), iFocusedListIndex);

    CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewPlugin::ConstructL()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::PreparePluginViewL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::PreparePluginViewL(
    MCCAppPluginParameter& /*aPluginParameter*/)
{

    PrepareViewResourcesL();

    // Get the PBK2AppServices
    CPbk2ApplicationServices& pbk2AppServices = GetPbk2AppServicesL();

    if (AppEngine())
    {
        AppEngine()->AddObserverL(*this);
    }
    CPbk2StoreManager& storeManager = pbk2AppServices.StoreManager();
    storeManager.RegisterStoreEventsL(*this);
    storeManager.OpenStoresL();

    //Get the Contact Link
    HBufC& contactData = AppEngine()->Parameter().ContactDataL();
    HBufC8* contactData8 = HBufC8::NewLC( contactData.Size() );
    TPtr8 contactData8Ptr( contactData8->Des() );
    contactData8Ptr.Copy( contactData.Des() ); 
    CVPbkContactManager* vPbkContactManager = &iAppServices->ContactManager();
    
    if( vPbkContactManager )
    {
        MVPbkContactLinkArray* contactArray = NULL;        
        contactArray = vPbkContactManager->CreateLinksLC( contactData8Ptr );
        if( contactArray->Count() > 0 )
        {
            iLink = contactArray->At( 0 ).CloneLC();
            CleanupStack::Pop(); //link
        }  
        if( contactArray )
        {
            CleanupStack::PopAndDestroy(); // contactArray
        }
    }  
    CleanupStack::PopAndDestroy(); // contactData8  
    
    iSchedulerWait.Start();

    BaseConstructL(R_CCAPPDETAILSVIEW_MAINVIEW);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::ProvideBitmapL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::ProvideBitmapL(
    TCCAppIconType aIconType, CAknIcon& aIcon)
{
    if (ECCAppTabIcon == aIconType)
    {
        CFbsBitmap* bmp = NULL;
        CFbsBitmap* bmpMask = NULL;

        AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDDefault,//todo; get a proper skin
            bmp,
            bmpMask,
            KCCAppDetailsViewIconFileName,
            EMbmCcappdetailsviewpluginQgn_prop_contact_detail_tab2,
            EMbmCcappdetailsviewpluginQgn_prop_contact_detail_tab2_mask);

        aIcon.SetBitmap(bmp);
        aIcon.SetMask(bmpMask);
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::NewContainerL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::NewContainerL()
{
    iContainer = new(ELeave) CCCAppDetailsViewContainer(Id(), *this);

    // Get the PBK2AppServices
    CPbk2ApplicationServices& pbk2AppServices = GetPbk2AppServicesL();

    static_cast<CCCAppDetailsViewContainer*>
        (iContainer)->SetApplicationServices(&pbk2AppServices);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::HandleStatusPaneSizeChange()
{
    // Resize the container to fill the client rectangle
    if (iContainer)
    {
        iContainer->SetRect(ClientRect());
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::DynInitMenuPaneL(TInt aResourceId,
    CEikMenuPane* aMenuPane)
{
    EnsureMenuHandlerCreatedL();
    iMenuHandler->DynInitMenuPaneL(aResourceId, aMenuPane);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::EnsureMenuHandlerCreatedL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::EnsureMenuHandlerCreatedL()
{
    if (!iMenuHandler)
    {
        iMenuHandler = CCCAppDetailsViewMenuHandler::NewL(*this);
        iMenuHandler->SetAppEngine(AppEngine());
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::HandleEditRequestL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::HandleEditRequestL()
{
    /* Note: We do not need to check if contact is editable, as this is called
     * from CCCAppDetailsViewListBoxModel::HandleEditRequestL(), which checks
     * if the contact is editable or not.
     */
    EnsureMenuHandlerCreatedL();
    iMenuHandler->HandleCommandL(ECCAppDetailsViewEditItemCmd);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::HandleDeleteRequestL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::HandleDeleteRequestL()
{
    /* Note: We do not need to check if contact is editable, as this is called
     * from CCCAppDetailsViewListBoxModel::HandleDeleteRequestL(), which checks
     * if the contact is editable or not.
     */
    EnsureMenuHandlerCreatedL();
    iMenuHandler->HandleCommandL(ECCAppDetailsViewDeleteCmd);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::HandleCommandL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::HandleCommandL(TInt aCommand)
{
    // CCApp handles the "Exit"- and "Back"-commands,
    // rest to menuhandler
    CCCAppViewPluginAknView::HandleCommandL(aCommand);

    if (iContainer)
    {
        iFocusedListIndex = static_cast<CCCAppDetailsViewContainer*>
            (iContainer)->ListBoxModel().FocusedListIndex();
    }

    // Rest to menuhandler
    EnsureMenuHandlerCreatedL();
    iMenuHandler->HandleCommandL(aCommand);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::Id
// ---------------------------------------------------------------------------
//
TUid CCCAppDetailsViewPlugin::Id() const
{
    return TUid::Uid(KCCADetailsViewPluginImplmentationUid);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::PrepareViewResourcesL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::PrepareViewResourcesL()
{
    // preparing resources for use
    TFileName fileName(KCCAppDetailsViewResourceFileName);
    BaflUtils::NearestLanguageFile(iCoeEnv->FsSession(), fileName);
    iResourceLoader.OpenL(fileName);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::DoActivateL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage)
{
    CCCAppViewPluginAknView::DoActivateL(aPrevViewId, aCustomMessageId,
        aCustomMessage);

    GetContactDataL();

    static_cast<CCCAppDetailsViewContainer*>
        (iContainer)->SetAppEngine(AppEngine());

    if ( iMenuHandler != NULL )
        {
        iMenuHandler->Activate();
        }
    
    if ( iContactorService == NULL )
        {
        iContactorService = CCAContactorService::NewL();
        }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::DoDeactivate
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::DoDeactivate()
{
    if (iContainer)
    {
        iFocusedListIndex = static_cast<CCCAppDetailsViewContainer*>
            (iContainer)->ListBoxModel().FocusedListIndex();
    }

    if ( iMenuHandler != NULL )
        {
        iMenuHandler->Reset();
        }

    CCCAppViewPluginAknView::DoDeactivate();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::GetContactDataL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::GetContactDataL()
{
    CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewPlugin::GetContactDataL()"));

    if (iContainer)    
    {        
        // Get the PBK2AppServices
        CPbk2ApplicationServices& pbk2AppServices = GetPbk2AppServicesL();

        HBufC8* contactLinkArrayDes = iCmsWrapper->ContactIdentifierLC();        
        CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC(*contactLinkArrayDes,
            pbk2AppServices.ContactManager().ContactStoresL());

        CCA_DP(KDetailsViewLogFile, CCA_L("iFocusedListIndex = %d"), iFocusedListIndex);
        
        
        static_cast<CCCAppDetailsViewContainer*>
            (iContainer)->ListBoxModel().SetLinks(links); // takes ownership of links.
        CleanupStack::Pop(links); 
        CleanupStack::PopAndDestroy(contactLinkArrayDes);
        
        static_cast<CCCAppDetailsViewContainer*>
            (iContainer)->ListBoxModel().SetInitialFocusIndex(iFocusedListIndex);
        iContainer->DrawDeferred();
    }

    CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewPlugin::GetContactDataL()"));
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::OpenComplete
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::OpenComplete()
{
    if (iSchedulerWait.IsStarted())
    {
        iSchedulerWait.AsyncStop();
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::StoreReady
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::StoreReady(MVPbkContactStore& aContactStore)
{
    if( iLink )
    {
        const MVPbkContactStoreProperties& storeProperties = 
                            iLink->ContactStore().StoreProperties();
        TVPbkContactStoreUriPtr uri = storeProperties.Uri();
        if( 0 == uri.Compare( aContactStore.StoreProperties().Uri(), 
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) )
        {
            delete iLink;
            iLink = NULL;
            if ( iSchedulerWait.IsStarted() )
            {
                iSchedulerWait.AsyncStop();
            }
        }
    }
    else
    {
        //We shouldnt get here
        //but if so, lets be safe
        if ( iSchedulerWait.IsStarted() )
        {
            iSchedulerWait.AsyncStop();
        }
    }
    
    
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::StoreUnavailable
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::StoreUnavailable(
    MVPbkContactStore& aContactStore,
    TInt /*aReason*/)
{
    if( iLink )
        {
        TVPbkContactStoreUriPtr linkuri = 
                iLink->ContactStore().StoreProperties().Uri();
        
        TVPbkContactStoreUriPtr uri = aContactStore.StoreProperties().Uri();
        
        if( linkuri.Compare( uri, 
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) == 0 )
            {
            /*
             * When the end user is selecting a phone contact, the SIM store 
             * will be opened during opening phone stores, and this function is 
             * called if the SIM store is unavailable.However the scheduler wait
             * object can not stop during the operation (phone contact 
             * selecting), since the program is keeping on waiting for the 
             * other call backs coming.
             */
            if (iSchedulerWait.IsStarted())
                {
                iSchedulerWait.AsyncStop();
                }
            }
        }
    else
        {
        if (iSchedulerWait.IsStarted())
            {
            iSchedulerWait.AsyncStop();
            }
        }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::HandleStoreEventL(
    MVPbkContactStore& /*aContactStore*/,
    TVPbkContactStoreEvent aStoreEvent)
{
    if (iSchedulerWait.IsStarted())
    {
        iSchedulerWait.AsyncStop();
    }

    switch (aStoreEvent.iEventType)
    {
    case TVPbkContactStoreEvent::EContactChanged:
        GetContactDataL();
        break;

    case TVPbkContactStoreEvent::EContactDeleted:
        break;

    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::SetTitleL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::SetTitleL(const TDesC& aTitle)
{
    // Forward the SetTitleL-call to base-class
    CCCAppViewPluginAknView::SetTitleL(aTitle);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::CheckVisibilityL
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewPlugin::CheckVisibilityL(MCCAppPluginParameter& /*aPluginParameter*/)
{    
    // Start checking the visibility of this plugin in the background
    CheckVisibilityInBackgroundL();
    return iVisible;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::CCCAppViewPluginBaseExtension
// ---------------------------------------------------------------------------
//
TAny* CCCAppDetailsViewPlugin::CCCAppViewPluginBaseExtension( TUid aExtensionUid )
    {
    if ( aExtensionUid == KMCCAppViewPluginBaseExtension2Uid )
        {
        return static_cast<MCCAppViewPluginBase2*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::UpdateMSKinCbaL
// Sets CBA with empty MSK or MSK with text "Edit" when contact is editable.
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::UpdateMSKinCbaL(TBool aEditable)
{
    CEikButtonGroupContainer* cba = Cba();

    if (cba)
    {
        cba->SetCommandSetL(aEditable
            ? R_CCAPPDETAILSVIEW_SOFTKEYS_OPTIONS_BACK_EDIT
            : R_CCAPPDETAILSVIEW_SOFTKEYS_OPTIONS_BACK_EMPTY);

        cba->DrawDeferred();
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::CheckValidityOfContactInfoL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::CheckValidityOfContactInfoL()
{
   TBool validContact = EFalse;   
   HBufC8* contactLinkArrayDes = iCmsWrapper->ContactIdentifierLC();   

   if (contactLinkArrayDes)
   {
       // Get the PBK2AppServices
       CPbk2ApplicationServices& pbk2AppServices = GetPbk2AppServicesL();
       CVPbkContactLinkArray* contactLinkArray = CVPbkContactLinkArray::NewLC(*contactLinkArrayDes,
           pbk2AppServices.ContactManager().ContactStoresL());       
       if (contactLinkArray->Count())
       {
           // Valid contact
           validContact = ETrue;           
       }       
       CleanupStack::PopAndDestroy(contactLinkArray);
   }
   CleanupStack::PopAndDestroy(contactLinkArrayDes);   
   
    if (validContact)
    {
        MakeMeVisibleL();
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::CheckVisibilityInBackgroundL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::CheckVisibilityInBackgroundL()
{
    // Start checking the visibility of this plugin in the background
    if (!iBgTaskForCheckingVisibility->IsActive())
    {
        iBgTaskForCheckingVisibility->Start(TCallBack(CheckVisibilityCallbackL , this));
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::CheckVisibilityCallbackL
// ---------------------------------------------------------------------------
//

TInt CCCAppDetailsViewPlugin::CheckVisibilityCallbackL(TAny* aContainer)
{
    CCCAppDetailsViewPlugin* container = static_cast<CCCAppDetailsViewPlugin*>(aContainer);
    container->CheckValidityOfContactInfoL();
    return KErrNone;
}


// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::MakeMeVisibleL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewPlugin::MakeMeVisibleL()
{
    // Set this plugin as visible
    iVisible = ETrue;
    // Call the below API to make yourself visible in the CCA Tabs
    ShowPluginL();
}


// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::GetPbk2AppServicesL
// ---------------------------------------------------------------------------
//
CPbk2ApplicationServices& CCCAppDetailsViewPlugin::GetPbk2AppServicesL()
{
    // create only if iAppServices is NULL
    if (!iAppServices)
    {
        //Required for creating the instance of CPbk2ApplicationServices
        iCommandsResourceFile.OpenL(
            KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommandsDllResFileName);
        iUiControlsResourceFile.OpenL(
            KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2UiControlsDllResFileName);
        iCommonUiResourceFile.OpenL(
            KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommonUiDllResFileName);

        iAppServices = CPbk2ApplicationServices::InstanceL();
    }

    return *iAppServices;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::NotifyPluginOfContactEventL
// ---------------------------------------------------------------------------
//
void  CCCAppDetailsViewPlugin::NotifyPluginOfContactEventL()
{
    //If a contact has e.g. relocated from sim to phone memory we need to
	//continue using relocated contact instead of old
    iCmsWrapper->RefetchContactL();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::ContactorService()
// ---------------------------------------------------------------------------
//
CCAContactorService* CCCAppDetailsViewPlugin::ContactorService()
    {
    return iContactorService;
    }

// ---------------------------------------------------------------------------
// CCCAppDetailsViewPlugin::PluginBusy()
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewPlugin::PluginBusy()
    {
    TBool ret = EFalse;
    
    if ( iContactorService != NULL )
       {
       ret = iContactorService->IsBusy();
       }
    return ret;
    }


// End of File
