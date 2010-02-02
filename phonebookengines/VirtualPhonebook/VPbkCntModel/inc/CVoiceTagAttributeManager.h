/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#ifndef CVOICETAGATTRIBUTEMANAGER_H
#define CVOICETAGATTRIBUTEMANAGER_H

#include <cvpbkcontactattributeplugin.h> 

namespace VPbkCntModel {
    
/**
 * Contact model store Voice tag attribute manager factory.
 */
NONSHARABLE_CLASS( CVoiceTagAttributeManager ): 
        public CVPbkContactAttributePlugin
    {
    public: // Construction and destruction
        static CVoiceTagAttributeManager* NewL(TAny* aParam);
        ~CVoiceTagAttributeManager() { }
        
    };
    
}

#endif // CVOICETAGATTRIBUTEMANAGER_H

// End of File
