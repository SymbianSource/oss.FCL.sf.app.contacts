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
* Description:  Listbox model row contact model
*
*/

#ifndef CSPBCONTACTDATAMODELROW_H
#define CSPBCONTACTDATAMODELROW_H

#include <e32base.h>
#include <TPbk2IconId.h>

class RWriteStream;
class RReadStream;


/**
 * MyCard view list box model row.
 *
 */
NONSHARABLE_CLASS( CSpbContactDataModelRow ) : public CBase
{
public: // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @return  A new instance of this class.
     */
    static CSpbContactDataModelRow* NewL();

    /**
     * Destructor.
     */
    ~CSpbContactDataModelRow();

public: // Interface

    /**
     * Returns the number of columns in this row.
     *
     * @return  Number of columns in this row.
     */
    TInt ColumnCount() const;

    /**
     * Returns the descriptor at the given position.
     *
     * @param aColumnIndex  Column index.
     * @return  The descriptor at the given index.
     */
    TPtrC At(TInt aColumnIndex) const;

    /**
     * Appends a given column to the row.
     *
     * @param aColumnText   Column to append.
     */
    void AppendColumnL(const TDesC& aColumnText);

    /**
     * replace column's text with a new one.
     *
     * @param aIndex        Column index
     * @param aColumnText   new text
     */
    void ReplaceColumnL( TInt aIndex, const TDesC& aColumnText );
    
    /**
     * Is clip required.
     */
    TBool IsClipRequired() const;

    /**
     * Clip text from beginning if field is numeric field or e-mail field.
     */
    void SetClipRequired(TBool aFlag);

    /**
     * Returns the total descriptor length of all the columns.
     *
     * @return  Total row length.
     */
    TInt TotalLength() const;

    /**
     * Returns the maximum column length of this row.
     *
     * @return  Maximum column length.
     */
    TInt MaxColumnLength() const;

    /**
     * Externalize row into stream
     */
    void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalize row from stream
     */
    void InternalizeL( RReadStream& aStream );
    
    /**
     * 
     */
    const TPbk2IconId& Icon();
    
    /**
     * 
     */
    void SetIcon( const TPbk2IconId& aIcon );
    
private: // Implementation

    CSpbContactDataModelRow();

private: // Data

    /// Own: Column array
    RPointerArray<HBufC> iColumns;

    /// Is clip required.
    TBool iClipBeginning;
    
    /// Icon ID
    TPbk2IconId iIcon;
};

#endif //CSPBCONTACTDATAMODELROW_H

// End of file
