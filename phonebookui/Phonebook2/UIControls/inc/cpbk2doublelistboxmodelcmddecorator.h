/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Model decorator for double listbox (Social phonebook)
*
*/

#ifndef CPBK2DOUBLELISTBOXMODELCMDDECORATOR_H_
#define CPBK2DOUBLELISTBOXMODELCMDDECORATOR_H_

// internal
#include "cpbk2listboxmodelcmddecorator.h"

class CPbk2ContactViewDoubleListboxDataElement;

/*
 * Decorator for double listbox
 */
NONSHARABLE_CLASS( CPbk2DoubleListboxModelCmdDecorator ) : public CPbk2ListboxModelCmdDecorator
	{
public:	//constructor & destructor
	 /**
	 * Constructs the object.
	 */
	static CPbk2DoubleListboxModelCmdDecorator* NewL(
		const CPbk2IconArray& aIconArray,
		TPbk2IconId aEmptyIconId,
		TPbk2IconId aDefaultIconId );
	
	~CPbk2DoubleListboxModelCmdDecorator();
	
	
public:
	// see CPbk2ListboxModelCmdDecorator
	TPtrC MdcaPoint( TInt aIndex ) const;
	
private:	// new methods
	  /**
	   * Append text into line buffer. Removes 
	   * illegal characters from the aText before appending. aText is subject to
	   * modifications during this call.
	   * 
	   * @param aText text to append into line buffer 
	   */
	   void AppendText( TDes& aText ) const;
	
private:	// constructors
	   
	   void ConstructL();
	   
	   CPbk2DoubleListboxModelCmdDecorator( 
			const CPbk2IconArray& aIconArray,
			TPbk2IconId aEmptyIconId,
			TPbk2IconId aDefaultIconId);

private: // data 

   /// Own. Listbox data element
   CPbk2ContactViewDoubleListboxDataElement* iElement;
};

#endif /* CPBK2DOUBLELISTBOXMODELCMDDECORATOR_H_ */
