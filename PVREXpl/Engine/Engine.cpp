// Engine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Platform.h"
#include "GlobalDefs.h"
#include "EnumPVRDrives.h"
#include "DishPvr.h"
#include "DishPVR50X.h"
#include "DishPVR522.h"
#include "DishPVR721.h"
#include "ExtractProgram.h"

//------------------------- engin test code ------------------------
int main(int argc, char* argv[])
{

	EnumPVRDrives	DriveEnum;
	int				j;
	char			DriveName[512];
	char			input[32];
	int				DriveSelected;
	DishPvr			*pvr;
	DishPVR50X		*pvr50x;
	DishPVR522		*pvr522;
	DishPVR721		*pvr721;
	int				NumPrograms;
	int				ProgramSelected;
	ExtractProgram	*ext;
	WhichStreams	strm;

	printf("\n----------------------------------------------------------------------");
	printf("\n PVRExplorer2 Platform test code:");
	printf("\n\nPVRExplorer> Scanning for PVR HDDs....");


	DriveEnum.ScanForDrives();
	if( DriveEnum.GetNumberOfPVRDrives() == 0 ){
		printf("\nPVRExplorer> No Pvr Hard drives found!");
		printf("\nPVRExplorer> Enter HDD Image file Path:");
		scanf("%s",DriveName);
		if( DriveName[0] = 0 )
			exit(1);
		printf("\nPVRExplorer> Enter HDD Image file Type (1- 50x, 2-522, 3-721:");
		scanf("%d",&j);
		DriveEnum.AddPVRDrive(DriveName,j);
	}
	for( j = 0; j < DriveEnum.GetNumberOfPVRDrives(); j++ ){
		DriveEnum.GetDriveFriendlyName(	DriveName,j);
		printf("\nPVRExplorer> #%d : %s - found",j + 1,DriveName);
	}
	if( DriveEnum.GetNumberOfPVRDrives() == 1 ){
		printf("\nPVRExplorer> Only One Drive found, Selecting drive #1");
		DriveSelected = 0;
	} else {
		printf("\nPVRExplorer> Select Drive #:");
		scanf("%s",input);
		if( input[0] >= '0' && input[0] <= '9' ){
			DriveSelected = input[0] - '0';
			DriveSelected--;
		} else {
			printf("\nPVRExplorer> Invalid Drive");
			exit(2);
		}
	}
	//---------------- ok now create the pvr object acording to the type enumerated.
	pvr = NULL;
	DriveEnum.GetDriveName(	DriveName,DriveSelected);
	if( DriveEnum.GetPVRDriveType( DriveSelected ) == PVR_TYPE_50X ){
		pvr50x = (DishPVR50X *)new DishPVR50X;
		pvr50x->SetPvrHdd( DriveName, PVR_TYPE_50X);
		pvr = (DishPvr *)pvr50x;
	}
	if( DriveEnum.GetPVRDriveType( DriveSelected ) == PVR_TYPE_522 ){
		pvr522 = (DishPVR522 *)new DishPVR522;
		pvr522->SetPvrHdd( DriveName, PVR_TYPE_522);
		pvr = (DishPvr *)pvr522;
	}
	if( DriveEnum.GetPVRDriveType( DriveSelected ) == PVR_TYPE_721 ){
		pvr721 = (DishPVR721 *)new DishPVR721;
		pvr721->SetPvrHdd( DriveName, PVR_TYPE_721);
		pvr = (DishPvr *)pvr721;
	}
	if( pvr == NULL ){
		printf("\nPVRExplorer> Failed to create pvr instance!, Exiting...");
		exit(3);
	}
	NumPrograms = pvr->GetNumberOfPrograms();
	printf("\nPVRExplorer> %d Program(s) found...",NumPrograms);
	ProgramSelected = 0;
	//-------------------------------------------------------------------------
	//now enter program loop
	while( 1 ) {
		char	Name[128];
		char	DateTime[64];
		char	Length[32];
		char	ChannelName[16];
		int		Channel,j,k;
		char	Description[400];
		STREAM_INFO		sInfo;

		int		h,m,s,f;
		double	x;

		printf("\nPVRExplorer> LS - List programs, IF # - Display info on program, EX # - Extract program, QT - Quit");
		printf("\nPVRExplorer> ");
		ProgramSelected = 0;
		scanf("%32s",input);
		if( !stricmp(input,"QT") )
			break;
		if( !stricmp(input,"LS") ){
			printf("\nPVRExplorer> -------------------------------------------------------------------------");
			printf("\nPVRExplorer> PVR Programs list:");
			for( j = 0; j < NumPrograms; j++){
				Name[0] = DateTime[0] = Length[0] = 0;
				pvr->GetProgramInfo(Name,DateTime,Length,NULL,NULL,NULL,j);
				printf("\nPVRExplorer> #%d : %32s %16s %16s",j,Name,DateTime,Length);
			}
			printf("\nPVRExplorer>");
		}
		if( !stricmp(input,"IF") ){
			scanf("%d",&ProgramSelected);
			printf("\nPVRExplorer> -------------------------------------------------------------------------");
			Name[0] = DateTime[0] = Length[0] = ChannelName[0] = Channel = Description[0] = 0;
			pvr->GetProgramInfo(Name,DateTime,Length,ChannelName,&Channel,Description,ProgramSelected);
			printf("\nPVRExplorer> PVR Programs Information:");
			printf("\nPVRExplorer> #%d : %32s %16s %16s",ProgramSelected,Name,DateTime,Length);
			printf("\nPVRExplorer>     : %04d %32s %s",Channel,ChannelName,Description);
			printf("\nPVRExplorer>");
		}
		if( !stricmp(input,"EX") ){
			scanf("%d",&ProgramSelected);
			printf("\nPVRExplorer> -------------------------------------------------------------------------");
			printf("\nPVRExplorer> Extract PVR Program #%d...",ProgramSelected);
			ext = (ExtractProgram *)new ExtractProgram;
			strm.m_RenderChk = true;
			strm.m_AudioChk = false;
			strm.m_VideoChk = false;
			strcpy((char *)strm.m_DefaultPath,"C:\\");
			ext->StartExtractProcess(pvr,ProgramSelected,&strm,"TestFile");
			Sleep(1000);
			printf("\nPVRExplorer> ");
			j = 0;
			while(1){
				Sleep(1000);
				ext->GetProgressInfo(&sInfo);
				if( sInfo.Status == EXTRACT_STATUS_IDLE ){
					printf("\nPVRExplorer> Program extracted completed:");
					break;
				}
				if( sInfo.Status == EXTRACT_STATUS_CANCELED ){
					printf("\nPVRExplorer> Program extracted CANCELED!");
					break;
				}
				if( sInfo.Status == EXTRACT_STATUS_ERROR ){
					printf("\nPVRExplorer> Program extracted FAILED!");
					break;
				}
				Description[0] = 0x00;
				for(k = 0; k < j; k++)
					strcat(Description,"\b");
				printf(Description);
				
				x = (double)sInfo.FrameNumber / sInfo.VideoRate;
				h = (int)x / (60 * 60);
				x -= h * (60 * 60);
				m = (int)x / 60;
				x -= m * 60;
				s = (int)x;
				x -= s;
				f = (int)(x * sInfo.VideoRate);
				
				sprintf(Description,"Video: %dx%d %02.02fFps time: %d:%02d:%02d:%02d Audio: %s %dKbs offset:%f Bytes:%I64d",
						sInfo.VideoX,sInfo.VideoY,sInfo.VideoRate,h,m,s,f,
						sInfo.AudioType,sInfo.AudioRate,sInfo.AudioOffset,
						sInfo.TotalBytesProcessed);
				printf(Description);
				j = strlen(Description);
			}
			delete ext;
		}
	}
	delete pvr;
	
	return 0;
}
