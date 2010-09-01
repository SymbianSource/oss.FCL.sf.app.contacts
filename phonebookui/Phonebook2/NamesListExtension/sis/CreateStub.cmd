rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

CLS
@echo Create the needed files to be able to eclipse Pbk2nameslist view:
del armv5\pbk2_stub.sis
\epoc32\tools\makesis.exe -s -v pbk2_stub.pkg armv5\pbk2_stub.sis
copy armv5\pbk2_stub.sis  \epoc32\data\z\system\install\pbk2_stub.sis
pause
