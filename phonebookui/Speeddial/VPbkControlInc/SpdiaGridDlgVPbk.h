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
* Description:     The main UI container element of the Speeddial Control 
*                Utility that contains the 3x3 grid of speeddial entries.
*
*/






#ifndef     SPDIAGRIDDLGVPBK_H
#define     SPDIAGRIDDLGVPBK_H

//  INCLUDES
#include <AknGrid.h>
#include <AknDialog.h>   // CEikDialog


//  FORWARD DECLARATIONS

// CLASS DECLARATION
class CSpeedDialPrivate;
class CSpdiaGridVPbk;


/**
*  The main UI container element of the Speeddial Control Utility 
*  that contains the 3x3 grid of speeddial entries.
*
*  @since 
*/
NONSHARABLE_CLASS( CSpdiaGridDlgVPbk ) : public CAknDialog
                     // public MPbkContactDbObserver
    {
    public:  // Constructors and destructor
        /**
        * Creates a selection grid dialog.
        *
        * @param aDial   select dial number.
        */
        static CSpdiaGridDlgVPbk* NewL(TInt& aDial, const CSpeedDialPrivate& aControl);

        /**
        * Construction.
        */ 
        CSpdiaGridDlgVPbk(TInt& aDial, const CSpeedDialPrivate& aControl);

        /**
        * Construction.
        */ 
        CSpdiaGridDlgVPbk();

        /**
        * Destructor.
        */ 
        virtual ~CSpdiaGridDlgVPbk();
        
        /**
        * From CAknDialog takes care of skin changes and layout.
        * @param aType skin event identifier.
        */
        void HandleResourceChange(TInt aType);
        
        /**
        * from CAknDialog
        */ 
        void HandleDialogPageEventL (TInt);


    public: // data structures

    public:  // constructors

    public:  // constructors
        void FocusChanged(TDrawNow aDrawNow);

    protected:
        /**
        * from CAknDialog
        */ 
        void PreLayoutDynInitL();

        /**
        * from CAknDialog
        */
        void PostLayoutDynInitL();

        /**
        * from CAknDialog
        */ 
        SEikControlInfo CreateCustomControlL(TInt aControlType);

        /**
        * from CAknDialog
        */ 
        TBool OkToExitL(TInt aButtonId);

    protected:
        /**
        * from CCoeControl
        */ 
        TKeyResponse OfferKeyEventL(
                    const TKeyEvent& aKeyEvent,
                    TEventCode aType);

    private: // Functions from base classes
        /**
        * From MContactDbObserver, Handles an event of type aEventType.
        */
        //void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    private:  // functions
        /**
        * 
        */ 
        void LoadCbaL();

    private:  // data
        CSpeedDialPrivate* iControl;

        TInt&       iDial;
        
        CSpdiaGridVPbk* iGrid;
        TSize       iCellSize;
        TInt        iCbaID;
        TBool       iNeedUpdate;
        TInt        iPrevIndex;
   };

#endif  // SPDIAGRIDDLG_H
            

// End of File

