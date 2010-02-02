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






#ifndef     SPDIAGRIDDLG_H
#define     SPDIAGRIDDLG_H

//  INCLUDES
#include <AknGrid.h>
#include <AknDialog.h>   // CEikDialog
#include <MPbkContactDbObserver.h>

//  FORWARD DECLARATIONS

// CLASS DECLARATION
class CSpdiaControl;
class CSpdiaGrid;
class CPbkContactChangeNotifier;

/**
*  The main UI container element of the Speeddial Control Utility 
*  that contains the 3x3 grid of speeddial entries.
*
*  @since 
*/
class CSpdiaGridDlg : public CAknDialog,
                      public MPbkContactDbObserver
    {
    public:  // Constructors and destructor
        /**
        * Creates a selection grid dialog.
        *
        * @param aDial   select dial number.
        */
        static CSpdiaGridDlg* NewL(TInt& aDial, const CSpdiaControl& aControl);

        /**
        * Construction.
        */ 
        CSpdiaGridDlg(TInt& aDial, const CSpdiaControl& aControl);

        /**
        * Construction.
        */ 
        CSpdiaGridDlg();

        /**
        * Destructor.
        */ 
        virtual ~CSpdiaGridDlg();
        
        /**
        * From CAknDialog takes care of skin changes and layout.
        * @param aType skin event identifier.
        */
        void HandleResourceChange(TInt aType);

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
        /**
        * from CAknDialog
        */ 
        void HandleDialogPageEventL (TInt);

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
        void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    private:  // functions
        /**
        * 
        */ 
        void LoadCbaL();

    private:  // data
        CSpdiaControl* iControl;

        TInt&       iDial;
        CPbkContactChangeNotifier* iPbkNotifier;    // owned by this
        CSpdiaGrid* iGrid;
        TSize       iCellSize;
        TInt        iCbaID;
        TBool       iNeedUpdate;
        TInt        iPrevIndex;
   };

#endif  // SPDIAGRIDDLG_H
            

// End of File

