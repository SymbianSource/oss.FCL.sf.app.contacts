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
* Description:     A utility that provides services to both the Speeddial and
*                PhoneBook applications for getting and setting speeddial
*                number configuration.
*
*/







// INCLUDE FILES
#include <bldvariant.hrh>

#include <bautils.h>

#include <sysutil.h>

#include <avkon.mbg>
#include <gdi.h>
#include <data_caging_path_literals.hrh>
#include <f32file.h>
#include <featmgr.h>

#include "spdiadialogs.h"
#include "speeddialprivate.h"

// LOCAL CONSTANTS AND MACROS
//This order is based on 'Standard field ids' (PbkFields.hrh)



// ================= MEMBER FUNCTIONS =======================
//
//CSpdiaDialogs :: NewL
//
//-----------------------------------------------------------
//

EXPORT_C CSpdiaDialogs* CSpdiaDialogs ::NewL(CVPbkContactManager& aContactManager)
	{
	CSpdiaDialogs* self = new(ELeave) CSpdiaDialogs();
	
	CleanupStack::PushL(self);
	self->ConstructL(&aContactManager );
	CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------
//CSpdiaDialogs :: ~CSpdiaDialogs()
//
//----------------------------------------------------------
//
CSpdiaDialogs::~CSpdiaDialogs()
{
	delete iSpeedPrivate;
}

// ---------------------------------------------------------
//CSpdiaDialogs :: ConstructL()
//
//----------------------------------------------------------
//
void CSpdiaDialogs::ConstructL(CVPbkContactManager* aContactManager)
{
   iSpeedPrivate = CSpeedDialPrivate::NewL(aContactManager);
    	
}

// ---------------------------------------------------------
//CSpdiaDialogs :: CSpdiaDialogs()
//
//----------------------------------------------------------
//
CSpdiaDialogs::CSpdiaDialogs()
{
	
}
	
// ---------------------------------------------------------
// CSpdiaDialogs::ShowAssign
// Function called when assigning speed dial from phone book
// application
// ---------------------------------------------------------
//

EXPORT_C TInt CSpdiaDialogs::ShowAssign( MVPbkContactLink& aContactLink, TInt& aIndex )
{
	return iSpeedPrivate->ShowAssign(aContactLink, aIndex);	
}
// ---------------------------------------------------------
// CSpdiaDialogs::ShowAssign
// Function called when assigning speed dial from SpeedDial and Telephony
// application
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaDialogs::ShowAssign( TInt aIndex, MVPbkContactLink*& aContactLink  )
{

	return iSpeedPrivate->ShowAssign(  aIndex, aContactLink   );
}

// ---------------------------------------------------------
// CSpdiaDialogs::ShowRemove
// Function called when removing speed dial from SpeedDial application
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaDialogs::ShowRemove( TInt aIndex )
{
	return iSpeedPrivate->ShowRemove(aIndex);
}

// ---------------------------------------------------------
// CSpdiaDialogs::Cancel
// Function called from Telephony to cancel the Aiw event
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaDialogs::Cancel()
{
	return iSpeedPrivate->Cancel();
}

//End of file
