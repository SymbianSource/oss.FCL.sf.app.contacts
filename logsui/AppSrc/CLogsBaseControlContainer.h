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
*     Logs Application view control container superclass
*
*/


#ifndef     __Logs_App_CLogsBaseControlContainer_H__
#define     __Logs_App_CLogsBaseControlContainer_H__

//  INCLUDES

#include <coecntrl.h>	// CCoeControl
#include <aknlists.h>	// iListBox's format

#include "LogsConsts.h"
#include "Logs.hrh"

//#include <aknlongtapdetector.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknIconArray;
class CGulIcon;
class CEikListBox;
class CEikStatusPane;
class MLogsKeyProcessor;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class MLogsNaviDecoratorWrapper;

// CLASS DECLARATION

/**
 * Logs Application view control container superclass.
 * Provides functionality common to all control containers. Derive specialized 
 * control containers from this class to use the common functionality.
 * The control container encapsulates and creates the actual controls 
 * implementing the view. It also receives view's keyboard events and maps them
 * to application commands or forwards the events to child controls for 
 * processing.
 * If derived control container class contains more than 1 component
 * functions:
 * - SetFocus
 * - OfferKeyEventL
 * - CountComponentControls must be rewritten
 * The application view creates and owns the control container.
*/

class CLogsBaseControlContainer
	: public CCoeControl
	, public MCoeControlObserver				
	//, public MAknLongTapDetectorCallBack    // For long tap event callback handling
	{
	protected: // construction
		/**
		 * C++ Constructor
		 *   
		 * @param aKeyProcessor pointer to keyprocessor view
		 */
		CLogsBaseControlContainer( MLogsKeyProcessor* aKeyProcessor );
 
		/**
		 * C++ Destructor
		 * Needed to delete the iLongTapDetector object
		 */
		~CLogsBaseControlContainer(); 

		/**
		 * Second phase base class constructor
		 */
        void BaseConstructL();

private: 
		/**
		 * C++ Constructor
		 */
		CLogsBaseControlContainer();


	protected:  // Generic operations
		/**
		 * Make title
		 * @param aResourceText resource to create title
		 */
		void MakeTitleL( TInt aResourceText );

		/**
		 * Make title
		 * @param aText title text
		 */
		void MakeTitleL( const TDesC& aText );

		/**
		 * Appui's status pane getter
		 * @return Statuspane pointer
		 */
		CEikStatusPane* StatusPane();

		/**
		 * Navi pane getter
		 * @return navipane pointer
		 */
		CAknNavigationControlContainer* NaviPaneL();

		/**
		 * Navigation decorator tabulator getter
		 *
		 * @param aResource navigation bar resource id
		 * @param interface for singleton wrapper CLogsNaviDecoratorWrapper
		 *        (passed as an interface to hide it from Logs Plugin)
		 * @return navigation decorator
		 */
		CAknNavigationDecorator* NavigationTabGroupL( TInt aResource, 
		                                              MLogsNaviDecoratorWrapper*  aNaviDecoratorWrapper );

       /**
		 *  Append a color icon to icon array. CAknIconArray owns icons so it is 
		 *  easier to it leave safely.
		 *
		 *  @param aIcons                 Icon array
		 *  @param aIconFile              Icon file name without path
		 *  @param aIconGraphicsIndex     Picture index.
		 *  @param aIconGraphicsMaskIndex Mask index.
		 */
        void AddColorIconL
                (	CAknIconArray* aIcons
				,	const TDesC& aIconFile
                ,   TAknsItemID aSkinnedIcon
				,	TInt aIconGraphicsIndex
				,	TInt aIconGraphicsMaskIndex
				);
            
		/**
		 *  Append icon to icon array. CAknIconArray owns icons so it is 
		 *  easier to it leave safely.
		 *
		 *  @param aIcons                 Icon array
		 *  @param aIconFile              Icon file name without path
		 *  @param aIconGraphicsIndex     Picture index.
		 *  @param aIconGraphicsMaskIndex Mask index.
		 */
		void AddIconL
				(	CAknIconArray* aIcons
				,	const TDesC& aIconFile
                ,   TAknsItemID aSkinnedIcon
				,	TInt aIconGraphicsIndex
				,	TInt aIconGraphicsMaskIndex
				);

		/**
		 *  Append icon to icon array. CAknIconArray owns icons so it is 
		 *  easier to it leave safely.
		 *
		 *  @param aIcons                 Icon array
		 *  @param aIconFile              Icon file name with full path
		 *  @param aIconGraphicsIndex     Picture index.
		 *  @param aIconGraphicsMaskIndex Mask index.
		 */
        void AddIconWithPathL
                ( CAknIconArray* aIcons
                ,  const TDesC& aIconFileWithPath   //File name with full path
                ,  TAknsItemID aSkinnedIcon
                ,  TInt aIconGraphicsIndex
                ,  TInt aIconGraphicsMaskIndex 
                );

		/**
		 *  Makes view's one line with the given information
		 *
		 *  @param  aItems          Array where to put the line information
		 *  @param  aText           Icon index and tabulator
		 *  @param  aResourceText   The resource index of text to be shown
		 *  @param  aSecondLineText Second line text. Defaul is one space
		 */
		void MakeListBoxLineL
				(	CDesCArrayFlat* aItems
				,	const TDesC& aText
				,	TInt aResourceText
				,	const TDesC& aSecondLineText = KSpace
				);

		/**
		 *  Make the empty listbox text from the given resource text
		 *
		 *  @param  aListBox    The listbox pointer on where to make the text
		 *  @param  aResourceText   The resource text
		 */
		void MakeEmptyTextListBoxL
				(	CEikListBox* aListBox
				,	TInt aResourceText
				);
				
	    /**
		 *  Contstruct a "Logging not enabled" informative text
		 *  to be shown in an empty listbox
		 *
		 *  @return  The text buffer
		 */		
		HBufC* MakeNoLogTextLC();			

	public:
		/**
		 *  Makes the scroll up/down arrows visible
		 *
		 *  @param  aListBox    The listbox where to point
		 */
		void MakeScrollArrowsL( CEikListBox* aListBox );

		/**
		* From CCoeControl
		*/
		void HandleResourceChange( TInt aType );

		/**
		 *  Restore original Logs icon to Context Pane
		 */
        void SetContextPanePictureToDefaultL();
        
       /**
        *  Restore original Logs title pane text according to current view
        *  
        *  @param  aViewId  the current logs view id
        */
        void SetTitlePaneTextToDefaultL(TLogsViewIds aViewId);

	protected:  // from CCoeControl
		/**
		 * Focus update
		 * @param aFocus, 
		 * @param aDrawNow, 
		 */
		void SetFocus( TBool aFocus, TDrawNow aDrawNow = ENoDrawNow );

		/**
		 * Receives keyboard events for the view. Called by the CONE subsystem.
		 *  
		 * @return EKeyWasConsumed if the keyboard event was consumed.
		 * @return EKeyWasNotConsumed if the keyboard was not consumed.
		 */
		virtual TKeyResponse OfferKeyEventL
								(	const TKeyEvent& aKeyEvent
								,	TEventCode aType
								);
		
		// --------- currently not needed ------------------------------
		//  			
	    /**
	     * Called by the framework when a (touch ui) pointer event happens
		 *
		 * @param aPointerEvent 	The pointer event
		 */
	    //void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	    
	    /**
	     * Default callback function for CAknLongTapDetector member object.
	     * Base class implementation does nothing
	     * 
		 * @param aPenEventLocation Long tap event location relative to parent control.
         * @param aPenEventScreenLocation Long tap event location relative to screen.
		 */
	    //void HandleLongTapEventL( const TPoint& aPenEventLocation, 
        //                      	const TPoint& aPenEventScreenLocation );
        //
        // --------------------------------------------------------------	
        
        /**
         * Virtual getter for the containers ListBox instance.
         * Called from the HandlePointerEventL cause we need to pass the 
         * pointer event to the ListBox.
         *
         * @return 	Pointer to the list box 
         */                    	  
        virtual CEikListBox* ListBox();

		/**
		 *  Returns the count of child controls.
		 *
		 *  @return Count of the sub controls.
		 */
		TInt CountComponentControls() const;

		/**
		 *  Called from HandleResourceChange. Base class version has no
		 *  implementation.
		 *
		 */
		virtual void AddControlContainerIconsL();

	protected:  // MCoeControlObserver
		/**
		 * Handles event from observed control.
		 *
		 * @param aControl   The control that sent the event.
		 * @param aEventType The event type.
		 */
		void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

	protected:
        /// Ref: keyprocessor
        MLogsKeyProcessor*		iKeyProcessor;
        
        //CAknLongTapDetector* 	iLongTapDetector; 
	};

#endif  // __Logs_App_CLogsBaseControlContainer_H__


// End of File

