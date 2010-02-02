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
* Description:  UI control Command item for "Add to Top".
*
*/


#ifndef CPBK2CMDITEMADDTOP_H_
#define CPBK2CMDITEMADDTOP_H_

//  INCLUDES
#include <e32base.h>
#include <MPbk2UiControlCmdItem.h>
#include <pbk2doublelistboxcmditemextension.h>

class MPbk2CmdItemVisibilityObserver;
class CPbk2AddFavoritesVisibility;

/**
 * UI control Command item for "Add to Top".
 */
NONSHARABLE_CLASS( CPbk2CmdItemAddTop ) :
        public CBase,
        public MPbk2UiControlCmdItem,
        public MPbk2DoubleListboxCmdItemExtension

	{
	public: // Construction & destruction

		static CPbk2CmdItemAddTop* NewLC();
		
		~CPbk2CmdItemAddTop();

	private: // From MPbk2UiControlCmdItem
	    TPtrC NameForUi() const;
	    TBool IsEnabled() const;
	    void SetEnabled( TBool aEnabled );
        TInt CommandId() const;
        void SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver);
        TAny* ControlCmdItemExtension( TUid aExtensionUid );

    private: // From MPbk2DoubleListboxCmdItemExtension
        void FormatDataL( MPbk2DoubleListboxDataElement& aDataElement );
        const MVPbkContactLink* Link();
	
	private:
		CPbk2CmdItemAddTop();
        void ConstructL();
        		
	private: // data
	    // TBool iEnabled;
	    CPbk2AddFavoritesVisibility* iAddFavoritesVisibility; // owned
	    HBufC* iText;
	};

#endif /*CPBK2CMDITEMADDTOP_H_*/
