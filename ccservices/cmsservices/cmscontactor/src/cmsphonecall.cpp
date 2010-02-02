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
#include "cmscontactorheaders.h"

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


// ---------------------------------------------------------
// CCmsPhoneCall::ExecuteL
//
// ---------------------------------------------------------
void CCmsPhoneCall::ExecuteL( const TDesC& aPhoneNumber, TCmsPhoneCallType aCallType)
	{
	CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCmsPhoneCall::ExecuteLD()"));
    
    __ASSERT_DEBUG (NULL != &aPhoneNumber && 0 < aPhoneNumber.Size(), Panic (EPanicPreCond_ExecuteL));

#ifdef _DEBUG
	HBufC8* buf = HBufC8::NewLC ( aPhoneNumber.Length ());
	buf->Des().Copy ( aPhoneNumber);
	TPtrC8 numberDesc( buf->Des ());
	CMS_DP(KCMSContactorLoggerFile, CMS_L8( "  MSISDN:    %S" ), &numberDesc );
	CMS_DP(KCMSContactorLoggerFile, CMS_L8( "  Call Type: %d" ), aCallType );
	CleanupStack::PopAndDestroy (); //buf
#endif

	//ParsePhoneNumber may only short length of phonenumber, 
	//so it's safe to use same lenght as given number and do the copy.
	HBufC* numBuf = HBufC::NewLC ( aPhoneNumber.Length ());
	numBuf->Des().Append (aPhoneNumber);
	TPtr numDesc( numBuf->Des ());

	CommonPhoneParser::ParsePhoneNumber ( numDesc,
			CommonPhoneParser::EPhoneClientNumber); 

	CAiwDialDataExt* dialDataExt = CAiwDialDataExt::NewLC ();
	dialDataExt->SetPhoneNumberL ( numBuf->Des ());
	switch (aCallType)
	    {
	    case ECmsCallTypeVoice:
	        dialDataExt->SetCallType ( CAiwDialData::EAIWVoice);
	        break;
	    case ECmsCallTypeVoIP:
	        dialDataExt->SetCallType ( CAiwDialData::EAIWVoiP);
	        break;
	    case ECmsCallTypeVideo:
	        dialDataExt->SetCallType ( CAiwDialData::EAIWForcedVideo);
	        break;
	    default:
	        dialDataExt->SetCallType ( CAiwDialData::EAIWVoice);
	        break;
	    }

	dialDataExt->SetWindowGroup ( CCoeEnv::Static()->RootWin().Identifier ());

	DoAIWCallL (*dialDataExt);

	CleanupStack::PopAndDestroy (dialDataExt);

	CleanupStack::PopAndDestroy (numBuf);

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCmsPhoneCall::ExecuteLD(): Done."));
	return;
	}

// ---------------------------------------------------------
// CCmsPhoneCall::DoAIWCallL
//
// ---------------------------------------------------------
void CCmsPhoneCall::DoAIWCallL( CAiwDialDataExt& aDialData)
	{
	CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCmsPhoneCall::DoAIWCallL()"));

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

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCmsPhoneCall::DoAIWCallL(): Done."));
	return;
	}

//  End of File  
