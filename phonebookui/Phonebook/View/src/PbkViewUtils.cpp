/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*		Namespace structure for Phonebook view helper functions
*
*/


// INCLUDE FILES
#include "PbkViewUtils.h"
#include <pbkview.rsg>
#include <StringLoader.h>
#include <CPbkContactEngine.h>


// ================= MEMBER FUNCTIONS =======================

TChar PbkViewUtils::ResolveNameOrderSeparatorL(CPbkContactEngine& aEngine)
    {
    TChar separatorChar = aEngine.NameSeparator();		    		    
    TBuf<1> oneChar;
    oneChar.Append(separatorChar);
                
    if (oneChar[0]==0 || separatorChar.IsSpace())
        {
        // Separator is undefined. Load default value.
        HBufC* defaultSeparator = StringLoader::LoadL(R_QTN_NAME_SEPARATOR_CHAR);
        if (defaultSeparator)
            {                    
            separatorChar = defaultSeparator->Des()[0];
            delete defaultSeparator;        
            }
        }
    return separatorChar;
    }


// End of File
