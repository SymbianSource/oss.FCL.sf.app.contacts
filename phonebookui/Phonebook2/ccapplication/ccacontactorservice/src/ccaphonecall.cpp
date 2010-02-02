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
* Description:  Launching Call.
 *
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

_LIT8( KAiwContentTypeAll,                           "*" );

/// Unnamed namespace for local definitions
namespace
	{

#ifdef _DEBUG
	enum TPanicCode
		{
		EPanicPreCond_ExecuteL = 1,
		EPanicPreCond_ExecuteL2
		};

	void Panic(TPanicCode aReason)
		{
		_LIT(KPanicText, "CCmsPhoneCall");
		User::Panic (KPanicText, aReason);
		}
#endif // _DEBUG


	} /// namespace

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CCCAPhoneCall::ExecuteL
// ---------------------------------------------------------
void CCCAPhoneCall::ExecuteL( const TDesC& aPhoneNumber, TCCAPhoneCallType aCallType, TUint32 aServiceId)
	{
    
    __ASSERT_DEBUG (NULL != &aPhoneNumber && 0 < aPhoneNumber.Size(), Panic (EPanicPreCond_ExecuteL));

#ifdef _DEBUG
	HBufC8* buf = HBufC8::NewLC ( aPhoneNumber.Length ());
	buf->Des().Copy ( aPhoneNumber);
	TPtrC8 numberDesc( buf->Des ());

	CleanupStack::PopAndDestroy (); //buf
#endif

	//aPhoneNumber will be an xsp id of format "ServiceName:UserId"
	//here we need to truncate the servicename and pass only the UserId
	
    //Find if aPhoneNumber of type "ServiceName:UserId"
	//if true, then remove the service part
    TInt phoneNumberStart = aPhoneNumber.Locate(':');
    HBufC* numBuf = NULL;
    
    if ( KErrNotFound != phoneNumberStart )
        {
        phoneNumberStart++; //To get rid of ':'
        numBuf = aPhoneNumber.Right( 
                                aPhoneNumber.Length() - phoneNumberStart 
                                ).AllocLC();
        }
    else
        {
        numBuf = aPhoneNumber.AllocLC();
        }
    
    TPtr numDesc( numBuf->Des ());
	CommonPhoneParser::ParsePhoneNumber ( numDesc,
			CommonPhoneParser::EPhoneClientNumber); 

	CAiwDialDataExt* dialDataExt = CAiwDialDataExt::NewLC ();
	dialDataExt->SetPhoneNumberL ( numBuf->Des ());
	switch (aCallType)
	    {
	    case ECCACallTypeVoice:
	        dialDataExt->SetCallType ( CAiwDialData::EAIWForcedCS);
	        break;
	    case ECCACallTypeVoIP:
	        dialDataExt->SetServiceId(aServiceId);
	        dialDataExt->SetCallType ( CAiwDialData::EAIWVoiP);
	        break;
	    case ECCACallTypeVideo:
	        dialDataExt->SetCallType ( CAiwDialData::EAIWForcedVideo);
	        break;
	    default:
	        dialDataExt->SetCallType ( CAiwDialData::EAIWForcedCS);
	        break;
	    }

	dialDataExt->SetWindowGroup ( CCoeEnv::Static()->RootWin().Identifier ());

	DoAIWCallL (*dialDataExt);

	CleanupStack::PopAndDestroy (dialDataExt);

	CleanupStack::PopAndDestroy (numBuf);

	return;
	}

// ---------------------------------------------------------
// CCCAPhoneCall::DoAIWCallL
// ---------------------------------------------------------
void CCCAPhoneCall::DoAIWCallL( CAiwDialDataExt& aDialData)
	{

	RCriteriaArray interest;
	CleanupClosePushL ( interest);
	CAiwCriteriaItem* criteria = CAiwCriteriaItem::NewLC ( KAiwCmdCall,
			KAiwCmdCall, KAiwContentTypeAll);
	criteria->SetServiceClass ( TUid::Uid (KAiwClassBase));
	User::LeaveIfError ( interest.Append ( criteria));

	CAiwServiceHandler* serviceHandler = CAiwServiceHandler::NewLC ();
	serviceHandler->AttachL ( interest);

	CAiwGenericParamList& paramList = serviceHandler->InParamListL ();
	aDialData.FillInParamListL ( paramList);

	serviceHandler->ExecuteServiceCmdL ( KAiwCmdCall, paramList,
			serviceHandler->OutParamListL (), 0, NULL);
	serviceHandler->DetachL ( interest);

	CleanupStack::PopAndDestroy ( serviceHandler);
	CleanupStack::PopAndDestroy ( criteria);
	CleanupStack::PopAndDestroy (); //interest

	return;
	}

//  End of File  
