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
* Description:  Server-side service implementation
*
*/


#include <bldvariant.hrh>
#include <apgcli.h>
#include <eikenv.h>
#include <eikappui.h>
#include <AknServerApp.h>
#include <s32mem.h>

#include "ccaappservice.h"
#include "ccalogger.h"
#include "ccauids.h"
#include "ccaclientserverappipc.h"
#include "ccaclientutils.h"
#include "ccaparameter.h"

// ---------------------------------------------------------
// CCAAppService::CCAAppService
// ---------------------------------------------------------
//
CCAAppService::CCAAppService()
	{
	if ( !iAppUi )
	    {
	    // Ignored CS warning ( Using CEikonEnv::Static )
	    // There is no other way to do this
            iAppUi = (( CCCAAppAppUi* )CEikonEnv::Static()->EikAppUi() );
	    }
	}

// ---------------------------------------------------------
// CCAAppService::~CCAAppService
// ---------------------------------------------------------
//
CCAAppService::~CCAAppService()
	{
	}

// ---------------------------------------------------------
// CCAAppService::CreateL
// ---------------------------------------------------------
//
void CCAAppService::CreateL()
	{
	CAknAppServiceBase::CreateL();
	}

// ---------------------------------------------------------
// CCAAppService::ServiceL
// ---------------------------------------------------------
//
void CCAAppService::ServiceL(const RMessage2& aMessage)
	{
    CCA_DP( KCCAppLogFile, CCA_L( "CCAAppService::ServiceL"));
    CCA_DP( KCCAppLogFile, CCA_L( "CCAAppService::ServiceL: Funtion=%d"), aMessage.Function());

	switch (aMessage.Function())
		{
		case ECCALaunchApplication:
		    {
		    TRAPD(err, ReadMsgL(aMessage));
            //Complete request with err
            CCA_DP( KCCAppLogFile, CCA_L( "CCAAppService::ServiceL: ReadMsgL=%d"), err);
            aMessage.Complete(err);
		    }
		    break;

        case ECCAGetWindowGroupId:   
            {
            TInt windowGroupId = CCoeEnv::Static()->RootWin().Identifier();
            TPckg<TInt> ptr( windowGroupId );
            aMessage.Write( 0, ptr );
            aMessage.Complete( KErrNone );
            }
            break;

		default:
			CAknAppServiceBase::ServiceL(aMessage);
			break;
		}

    CCA_DP( KCCAppLogFile, CCA_L( "CCAAppService::ServiceL: Done."));
	}

// ---------------------------------------------------------
// CCAAppService::ServiceError
// ---------------------------------------------------------
//
void CCAAppService::ServiceError(
    const RMessage2& aMessage,
    TInt aError )
	{
    CCA_DP( KCCAppLogFile, CCA_L( "CCAAppService::ServiceError: aError=%d"), aError);
	CAknAppServiceBase::ServiceError( aMessage, aError );
	}

// ---------------------------------------------------------
// CCAAppService::ReadMsgL
// ---------------------------------------------------------
//
void CCAAppService::ReadMsgL( const RMessage2& aMessage )
	{
    CCA_DP( KCCAppLogFile, CCA_L( "CCAAppService::ReadMsgDataL"));

    const TInt clientSideDataLength = aMessage.GetDesLengthL( 0 );
    HBufC8* clientData = HBufC8::NewLC( clientSideDataLength );
    TPtr8 clientDataPtr( clientData->Des() );
    aMessage.Read( 0, clientDataPtr );

    RDesReadStream readStream( clientDataPtr );
    CleanupClosePushL( readStream );

    CCCAParameter* data = CCCAParameter::NewL();
    CleanupStack::PushL( data );
    data->InternalizeL( readStream );

    //PERFORMANCE LOGGING: 5. Parameter transferred/unpacked over AppServer
    WriteToPerfLog();    
    CleanupStack::Pop( data ); // data
    iAppUi->SetParameter( *data );//ownership transferred
    iAppUi->InitializePluginL();
    

    CleanupStack::PopAndDestroy( 2, clientData );// readStream, clientData

    CCA_DP( KCCAppLogFile, CCA_L( "CCAAppService::ReadMsgDataL: Done"));
	}

// end of file
