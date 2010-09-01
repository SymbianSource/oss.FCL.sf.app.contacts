/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements Phonebook 2 launch New Contact directly from client application.
*
*/

// Phonebook 2
#include "Pbk2LaunchNewContact.h"
#include <e32property.h>
#include <e32std.h>
#include <Pbk2InternalUID.h>
#include <apgtask.h>
#include <apgcli.h> 
#include <eikenv.h>
// System includes
//const TUid KPbk2LaunchNewContactProperty = {0xEC42FD42};//AnyKey Uid//0x2002680C(protected UID) //0x3D335E82(random)
const TUid KNewContactLauncherUid = {0x20026FC2};

EXPORT_C void Pbk2LaunchNewContact::LaunchNewContactL()
    {
    _LIT8(dontActivate, "dontactivate");
    HBufC* param = HBufC::NewLC( 256 );
    param->Des().Format( _L( "4 %S" ),&dontActivate );

    TUid id( KNewContactLauncherUid );

    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( id );
    if ( task.Exists() )
    {
         HBufC8* param8 = HBufC8::NewLC( param->Length() );
         param8->Des().Append( *param );
         task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
         task.BringToForeground();
         CleanupStack::PopAndDestroy(); // param8
    }
    else
    {
         RApaLsSession appArcSession;
         User::LeaveIfError(appArcSession.Connect()); // connect to AppArc server
         TThreadId id;
         appArcSession.StartDocument( *param, KNewContactLauncherUid, id );
         appArcSession.Close();
    }
    CleanupStack::PopAndDestroy(); // param


    }

// End of File
