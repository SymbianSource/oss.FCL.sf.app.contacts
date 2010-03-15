@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem
@echo off
echo.
makesis -v CRICTestEcomPlugin.pkg
signsis.exe CRICTestEcomPlugin.sis CRICTestEcomPlugin.sisx rd.cer rd-key.pem
set myPassWd=World123
set certKeyName=WorldKey.key
set certName=WorldCert.cer
echo.
echo.

rem makekeys -cert -expdays 10000 -password %myPassWd% -len 2048 -dname "CN=Jeffrey Noone OU=Telecom OR=Digia Plc. C=FI EM=noone@nowhere.com" %certKeyName% %certName%
rem signsis.exe CRICTestEcomPlugin.sis CRICTestEcomPlugin.sisx %certName% %certKeyName% %myPassWd%


