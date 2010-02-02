/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Logs etel base class
*
*/


//  INCLUDE FILES  
#include <e32svr.h>
#include <exterror.h>
#include <etel.h>
#include <etelmm.h>
#include <mmtsy_names.h>
#include <secui.h>
#include <SecUiSecurityHandler.h>


#include "CLogsEtelBase.h"

// CONSTANTS

// _LIT(KMissingImplementation,"ETEL functionality missing");

// ================= MEMBER FUNCTIONS =======================


//  Default c++ constructor
CLogsEtelBase::CLogsEtelBase(): 
            CActive(EPriorityStandard), 
            iConnectionOk(EFalse)
    {
    }

//  Destructor
CLogsEtelBase::~CLogsEtelBase()
    {
    }

void CLogsEtelBase::CloseEtelConnection()
    {
    if( iConnectionOk )
        {
        iPhone.Close();
        iServer.UnloadPhoneModule( KMmTsyModuleName );
        iServer.Close();
        iConnectionOk = EFalse;
        }
    }

TInt CLogsEtelBase::OpenEtelConnection()
    {
	if(iConnectionOk)
        {
		return KErrNone;
        }
    
	TInt ret( iServer.Connect() );
    if( ret != KErrNone )
        {
        return ret;
        }

    // load phone module
    ret = iServer.LoadPhoneModule( KMmTsyModuleName );
    if( ret != KErrNone )
        {
        iServer.Close();
        return ret;
        }
    iServer.SetExtendedErrorGranularity( RTelServer::EErrorExtended );

    // get phone name
    RTelServer::TPhoneInfo iInfo;
    ret = iServer.GetPhoneInfo( 0, iInfo );
    if( ret != KErrNone )
        {
        iServer.UnloadPhoneModule( KMmTsyModuleName );
        iServer.Close();
        return ret;
        }

    // open phone
    ret = iPhone.Open( iServer, iInfo.iName );
    if( ret != KErrNone )
        {
        iServer.UnloadPhoneModule( KMmTsyModuleName );
        iServer.Close();
        return ret;
        }

	iConnectionOk = ETrue;
    return ret;
    }

void CLogsEtelBase::ShowSecUiNoteLD( TInt aResourceId )
    {
    TSecUi::InitializeLibL();
    TInt err;
    TRAP(err, CAknNoteDialog* noteDlg = new ( ELeave ) 
                    CAknNoteDialog(
                    CAknNoteDialog::ENoTone, 
                    CAknNoteDialog::ELongTimeout );
                noteDlg->ExecuteLD( aResourceId );
                );
    TSecUi::UnInitializeLib();    
    }


// End of file
