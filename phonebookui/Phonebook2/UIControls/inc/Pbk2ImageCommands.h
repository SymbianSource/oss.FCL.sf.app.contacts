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


#ifndef PBK2IMAGECOMMANDS_H
#define PBK2IMAGECOMMANDS_H

// FORWARD DECLARATIONS
class MPbk2BaseCommand;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MVPbkFieldType;
class MPbk2ImageFieldObserver;

/**
 * Phonebook 2 image commands are used to change contact image and thumbnail
 * fields and operate on a locked store contact.
 * Usually image field contains the filename of an original image and thumbnail field
 * contains its scaled down binary version. It's possible that a contact has 
 * only thumbnail field, e.g. when it was imported as vCard. 
 * 
 * All commands that return a MPbk2BaseCommand* handle may run asynchronously.
 * The handle should be deleted to cancel the running command.
 * 
 * NOTE: Some command functions may return NULL, it is a valid value and means
 * that command run synchronously. 
 */
class Pbk2ImageCommands
    {
    public:

    	/**
         * Assigns an image to a contact.
         * 
         * Runs select image dialog and assign it to the contact.
         *
         * @param aContact		Locked store contact.

         * @return  A command handle.
         */    	
    	static MPbk2BaseCommand* SetImageL(MVPbkStoreContact* aContact,MPbk2ImageFieldObserver* aObserver);

    	/**
         * Displays image via external image viewer.
         *
         * @param aContact		Locked store contact.

         * @return  A command handle.
         */    	
    	static MPbk2BaseCommand* ViewImageL(MVPbkStoreContact* aContact);
    	
        /**
         * Removes an image.
         * 
         * @param aContact		Locked store contact.
         * 
         * @return  A command handle.
         */    	
    	static MPbk2BaseCommand* RemoveImageL(MVPbkStoreContact* aContact);

        /**
         * Returns ETrue if aField is image field.
         *
         * @param aField		Store contact field.
         * 
         * @return  ETrue if aField is image field, EFalse otherwise
         */    	
    	static TBool IsImageFieldL(const MVPbkStoreContactField& aField);

        /**
         * Returns ETrue if aField is thumbnail field.
         *
         * @param aField		Store contact field.
         * 
         * @return  ETrue if aField is thumbnail field, EFalse otherwise
         */    	
    	static TBool IsThumbnailFieldL(const MVPbkStoreContactField& aField);
    	
        /**
         * Returns ETrue image field points to a valid ringtone file.
         *
         * @param aField		Store contact field.
         * 
         * @return  ETrue if aField contains a valid ringtone filed, EFalse otherwise
         */    	
    	static TBool IsImageFieldValidL(const MVPbkStoreContactField& aField);
    };

#endif // PBK2IMAGECOMMANDS_H

// End of File
