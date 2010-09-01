/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CMSCREATORSESSION2_H__
#define __CMSCREATORSESSION2_H__

// INCLUDES
#include <e32std.h>

//FORWARD DECLARATIONS

class RCmsCreatorSession2;

//CLASS DECLARATION
class RCmsCreatorSession2 : public RSessionBase
    {
    public:  // New functions
        
        /**
        * C++ Constructor.
        */
        IMPORT_C RCmsCreatorSession2();
        
        /**
        * Start the server
        *
        * @return TInt Error code
        */
        IMPORT_C TInt CreateContacts();

        /**
        * Destructor
        */
        IMPORT_C ~RCmsCreatorSession2();

    private:
       
    };
        

#endif      //__CMSCREATORSESSION_H__



