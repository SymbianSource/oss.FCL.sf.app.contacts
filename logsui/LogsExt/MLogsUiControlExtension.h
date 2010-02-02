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
*    Abstract interface for making logs UI control extensions.
*
*/


#ifndef __MLogsUiControlExtension_H
#define __MLogsUiControlExtension_H


//  INCLUDES
//#include <PbkIconId.hrh>    // TPbkIconId
#include <cntdef.h>         // TContactItemId
#include "MLogsUiExtensionBase.h"


//  FORWARD DECLARATIONS
class CAknIconArray;

class MLogsExtObserver;
class MLogsModel;
class MLogsEventGetter;
class CAknDoubleGraphicStyleListBox;

// CLASS DECLARATION

/**
 * @internal This interface is internal to Logs.
 *
 * Abstract interface for logs UI control extensions.
 */
class MLogsUiControlExtension : public MLogsUiExtensionBase
    {
    public: // Interface
        /**
         * Append extension icons to the array
         *
         * @param aArray array of icons
         */
        virtual void AppendIconsL( CAknIconArray* aArray ) = 0;

        /**
         * Get presence icon index within the icon array
         *
         * @param aContactId contact id
         * @param aIndex icon index
         * @return ETrue if found, otherwise EFalse
         */
        virtual TBool GetIconIndexL( TContactItemId aContactId, TInt& aIndex ) = 0;

// Sawfish VoIP changes  >>>>
        /**
         * Creates and starts all the icon fetching processes
         *
         * @since S60 v3.2
         * @param aModel the data model that contains the
         * information about the log event entries in the current view.
         * @param aListBox fetched icons will be added to the icon array
         * of the specified listbox
         */
        virtual void HandleAdditionalData( 
                                MLogsModel& aModel, 
                                CAknDoubleGraphicStyleListBox& aListBox ) = 0;

        /**
         * Modifies the icon string descriptor
         * 
         * e.g. adding the presence icon index '6' would change the icon string 
         * from "0\ttext1\ttext2" to "0\ttext1\ttext2\t\t\t6"
         * If anything inside this function leaves it will be trapped and aDes
         * wont be changed, otherwise aDes contains the resulting icon string.
         *
         * @since S60 v3.2
         * @param aDes the descriptor that contains the iconstring
         * @param aDataField the unparsed data field of a log event
         */ 
        virtual void ModifyIconString ( 
                            TDes& aDes, 
                            const MLogsEventGetter& aLogsEventGetter ) = 0;
        
        /**
         * Sets the observer of the extension.
         *
         * @since S60 v3.2
         * @param aObserver an observer
         */
        virtual void SetObserver( MLogsExtObserver& aObserver ) = 0 ;
// <<<<  Sawfish VoIP changes
        
    protected:
        /**
         * Destructor.
         */
        virtual ~MLogsUiControlExtension() { }
    };

#endif
            
// End of File
