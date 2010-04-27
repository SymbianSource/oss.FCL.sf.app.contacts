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
* Description:     Declares container control for application.
*
*/






#ifndef SPDIACONTAINER_H
#define SPDIACONTAINER_H

// INCLUDES
#include <aknview.h>
#include <AknUtils.h>
#include <eiklbo.h>
#include <MPbkContactDbObserver.h>
#include "centralrepository.h"
#include <aknlongtapdetector.h>

// FORWARD DECLARATIONS
//class CAknGrid;
class CAknQdialStyleGrid;
class CSpdiaView;
class CPbkContactChangeNotifier;
class CSpeedDialPrivate;
class TSpdiaIndexData;
class CSpdiaNoteDialog;
class CVPbkContactManager;
class CPNGNumberGrouping;
class CAknLongTapDetector;
// CLASS DECLARATION

class MRefreshObserver
    {
    public: // Interface
        virtual void RefreshDialog() = 0;
    };

/**
*  Generic Speeddial container class.
*  Container's task is to dispatch key presses to an application-side
*  key event handler. If key event is not processed application-side,
*  it is passed to the contained control.
*
*  @since 
*/

class CSpdiaContainer : public CCoeControl,
                        public MPbkContactDbObserver,
                        public MAknLongTapDetectorCallBack,
                        public MRefreshObserver,
                        public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CSpdiaContainer(CSpeedDialPrivate *aSpeedPrivate);
        
        /**
        * Symbian second phase Constructor.
        * @param aView      Frame pointer.
        */
        void ConstructL(CAknView* aView);

        /**
        * Virtual C++ destructor.
        */
        virtual ~CSpdiaContainer();
        
        /**
        * From CAknContainer takes care of skin changes.
        * @param aType skin event identifier.
        */
        void HandleResourceChange(TInt aType);

    public: // New functions
        /**
        * Gets the grid data control of speeddial. 
        * @return   reference of spdctrl
        */
        CSpeedDialPrivate& Control();

        /**
        * Returns whether the cell position is a VoiceMailBox number or not. 
        * @param aIndex    index number
        * @return   ETrue: On VoiceMail Box number
        */
        TBool IsVoiceMailBox(TInt aIndex = -1) const;
        TBool IsVdoMailBox(TInt aIndex = -1) const;

        /**
        * Gets the current position of the grid.
        * @return   Grid position
        */
        TInt CurGridPosition() const;

        /**
        * Redraws and renfreshes the grid data.
        */
        void UpdatePhoneNumberL(TInt aIndex = -1);

        /**
        * Get resource id on grid position
        */
        TInt MenuResourceID();
        
        /**
        * Selection of the MiddleSoftKey
        * 
        */
        void MiddleSoftKeyL();

        /**
        * Shows the ViewNumber note.
        */
        void ShowViewNumberL();

        /**
        * Redraws and renfreshes the ViewNumber note.
        */
        void DoUpdateL();

        /**
        * Gets the pointer of the grid control.
        * @return   point of gird control.
        */
        CAknQdialStyleGrid* Grid();
        
        /* Activating the animation in 
        * the background for grid
        */
        void FocusChanged(TDrawNow aDrawNow);
        
        /**
        * Set status pane to the normal status.
        */
        void SetStatusPaneL();

    public: // Functions from CCoeControl classes
        /**
        * This function is called when Help application is launched.  
        * @param aContex
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
        * This function is called from Spdiaview once the application comes 
         to foreground.
        */
        void RefreshGrid();
        /**
        * Sets true when voice mail box dialog launched 
        * sets false dialog destroyed.  
        * @param aBool
        */
        void VoiceMailDialogLaunched( TBool aBool );


        CAknLongTapDetector& LongTapDetectorL();
        /**
        * refresh the dialog in speeddial view 
        */
        void RefreshDialog();
    public: // Functions from base classes

    private:    // New function


        /**
        * Call create by current number
        */
        void CallCreateL();

        /**
        * Creats image from thumbnail.
        */
        CEikImage* CreateBitmapL(const CGulIcon& aSrc);

    private: // Functions from base classes
        /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl,Draw.
        */
        void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl processing of a key event
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

        /**
        * From CCoeControl,HandlePointerEventL.
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        /**
        * From MContactDbObserver, Handles an event of type aEventType.
        */
        void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
        
        /**
        * Returns scaled CEikImage pointer
        * @param selected icon.
        */
        CEikImage* CreateScaledBitmapL(const CGulIcon& aSrc);
   
    public:    
        /**
        * From MAknLongTapDetectorCallBack
        * 
        */
        void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                  const TPoint& aPenEventScreenLocation );
        
        /**
        * From MEikListBoxObserver, Handles an event of listbox.
        */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    private:
        
        void SetLayoutL();
        
    private: //data
        CAknQdialStyleGrid*   iGrid;             // owned by this
        CSpdiaView* iView;
        CSpeedDialPrivate* iSpeedPrivate;                 // owned by this
        CVPbkContactManager *iContactManager;
        CSpdiaNoteDialog* iDialog;               // owned by this
        TBool iNeedUpdate;

        CPNGNumberGrouping* iNumberGrouping;
        TBool iVoiceDialog;
        TBool iAlreadyActive;
        CAknLongTapDetector* iLongTapDetector;
        CRepository*  ivmbxvariation;
        TInt iVmbxsupported;
        
        TBool iLongTapUsed;
        
        // Indicator for the note: no assigned number shown
        TBool iNoAssignedShown;
        
        // Grid index in which the button down
        TInt iButton1DownIndex;
    };

#endif        // SPDIACONTAINER_H

// End of File
