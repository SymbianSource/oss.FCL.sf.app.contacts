/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the definition of CSpdiaDialogs class.
*
*/


#ifndef SPDIADIALOGS_H
#define SPDIADIALOGS_H

#include <e32base.h>



// Forward declarations
class MVPbkContactLink;
class CVPbkContactManager;
class CSpeedDialPrivate;
/**
 *  This class provides dialogs to be shown when assigning or removing a 
 *  speed dial. When user has accepted the shown dialogs, a speed dial
 *  will be set or removed according to the given parameters.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CSpdiaDialogs ) : public CBase
{
public:
    /**
    * Creates a new instace of this class.
    *
    * @since S60 v3.2
    */
    IMPORT_C static CSpdiaDialogs* NewL( CVPbkContactManager& aContactManager );
        
    /**
    * Destructor.
    */
    virtual ~CSpdiaDialogs();
	
	/**
    * Shows a selection grid dialog for assigning a speed dial.
    * After selecting the index from the grid, speed dial will be set for
    * the given contact.
    *
    * @since S60 v3.2
    * @param aContactLink Contact link pointing to contact and field.
    * @param aIndex On return contains the index where speed dial was assigned to.
    * @return KErrNone if assignment was successfull, otherwise a system wide error code.
    */
   	IMPORT_C TInt ShowAssign( MVPbkContactLink& aContactLink, TInt& aIndex );

    /**
    * Shows necessary dialog(s) to assign a contact to the given speed dial
    * index. After accepting the dialogs and selecting the contact, speed dial
    * will be set to the given index.
    *
    * @since S60 v3.2
    * @param aIndex Speed dial index.
    * @param aContactLink On return contains the contact, for which the speed dial was assigned to.
    *                     Ownership transferres to the caller.
    * @return KErrNone if assignment was successfull, otherwise a system wide error code.
    */
    IMPORT_C TInt ShowAssign( TInt aIndex, MVPbkContactLink*& aContactLink );
    
	/**
    * Shows necessary dialog(s) to remove a speed dial from the given index. 
    * After accepting the dialog(s), speed dial will be removed from the index. 
    *
    * @since S60 v3.2
    * @param aIndex Index of the speed dial to be removed.
    * @return KErrNone if removal was successfull, otherwise a system wide error code.
    */
    IMPORT_C TInt ShowRemove( TInt aIndex );
    
    /**
    * Method is used to notify the cancel event
    * 
    * @since S60  v3.2
    * @return  KErrNone if Cancel is successful.
    */
    IMPORT_C TInt Cancel();
        
private:

	void ConstructL(CVPbkContactManager* aContactManager);
	
	CSpdiaDialogs();
	
 private:
  	
  	CSpeedDialPrivate *iSpeedPrivate;
    
 };

#endif // SEARCHCLIENTSESSION_H
