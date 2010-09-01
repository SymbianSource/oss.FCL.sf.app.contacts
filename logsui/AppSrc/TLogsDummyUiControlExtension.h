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
*    
*
*/


#ifndef __TLogsDummyUiControlExtension_H
#define __TLogsDummyUiControlExtension_H


//  INCLUDES
#include "MLogsUiControlExtension.h"


// CLASS DECLARATION

/**
 * Dummy ui control extension. Used as default implementation.
 */
class TLogsDummyUiControlExtension : public MLogsUiControlExtension                                        
    {
    public: // Constructor and destructor
        /**
         * Constructor.
         */
        TLogsDummyUiControlExtension();

        /**
         * Destructor.
         */
        ~TLogsDummyUiControlExtension();

    private: // Fron MLogsUiExtensionBase
        void DoRelease();
        
    public: // From MLogsUiControlExtension
        void AppendIconsL( CAknIconArray* aArray );
        TBool GetIconIndexL( TContactItemId aContactId, TInt& aIndex );

// Sawfish VoIP changes  >>>>
    public:
        /**
         * Creates and starts all the icon fetching processes
         *
         * @since S60 v3.2
         * @param aModel the data model that contains the
         * information about the log event entries in the current view.
         * @param aListBox fetched icons will be added to the icon array
         * of the specified listbox
         */    
        void HandleAdditionalData(
            MLogsModel& aModel, 
            CAknDoubleGraphicStyleListBox& aListBox );
        
        /**
         * Modifies the icon string descriptor 
         * e.g. adding the presence icon index '6' would change the icon string 
         * from "0\ttext1\ttext2" to "0\ttext1\ttext2\t\t\t6"
         * If anything inside this function leaves it will be trapped and aDes
         * wont be changed, otherwise aDes contains the resulting icon string.
         *
         * @since S60 v3.2
         * @param aDes the descriptor that contains the iconstring
         * @param aDataField a unparsed data field of a log event
         */ 
        void ModifyIconString ( TDes& aDes, const MLogsEventGetter& aLogsEventGetter );

        /**
         * Sets the observer of the extension.
         *
         * @since S60 v3.2
         * @param aObserver an observer
         */
        void SetObserver( MLogsExtObserver& aObserver );
// <<<<  Sawfish VoIP changes  
    };

#endif
            

// End of File
