// ExtractCopy.cpp: implementation of the ExtractCopy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtractCopy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __MS__
DWORD WINAPI CopyThreadProc(void *Param);
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExtractCopy::ExtractCopy()
{
	m_ThreadDone = false;

}

ExtractCopy::~ExtractCopy()
{

}

void ExtractCopy::StartExtractProcess(DishPvr *pvr, int ProgramNumber, char *DefPath, char *FileNameBase)
{
	char	SourcePath[360];
#ifdef __MS__
	unsigned long	ThreadID;
#endif

	
	m_pvr = pvr;
	m_Source = m_pvr->OpenVideoFile(ProgramNumber);
	if( m_Source == NULL )
		return;
	m_TotalSize = m_pvr->GetPvrFileSize(m_Source);
	m_CurrentSize = 0;
	strcpy(SourcePath,DefPath);
	strcat(SourcePath,FileNameBase);
	m_Destination = CreateFile(SourcePath,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if( m_Destination == INVALID_HANDLE_VALUE ){
		m_pvr->ClosePvrFile(m_Source);
		return;
	}
	m_ThreadDone = false;
#ifdef __MS__
	//create the thread
	m_ThreadHandle = CreateThread(NULL,0,CopyThreadProc,(void *)this,0,&ThreadID);
#endif


}

int ExtractCopy::GetProgressPercent()
{
	int		rc;

	if( m_ThreadDone )
		return(101);
	rc = (int)(((double)m_CurrentSize / (double)m_TotalSize) * (double)100);
	return(rc);
}

#define CPY_BUFFER_SIZE	65536

//***************************************************************************************
#ifdef __MS__
DWORD WINAPI 
#endif
CopyThreadProc(void *Param)
{
	ExtractCopy *dlg;
	unsigned char	*Buffer;
	unsigned long	x;


	dlg = (ExtractCopy *)Param;
	Buffer = (unsigned char *)new char[CPY_BUFFER_SIZE];
	while( dlg->m_CurrentSize < dlg->m_TotalSize ){
		x = dlg->m_pvr->ReadPvrFile(dlg->m_Source,Buffer,CPY_BUFFER_SIZE);
		WriteFile(dlg->m_Destination,Buffer,x,&x,NULL);
		dlg->m_CurrentSize += x;
		if( x == 0 )
			break;
	}
	delete []Buffer;
	dlg->m_ThreadDone = true;
	dlg->m_pvr->ClosePvrFile(dlg->m_Source);
	CloseHandle(dlg->m_Destination);
	return(0);
}
