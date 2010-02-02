/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for extending CCA connection interface. 
*
*/


#ifndef M_MCCACONNECTIONEXT_H
#define M_MCCACONNECTIONEXT_H

#include <e32std.h>
#include <mccaconnection.h>

/** @file mccaconnectionext.h
 *  MCCAConnectionExt is used to extend MCCAConnection functionality.
 *  MCCAConnectionExt can be obtained through use of MCCAParameter's extension
 *  function.
 *  @see MCCAParameter
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class MCCAConnectionExt : public MCCAConnection

    {
public:

    /**
     * Kills CCA application. This is intended only for special purposes 
     * that cannot be handled otherwise. In normal use there is never a
     * need to call this function.
     *
     * @since S60 5.0
     */     
    virtual void CloseAppL() = 0;
    
    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CcaConnectionExtension( 
            TUid /*aExtensionUid*/ ) { return NULL; }   
    };

#endif //M_MCCACONNECTIONEXT_H
// End of File
