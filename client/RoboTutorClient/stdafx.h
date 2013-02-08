// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include "conio.h"
#include <string>

//#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
	rename("RGB", "MSORGB") \
	rename("DocumentProperties", "MSODocumentProperties")

using namespace Office;

//#import "libid:0002E157-0000-0000-C000-000000000046"
// [-or-]
#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

using namespace VBIDE;

//#import "libid:91493440-5A91-11CF-8700-00AA0060263B" \
//	rename("RGB", "VisioRGB")
// [-or-]
#import "C:\\Program Files\\Microsoft Office\\Office12\\MSPPT.OLB" \
	rename("RGB", "VisioRGB")

#include "PPTController.h"
#include "Client.h"



// TODO: reference additional headers your program requires here
