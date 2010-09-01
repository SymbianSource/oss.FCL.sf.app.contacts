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
* Description:  UI control Command item for "Search from Remote".
*
*/


#ifndef CPBK2CMDITEMREMOTECONTACTLOOKUP_H_
#define CPBK2CMDITEMREMOTECONTACTLOOKUP_H_

//  INCLUDES
#include <e32base.h>
#include <MPbk2UiControlCmdItem.h>

class MPbk2CmdItemVisibilityObserver;
class CPbk2RemoteContactLookupVisibility;

/**
 * UI control Command item for "Add to Top".
 */
NONSHARABLE_CLASS( CPbk2CmdItemRemoteContactLookup ) : public CBase, public MPbk2UiControlCmdItem
	{
	public: // Construction & destruction

		static CPbk2CmdItemRemoteContactLookup* NewLC();
		
		~CPbk2CmdItemRemoteContactLookup();

	private: // From MPbk2UiControlCmdItem
	    TPtrC NameForUi() const;
	    TBool IsEnabled() const;
	    void SetEnabled( TBool aEnabled );
        TInt CommandId() const;
        void SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver);
	private:
		CPbk2CmdItemRemoteContactLookup();
        void ConstructL();
        		
	private: // data
	    //TBool iEnabled;
	    CPbk2RemoteContactLookupVisibility* iRemoteContactLookupVisibility; // owned
	    HBufC* iText;
	};

#endif /*CPBK2CMDITEMREMOTECONTACTLOOKUP_H_*/
