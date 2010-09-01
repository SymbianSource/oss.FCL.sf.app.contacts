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
* Description:  Phonebook 2 application UI base.
*
*/


#ifndef CPBK2APPUIBASE_H
#define CPBK2APPUIBASE_H

//  INCLUDES
#include <MPbk2AppUi.h>
#include <aknappui.h>
#include <aknViewAppUi.h>
#include <eikenv.h>

// CLASS DECLARATION

/**
 * Phonebook 2 application UI base.
 * Common base class for all Phonebook 2 applications.
 * The inheriting class may select the correct base class type.
 */
template <typename CBaseClassType>
class CPbk2AppUiBase : public CBaseClassType,
                       public MPbk2AppUi
    {
    protected: // Constructor and destructor
        inline CPbk2AppUiBase()
                {};
        ~CPbk2AppUiBase()
                {};
    };

#endif // CPBK2APPUIBASE_H

// End of File
