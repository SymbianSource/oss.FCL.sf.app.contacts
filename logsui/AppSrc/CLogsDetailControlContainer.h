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
*     Logs "Detail" view container control class implementation
*
*/


#ifndef     __Logs_App_CLogsDetailControlContainer_H__
#define     __Logs_App_CLogsDetailControlContainer_H__

//  INCLUDES
#include "CLogsBaseControlContainer.h"
#include "MLogsObserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsDetailView;
class CAknSingleHeadingStyleListBox;
class CAknNavigationDecorator;
class CCoeControl;
class CEikLabel;
class CPhoneNumberFormat;
class CLogsDetailAdapter;

// CLASS DECLARATION

/**
 * Logs "Detail" view container control class implementation.
 */

class CLogsDetailControlContainer
    : public CLogsBaseControlContainer
    {
    public:  // construction
        /**
         * Standard creation function.
         *
         * @param aAppView this control's application view.
         * @param aDetailView owner of this class.
         * @param aRect client rect size
         * @return new object
         */
        static CLogsDetailControlContainer* NewL
                                            (   CLogsDetailView* aView
                                            ,   const TRect& aRect
                                            );

        /**
         * Destructor. Deletes child controls.
         */
        ~CLogsDetailControlContainer();

    private:  
        /**
         * C++ constuctor
         * @param aView
         */
        CLogsDetailControlContainer( CLogsDetailView* aView );

        /**
         * Constructor, second phase.
         *
         * @param aAppView  this control's application view.
         * @param aRect client rect size
         */
        void ConstructL( const TRect& aRect );

        /**
         * Initialize & update navigation pane
         */
        void InitNaviPaneL();
        
        /**
         *  Handles focus change events. This will hand over focus changes to list so that 
         *  focus animations are displayed on the list.
         */
        void FocusChanged(TDrawNow aDrawNow);

    public:
        /**
         * Returns the iListBox.
         * @return listbox instance related to control container
         */
        CAknSingleHeadingStyleListBox* ListBox();

        /**
         * Update contents
         */
        void UpdateL();

    protected:  // from CCoeControl
        CCoeControl* ComponentControl( TInt aIndex ) const;

        void SizeChanged();

        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private:  // data
        /// Own: ListBox control
        CAknSingleHeadingStyleListBox*  iListBox;

        /// Ref: Pointer to the detail view.
        CLogsDetailView*                iView;

        /// Own: fetching data text
        HBufC*                          iFetchingDataText;

        /// Own: Tab group with time & arrows
        CAknNavigationDecorator*        iDecoratedTabGroup;

        /// Own: Title texts for various purposes
        HBufC*                          iPrivateNumber;
        HBufC*                          iUnknownNumber;
        HBufC*                          iPayphoneNumber;        
        HBufC*                          iEmergencyCall;
        HBufC*                          iOperServNumber;
        HBufC*                          iGroupCall;
        CEikLabel*                      iTitleLabel;
        HBufC*                          iCsdNumber;

        /// Own: show csd flag
        // TBool                           iShowCsd;

        /// Own: object with groupping, clipping and A&H functionality
        CPhoneNumberFormat*             iPhoneNumber;

        //Ref: listbox adapter
        CLogsDetailAdapter* iAdapter;
    };

#endif  // __Logs_App_CLogsDetailControlContainer_H__

// End of File

