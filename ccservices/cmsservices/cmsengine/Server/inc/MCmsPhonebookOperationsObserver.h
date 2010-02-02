/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An observer interface for contact store open completion
*
*/


#ifndef MCMSPHONEBOOKOPERATIONSOBSERVER_H
#define MCMSPHONEBOOKOPERATIONSOBSERVER_H


// INCLUDES
#include <e32std.h>


/**
 * An observer interface for contact store open complete notification
 *
 *
 */
class MCmsPhonebookOperationsObserver
    {
    public:  
        /**
         * Called when a the stores open is compete 
         *
         * 
         */
        virtual void StoreOpenComplete() =0;
        
        /**
         * Called when a the contact retrieve operation is complete 
         *
         * @param aError Operation completion status
         */
        virtual void CmsSingleContactOperationComplete( TInt aError ) =0;
        

              
    protected:  // Destructor
        virtual ~MCmsPhonebookOperationsObserver() { }

    };


#endif  // MCMSPHONEBOOKOPERATIONSOBSERVER_H

//End of file
