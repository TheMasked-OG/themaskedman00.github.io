// ExtractCopy.h: interface for the ExtractCopy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTRACTCOPY_H__A65F53D3_9FA7_4992_9402_C8DE7FD81136__INCLUDED_)
#define AFX_EXTRACTCOPY_H__A65F53D3_9FA7_4992_9402_C8DE7FD81136__INCLUDED_

#include "..\engine\Platform.h"
#include "GlobalDefs.h"
#include "DishPvr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ExtractCopy  
{
public:
	void StartExtractProcess(DishPvr *pvr, int ProgramNumber, char *FileNameBase);
	ExtractCopy();
	virtual ~ExtractCopy();

};

#endif // !defined(AFX_EXTRACTCOPY_H__A65F53D3_9FA7_4992_9402_C8DE7FD81136__INCLUDED_)
