/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <CPbk2AppUiBase.h>

namespace Phonebook2 {

// --------------------------------------------------------------------------
// CPbk2AppUiBase::Pbk2AppUi
// Selects and returns correct application UI.
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2AppUi* Pbk2AppUi()
    {
    MPbk2AppUi* ret = NULL;

    CEikAppUi* const eikAppUi = CEikonEnv::Static()->EikAppUi();

    // Casting has to be done in two steps since dynamic cast has problems
    // casting EikAppUi directly to the templated type. Therefore dynamic
    // cast is used to cast to the base class type and then we proceed
    // with static cast as usual.
    if ( dynamic_cast<CAknViewAppUi*>( eikAppUi ) )
        {
        ret = static_cast<CPbk2AppUiBase<CAknViewAppUi>*>
            ( eikAppUi );
        }
    else
        {
        ret = static_cast<CPbk2AppUiBase<CAknAppUi>*>
            ( eikAppUi );
        }

    return ret;
    }

} /// namespace

// End of File
