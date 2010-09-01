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
* Description:  A class responsible handling the views
 *
*/



#ifndef MCCACONTACTOBSERVER_H_
#define MCCACONTACTOBSERVER_H_

/**
 * CCA plugin contact observer interface
 *
 * Plugins recieve notifications from the MCCAppEngine once one of the
 * plugins has caused a contact to relocate from one store to another
 */
class MCCAppPluginsContactObserver
    {
public: // Types
    
    /**
     * Notify all the observers that the contact has been relocated.
     * To obtain the new contact, the MCCAppEngine provides a Parameter()
     * helper function for this purpose.
     * 
     * @since S60 v5.0
     */
     virtual void NotifyPluginOfContactEventL() = 0;
    
    };

#endif /*MCCACONTACTOBSERVER_H_*/
