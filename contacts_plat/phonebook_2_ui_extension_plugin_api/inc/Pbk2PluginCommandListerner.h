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
* Description:  Phonebook 2 UI extension ECom plug-in interface.
*
*/


#ifndef PBK2PLUGINCOMMANDLISTENER_H
#define PBK2PLUGINCOMMANDLISTENER_H

// INCLUDE FILES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Phonebook 2 command handler interface.
 * 
 * Allows CPbk2UIExtensionPlugin classes to also handle commands
 * Used for handling specific commands such as phonebook exiting to
 * background. The commands are from the view base class.
 */
class MPbk2PluginCommandListerner
	{
    public:
	
	/*
	* Hanlde command from the view base class.
	*/
	virtual void HandlePbk2Command( TInt aCommand ) = 0;
	
	};
#endif //PBK2PLUGINCOMMANDLISTENER_H
