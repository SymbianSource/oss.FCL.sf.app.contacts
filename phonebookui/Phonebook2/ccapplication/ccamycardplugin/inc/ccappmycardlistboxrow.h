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
* Description:  Listbox model row for MyCard details list
*
*/

#ifndef CCAPPMYCARDLISTBOXROW_H
#define CCAPPMYCARDLISTBOXROW_H

#include <e32base.h>


/**
 * MyCard view list box model row.
 *
 */
class CCCAppMyCardListBoxRow : public CBase
{
public: // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @return  A new instance of this class.
     */
    static CCCAppMyCardListBoxRow* NewL();

    /**
     * Destructor.
     */
    ~CCCAppMyCardListBoxRow();

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

private: // Implementation

    CCCAppMyCardListBoxRow();

private: // Data

    /// Own: Column array
    RPointerArray<HBufC> iColumns;

    /// Is clip required.
    TBool iClipBeginning;
};

#endif //CCAPPMYCARDLISTBOXROW_H

// End of file
