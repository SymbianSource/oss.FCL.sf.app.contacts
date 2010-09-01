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
#include "cmscontactorheaders.h"

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

// ---------------------------------------------------------
// CCmsMsgEditors::ExecuteL
//
// ---------------------------------------------------------
void CCmsMsgEditors::ExecuteL( const TUid aServiceUid,
		const TDesC& aAddress, const TDesC& aName)
	{
	CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCmsMsgEditors::ExecuteLD()"));

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
	CMS_DP(KCMSContactorLoggerFile, CMS_L( "  UID: %d" ), aServiceUid );
	CMS_DP(KCMSContactorLoggerFile, CMS_L8( "  Address:    %S" ), &numberDesc );
	CMS_DP(KCMSContactorLoggerFile, CMS_L8( "  Name: %S" ), &nameDesc );

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
	sendui->CreateAndSendMessageL (aServiceUid, msgdata, KNullUid, ETrue);

	CleanupStack::PopAndDestroy ( 3, numBuf); //msgdata, sendui

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCmsMsgEditors::ExecuteLD(): Done."));
	return;
	}

//  End of File  
