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
* Description: Class for the MyCard item on the top of the namelist view.
* Class handles the visibility of the item, text displayed on the list and
* linking the corresponding command to it.
*
*/


#ifndef CPBK2CMDITEMMYCARD_H_
#define CPBK2CMDITEMMYCARD_H_

//  INCLUDES
#include <e32base.h>
#include <MPbk2UiControlCmdItem.h>
#include <pbk2doublelistboxcmditemextension.h>
#include "mpbk2mycardobserver.h"

class MPbk2CmdItemVisibilityObserver;
class CPbk2MyCard;
class CSpbContentProvider;

/**
 *
 */
NONSHARABLE_CLASS( CPbk2CmdItemMyCard ) :
        public CBase,
        public MPbk2UiControlCmdItem,
        public MPbk2MyCardObserver,
        public MPbk2DoubleListboxCmdItemExtension
    {
    public: // Construction & destruction

        static CPbk2CmdItemMyCard* NewLC( CPbk2MyCard& aMyCard,
										  CSpbContentProvider& aContentProvider );

        ~CPbk2CmdItemMyCard();

    private: // From MPbk2UiControlCmdItem
        TPtrC NameForUi() const;
        TBool IsEnabled() const;
        void SetEnabled( TBool aEnabled );
        TInt CommandId() const;
        void SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver);
        TAny* ControlCmdItemExtension(
                TUid aExtensionUid );
        
    private:
        void FormatDataL(
                MPbk2DoubleListboxDataElement& aDataElement );
        
        const MVPbkContactLink* Link();

    private: // From MPbk2MyCardObserver
        void MyCardEvent( MPbk2MyCardObserver::TMyCardStatusEvent aEvent );

    private:
        CPbk2CmdItemMyCard( CPbk2MyCard& aMyCard,
							CSpbContentProvider& aContentProvider );
        void ConstructL();
        void NotifyVisibiltyChange( TBool aShow);

        enum TmcLastNotif
            {
            EmcLastNotifNone = 0,
            EmcLastNotifHidden,                    
            EmcLastNotifVisible            
            };        

    private: // data
        TBool iEnabled;
        TmcLastNotif iLastNotif;
        HBufC* iCreateMyCardText;
        HBufC* iOpenMyCardText;
        MPbk2CmdItemVisibilityObserver* iObserver;
        CPbk2MyCard& iMyCard;
        
        //REF:
        CSpbContentProvider& iStatusProvider;
    };

#endif /*CPBK2CMDITEMMYCARD_H_*/
