/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CMSCREATORSESSION_H__
#define __CMSCREATORSESSION_H__

// INCLUDES
#include <e32std.h>

//FORWARD DECLARATIONS

class RCmsCreatorSession;

//CLASS DECLARATION
class RCmsCreatorSession : public RSessionBase
    {
    public:  // New functions
        
        /**
        * C++ Constructor.
        */
        IMPORT_C RCmsCreatorSession();
        
        /**
        * Start the server
        *
        * @return TInt Error code
        */
        IMPORT_C TInt CreateContacts();

        /**
        * Destructor
        */
        IMPORT_C ~RCmsCreatorSession();

    private:
       
    };
        

#endif      //__CMSCREATORSESSION_H__



