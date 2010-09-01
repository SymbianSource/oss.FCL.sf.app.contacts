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
* Description:  Phonebook 2 ringing commands.
*
*/


#ifndef PBK2RINGTONECOMMANDS_H
#define PBK2RINGTONECOMMANDS_H

// FORWARD DECLARATIONS
class MPbk2BaseCommand;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MVPbkFieldType;

/**
 * Phonebook 2 ringtone commands are used to change contact ringtone field and
 * operate on a locked store contact.
 * All commands that return a MPbk2BaseCommand* handle may run asynchronously.
 * The handle should be deleted to cancel the running command.
 * 
 * NOTE: Some command functions may return NULL, it is a valid value and means
 * that command run synchronously.
 */
class Pbk2RingtoneCommands
    {
    public:

    	/**
         * Assigns a ringtone to a contact.
         * 
         * Runs select ringtone dialog and assign selected ringtone file
         * to the contact. If the contact already has a ringtone, it will
         * be preselected in the ringtone dialog. If default ringtone is
         * selected, ringtone field will be set empty.
         *
         * @param aContact		Locked store contact.

         * @return  A command handle.
         */    	
    	static MPbk2BaseCommand* SetRingtoneL(MVPbkStoreContact* aContact);

        /**
         * Removes a ringtone.
         * 
         * Runs a confirmation dialog and set ringtone field empty.
         * 
         * @param aContact		Locked store contact.
         * 
         * @return  A command handle.
         */    	
    	static MPbk2BaseCommand* RemoveRingtoneL(MVPbkStoreContact* aContact);

        /**
         * Returns ETrue if aField is a ringtone field.
         *
         * @param aField		Store contact field.
         * 
         * @return  ETrue if aField is a ringtone field, EFalse otherwise
         */    	
    	static TBool IsRingtoneFieldL(const MVPbkStoreContactField& aField);
    	
        /**
         * Returns ETrue if the ringtone field points to a valid ringtone file.
         *
         * @param aField		Store contact field.
         * 
         * @return  ETrue if aField contains a valid ringtone filed, EFalse otherwise
         */    	
    	static TBool IsRingtoneFieldValidL(const MVPbkStoreContactField& aField);
    };

#endif // PBK2SETTONEBASE_H

// End of File
