/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Launching Editors.
 *
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

// CONSTANTS
const TUid KShareOnline = { 0x200009D5 }; // From SendUiInternalConsts.h
const TUid KCmail = { 0x2001F406 }; // From sf/app/commonemail/meetingrequest/
                                    // inc/esmrinternaluid.h

/// Unnamed namespace for local definitions
namespace
	{

#ifdef _DEBUG
	enum TPanicCode
		{
		EPanicPreCond_ExecuteL = 1
		};

	void Panic(TPanicCode aReason)
		{
		_LIT(KPanicText, "CCmsMsgEditors");
		User::Panic (KPanicText, aReason);
		}
#endif // _DEBUG


	} /// namespace


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CCCAMsgEditors::ExecuteL
// ---------------------------------------------------------
void CCCAMsgEditors::ExecuteL( const TUid aServiceUid,
		const TDesC& aAddress, const TDesC& aName)
	{
    __ASSERT_DEBUG (NULL != &aAddress && 0 < aAddress.Size() &&
                        NULL != &aName && 0 < aName.Size() && 
                        NULL != &aServiceUid, Panic (EPanicPreCond_ExecuteL));

#ifdef _DEBUG
	HBufC8* buf = HBufC8::NewLC ( aAddress.Length ());
	HBufC8* buf2 = HBufC8::NewLC ( aName.Length ());
	buf->Des().Copy ( aAddress);
	TPtrC8 numberDesc( buf->Des ());
	buf2->Des().Copy ( aName);
	TPtrC8 nameDesc( buf2->Des ());

	CleanupStack::PopAndDestroy (2); //buf, buf2
#endif

	//ParsePhoneNumber may only short length of phonenumber, 
	//so it's safe to use same lenght as given number and do the copy.
	HBufC* numBuf = HBufC::NewLC ( aAddress.Length ());	
	numBuf->Des().Append (aAddress);
	TPtr numDesc( numBuf->Des ());

	//Unieditor could also use email adresses. If parser returns EFalse, do not care. 
	if ( KSenduiMtmUniMessageUid == aServiceUid)
		{
		CommonPhoneParser::ParsePhoneNumber ( numDesc,
				CommonPhoneParser::EPhoneClientNumber);
		}

	CSendUi* sendui = CSendUi::NewLC ();
	CMessageData* msgdata = CMessageData::NewLC ();
	msgdata->AppendToAddressL ( numBuf->Des (), aName);
	
	if ( KSenduiMtmSmtpUid == aServiceUid )
	    {
	    RPointerArray<CSendingServiceInfo> availableServices;
	    sendui->AvailableServicesL( availableServices, KGenericMtmPlugin );
	    
	    // Filter unrelated services
	    PrepareServicesList( availableServices );
	    
	    TInt availableServicesCount = availableServices.Count();
	    TUid singleServiceID = KNullUid;
	    if ( availableServicesCount == 1 )
	        {
	        // keep ID of the only available e-mail service
	        singleServiceID = availableServices[0]->ServiceId();
	        }
	    availableServices.ResetAndDestroy();

	    if ( availableServicesCount > 1 )
	        {
    	    CArrayFixFlat<TUid>* servicesToDim = new (ELeave) CArrayFixFlat<TUid>(1);
    	    CleanupStack::PushL(servicesToDim);
    	    
    	    servicesToDim->AppendL(KSenduiTechnologySmsUid);
    	    servicesToDim->AppendL(KSenduiTechnologyMmsUid);
     	    servicesToDim->AppendL(KSenduiTechnologyUniEditorUid);
    	    servicesToDim->AppendL(KSenduiTechnologyIrUid);
    	    servicesToDim->AppendL(KSenduiTechnologyBtUid);
    	    servicesToDim->AppendL(KSenduiTechnologyIMUid);	    
    	    servicesToDim->AppendL(KMmsDirectUpload);
    	    servicesToDim->AppendL(KMmsIndirectUpload);
    	    servicesToDim->AppendL(KMmsUploadService1Id);
    	    servicesToDim->AppendL(KMmsUploadService2Id);
    	    servicesToDim->AppendL(KMmsUploadService3Id);
    	    servicesToDim->AppendL(KMmsUploadService4Id);
    	    servicesToDim->AppendL(KShareOnline);
    	    
    	    sendui->ShowQueryAndSendL( msgdata, KCapabilitiesForAllServices,
    	        servicesToDim );
    	    servicesToDim->Reset();
    	    CleanupStack::PopAndDestroy( servicesToDim );
	        }
	    else if ( availableServicesCount == 1 )
            {
            // start the only available e-mail service
            sendui->CreateAndSendMessageL( singleServiceID, msgdata, KNullUid, ETrue );
            }
        else // Count == 0
            {
            // no e-mail services available, start with default e-mail UID
            // so user will be prompted to create new standard e-mail account
            sendui->CreateAndSendMessageL( KSenduiMtmSmtpUid, msgdata, KNullUid, ETrue );
            }
	    }
	else
	    {
	    sendui->CreateAndSendMessageL( aServiceUid, msgdata, KNullUid, ETrue );
	    }

	CleanupStack::PopAndDestroy ( 3, numBuf); //msgdata, sendui

	return;
	}

// ---------------------------------------------------------
// CCCAMsgEditors::PrepareServicesList
// ---------------------------------------------------------
void CCCAMsgEditors::PrepareServicesList( RPointerArray<CSendingServiceInfo>& aServiceList )
    {
    for ( TInt i = 0; i < aServiceList.Count(); i++ )
        {
        TUid serviceId = aServiceList[i]->ServiceId();
        if ( serviceId == KSenduiMtmSmsUid ||
             serviceId == KSenduiMtmMmsUid ||
             serviceId == KSenduiMtmIrUid ||
             serviceId == KSenduiMtmBtUid ||
             serviceId == KSenduiMtmSyncMLEmailUid ||
             serviceId == KSenduiMtmAudioMessageUid ||
             serviceId == KSenduiMtmUniMessageUid ||
             serviceId == KMmsDirectUpload ||
             serviceId == KMmsIndirectUpload ||
             serviceId == KMmsUploadService1Id ||
             serviceId == KMmsUploadService2Id ||
             serviceId == KMmsUploadService3Id ||
             serviceId == KMmsUploadService4Id ||
             serviceId == KShareOnline ||
             serviceId == KCmail )
            {
            CSendingServiceInfo* serviceInfo = aServiceList[i];
            delete serviceInfo;
            serviceInfo = NULL;
            aServiceList.Remove(i);
            i--;
            }
        }
    }

//  End of File  
