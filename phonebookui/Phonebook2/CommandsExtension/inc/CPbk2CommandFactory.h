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
* Description:  Phonebook 2 command object factory.
*
*/


#ifndef CPBK2COMMANDFACTORY_H
#define CPBK2COMMANDFACTORY_H

// INCLUDES
#include <e32base.h>
#include <MPbk2CommandFactory.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CPbk2UIExtensionManager;

//  CLASS DECLARATION

/**
 * Phonebook 2 command object factory.
 * Responsible for creating command objects.
 */
NONSHARABLE_CLASS(CPbk2CommandFactory) : public CBase,
                                         public MPbk2CommandFactory
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2CommandFactory* NewL();

        /**
         * Destructor.
         */
        ~CPbk2CommandFactory();

    public: // From MPbk2CommandFactory
        MPbk2Command* CreateCommandForIdL(
                TPbk2CommandId aCommandId,
                MPbk2ContactUiControl& aUiControl ) const;

    public: // Implementation
        MPbk2Command* CreateDeleteContactsCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateGoToURLCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateNewContactCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateEditContactCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateSendMessageCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateSetThumbnailCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateAssignDefaultsCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateRemoveThumbnailCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateAddImageCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateViewImageCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateChangeImageCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateRemoveImageCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateAssignSpeedDialCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateRemoveSpeedDialCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateSendContactCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateCopyContactsStarterCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateCopyNumberToClipboardCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateLaunchInfoDialogCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateSendMessageNoQueryCommandL(
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateMergeContactsCommandL(
                MPbk2ContactUiControl& aUiControl ) const;

        MPbk2Command* CreateCopyDetailToClipboardCommandL(
                        MPbk2ContactUiControl& aUiControl ) const;
        MPbk2Command* CreateCopyAddressToClipboardCommandL(
                        MPbk2ContactUiControl& aUiControl ) const;
//RCL_ADD        
        MPbk2Command* CreateRclCommandL(
                        MPbk2ContactUiControl& aUiControl ) const;
        
    private: // Implementation
        CPbk2CommandFactory();
    };

#endif // CPBK2COMMANDFACTORY_H

// End of File
