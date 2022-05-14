#include <windows.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <conio.h>


#include "libwmp3.h"

long filesize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, 2);
   length = ftell(stream);
   fseek(stream, curpos, 0);
   return length;

}


CWMp3* mp3;

FILE *wave = 0;
FILE *in = 0;
char *buff = 0;
char *ptr;
 
int __stdcall mp3_callback(unsigned int message, unsigned int param1, unsigned int param2);

int main(int argc, char **argv)
{
	system("CLS"); // clear screen

    mp3 = CreateCWMp3(); // create instance of WMp3 class

//	mp3->SetCallbackFunc(mp3_callback, MP3_MSG_ALL);

//	wave = fopen("wave.pcm", "wb");


/*
	in = fopen(argv[1], "rb");
	long size = filesize(in);
	buff = (char*) malloc(size);
	fread(buff, size, 1, in);
	fclose(in);
	ptr = buff;
*/

	if(mp3 == 0)
	{
		printf("Error: Can't create class instance !\nPress key to exit");
		getch();
		return 0;	
	}

	int ver = mp3->GetVersion();

	printf("LIBWMP3 v.%i.%i based on LIBMAD decoder\r\n\r\n", ver / 100, ver % 100);

    if(argc > 1) 
	{

		printf("x: play\t\tf: fade out\nc: pause\tg: fade in\nv: stop\t\tl: loop\ny: rew\t\tp: equalizer\nb: fwd\t\to: vocal cut\ne: echo\t\tr: reverse ( play song backward )\nm: echo mode\nq: quit\r\n\n");


     	if(!mp3->OpenFile(argv[1],800, 0, 0)) // open mp3 file, wave output buffer 2000 ms
		//if(!mp3->OpenStream(0,1, buff, 20000, 500))
		{
			
        	printf("Error: %s\nPress key to exit", mp3->GetError());
            getch();
            mp3->Release();
            return 0;
        }

	

		ID3_INFO id3_info;
        if(mp3->LoadID3(ID3_VERSION2, &id3_info)) { // loading ID3v1
        	printf("ID3 VERSION 2\r\n\n");
            printf("Title:   %s\r\n", id3_info.Title);
            printf("Artist:  %s\r\n", id3_info.Artist);
            printf("Album:   %s\r\n", id3_info.Album);
            printf("Year:    %s\r\n", id3_info.Year);
            printf("Comment: %s\r\n", id3_info.Comment);
            printf("Genre:   %s\r\n", id3_info.Genre);
			printf("Track:   %s\r\n\r\n", id3_info.TrackNum);
           
        }
		else
		{
			if(mp3->LoadID3(ID3_VERSION1, &id3_info)) { // loading ID3v2
        		printf("ID3 VERSION 1\r\n");
				printf("Title:   %s\r\n", id3_info.Title);
				printf("Artist:  %s\r\n", id3_info.Artist);
				printf("Album:   %s\r\n", id3_info.Album);
				printf("Year:    %s\r\n", id3_info.Year);
				printf("Comment: %s\r\n", id3_info.Comment);
				printf("Genre:   %s\r\n", id3_info.Genre);
				printf("Track:   %s\r\n\r\n", id3_info.TrackNum);
            
			}
		}



     	if(!mp3->Play())
			MessageBox(0, mp3->GetError(), "Error Play",0);


     }
     else
	 {
     // no filename in argument

		mp3->Release(); // delete CWMp3 class
     	printf("Usage: LIBWMP3PLAY.EXE mp3filename\r\n\r\nPress key to exit\r\n");
        getch();
     	return 0;
     }



	MP3_TIME length;
	mp3->GetSongLength(&length);
	MP3_STREAM_INFO pInfo;
	if(!mp3->GetMp3Info(&pInfo))
		printf(mp3->GetError());


	

	char *mpeg_ver = "";
	switch(pInfo.nMPEGVersion)
	{
		case MPEG_1:
			mpeg_ver = "MPEG Version 1";
		break;

		case MPEG_2:
			mpeg_ver = "MPEG Version 2";
		break;

		case MPEG_25:
			mpeg_ver = "MPEG Version 2.5";
		break;
	}

	char *layer_ver = "";
	switch(pInfo.nLayerVersion)
	{
		case LAYER_1:
			layer_ver = "Layer I";
		break;

		case LAYER_2:
			layer_ver = "Layer II";
		break;

		case LAYER_3:
			layer_ver = "Layer III";
		break;
	}


	char *channel_ver = "";

	switch(pInfo.nChannelMode)
	{
		case MODE_DUAL_CHANNEL:
			channel_ver = "Dual channel (Stereo)";
		break;

		case MODE_JOINT_STEREO:
			channel_ver = "Joint stereo (Stereo)";
		break;

		case MODE_STEREO:
			channel_ver = "Stereo";
		break;

		case MODE_SINGLE_CHANNEL:
			channel_ver = "Single channel (Mono)";
		break;
	}


     printf("\r\n%s %s %s %i Hz  Length: %02i:%02i:%02i\r\n", mpeg_ver, layer_ver,
     	channel_ver, pInfo.nSamplingRate,
		length.hms_hour, length.hms_minute, length.hms_second);

     int a;
     int pause = 0;
     char st[10];
	 int echo = 0;
	 int echo_mode = 0;
	 int eq = 0;
	 int vc = 0;
	 int reverse = 0;
	MP3_STATUS status;
	MP3_TIME pos;

     while(1)
	 {
     	mp3->GetStatus(&status); // get current status

		if(status.fPlay)
			strcpy(st,"Playing");
		else if(status.fPause)
			strcpy(st,"Pause  ");
		else
			strcpy(st,"Stop   ");		
			
     
    
		
		mp3->GetPosition(&pos);


     	printf("%s  %03i kbs Position %02i:%02i:%02i  Echo:%i-mode:%i EQ:%i VC:%i FO:%i FI:%i R:%i\r", 
				st, mp3->GetBitrate(1),
        		 pos.hms_hour, pos.hms_minute, pos.hms_second,
				 status.fEcho, status.nSfxMode, status.fExternalEQ, status.fVocalCut, status.fFadeOut, status.fFadeIn,
				 status.fReverse);

	


        if(kbhit())
		{
           	a = getch();
            switch(a)
			{

				case 'r':
					reverse = !reverse;
					mp3->ReverseMode(reverse);
				break;

				case 'o':
					vc = !vc;
					mp3->VocalCut(vc);
				break;

				case 'p':
				{
					eq = !eq;

					if(eq) {
						int eq_param[10];
						memset(eq_param,0,sizeof(eq_param));
						eq_param[4] = 5000; // +5 dB
						eq_param[5] = 10000;// + 10 dB
						eq_param[3] = -4000; // -4 dB
						mp3->SetEQParamEx(1,0, eq_param, 10);
					}
					mp3->EnableEQ(eq, 1);
					
				}
				break;

				case 'l':
				{
					MP3_TIME pCurrentTime;
					mp3->GetPosition(&pCurrentTime);
					MP3_TIME pEndTime;
					pEndTime.ms = pCurrentTime.ms + 2000;
					mp3->PlayLoop(TIME_FORMAT_MS, &pCurrentTime, TIME_FORMAT_MS, &pEndTime ,2);
				}
				break;

				case 'f':
				{
					MP3_TIME ftime;
					ftime.sec = 5;
					mp3->FadeVolumeEx(FADE_OUT, TIME_FORMAT_SEC, &ftime);
				}
				break;

				case 'g':
					MP3_TIME ftime;
					ftime.sec = 5;
					mp3->FadeVolumeEx(FADE_IN, TIME_FORMAT_SEC, &ftime);
				break;

				case 'm':
					echo_mode++;
					if(echo_mode > 9) echo_mode = 0;
            		mp3->SetSfxParam(echo,echo_mode,150,0,0,0);
            	break;
            	
            	case 'e':
					echo = !echo;
            		mp3->SetSfxParam(echo,echo_mode,150,0,0,0);
            	break;
            	
            	case 'x':
					ptr = buff;
					MP3_TIME time;
					mp3->GetPosition(&time);
					if(time.ms == 0 && reverse)
						MessageBox(0, "You are in reverse mode and song is at the beginnning.\nCan't play backward from this position !",0,0);
                	mp3->Play();
                break;

                case 'v':
                	mp3->Stop();
                break;

                case 'c':
                	if(pause) {
                    	mp3->Resume();
                        pause = 0;
                    }
                    else {
                		mp3->Pause();
                        pause = 1;
                    }
                break;

                case 'y':
				{
					MP3_TIME pTime;
					pTime.sec = 5;
                	mp3->Seek(TIME_FORMAT_SEC, &pTime, SONG_CURRENT_BACKWARD);
                    mp3->Play();
				}
                break;

                case 'b':
				{
                	MP3_TIME pTime;
					pTime.sec = 5;
					pTime.hms_hour = 0;
					pTime.hms_minute = 2;
					pTime.hms_second = 3;
					pTime.hms_millisecond = 0;
					pTime.bytes = 3200000;
                	mp3->Seek(TIME_FORMAT_SEC, &pTime, SONG_CURRENT_FORWARD);
                    mp3->Play();;
				}
                break;

                case 'q':
					mp3->Release();
					if(buff)
						free(buff);
					//fclose(wave);
                return 0;

               
            }

        }
        Sleep(200);
	}
}

int __stdcall mp3_callback(unsigned int message, unsigned int param1, unsigned int param2)
{

	switch(message)
	{

		case MP3_MSG_PLAY_ASYNC:
			printf( "MP3_MSG_PLAY_ASYNC\n");
		return 0;

		case MP3_MSG_PLAY:
			printf( "MP3_MSG_PLAY\n");	
		return 0;

		case MP3_MSG_STOP_ASYNC:
			printf( "MP3_MSG_STOP_ASYNC\n");
		break;	

		case MP3_MSG_STOP:
			printf( "MP3_MSG_STOP\n");
		break;

		case MP3_MSG_ENTER_LOOP_ASYNC:
			printf( "MP3_MSG_ENTER_LOOP_ASYNC\n");
		break;

		case MP3_MSG_ENTER_LOOP:
			printf( "MP3_MSG_ENTER_LOOP\n");
		return 0;

		case MP3_MSG_EXIT_LOOP_ASYNC:
			printf( "MP3_MSG_EXIT_LOOP_ASYNC\n");
		break;	

		case MP3_MSG_EXIT_LOOP:
			printf( "MP3_MSG_EXIT_LOOP\n");
		break;

		case MP3_MSG_ENTER_FADE_OUT_ASYNC:
			printf( "MP3_MSG_ENTER_FADE_OUT_ASYNC\n");
		return 0;

		case MP3_MSG_ENTER_FADE_OUT:
			printf( "MP3_MSG_ENTER_FADE_OUT\n");
		return 0;

		case MP3_MSG_EXIT_FADE_OUT_ASYNC:
			printf( "MP3_MSG_EXIT_FADE_OUT_ASYNC\n");
		break;
		
		case MP3_MSG_EXIT_FADE_OUT:
			printf( "MP3_MSG_EXIT_FADE_OUT\n");
		break;	


		case MP3_MSG_ENTER_FADE_IN:
			printf("MP3_MSG_ENTER_FADE_IN\n");
		return 0;

		case MP3_MSG_ENTER_FADE_IN_ASYNC:
			printf("MP3_MSG_ENTER_FADE_IN_ASYNC\n");
		break;

		case MP3_MSG_EXIT_FADE_IN_ASYNC:
			printf( "MP3_MSG_EXIT_FADE_IN_ASYNC\n");
		break;	

		case MP3_MSG_EXIT_FADE_IN:
			printf( "MP3_MSG_EXIT_FADE_IN\n");
		break;	

		case MP3_MSG_WAVE_BUFFER:
		{
			//fwrite((void*) param1, param2, 1, wave);	
			
		}
		return 0;

	
		case MP3_MSG_STREAM_BUFFER_DONE_ASYNC:
			printf( "MP3_MSG_STREAM_BUFFER_DONE_ASYNC: %u  %u\n" , param1, param2);
		break;

		case MP3_MSG_STREAM_BUFFER_DONE:
			printf( "MP3_MSG_STREAM_BUFFER_DONE: %u  %u\n" , param1, param2);	
		break;

		case MP3_MSG_STREAM_OUT_OF_DATA_ASYNC:
			printf( "MP3_MSG_STREAM_OUT_OF_DATA_ASYNC\n");
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
		return 0;
		return 0;	

		case MP3_MSG_STREAM_OUT_OF_DATA:
			printf( "MP3_MSG_STREAM_OUT_OF_DATA\n");
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
			ptr += 20000;
			mp3->PushDataToStream(ptr, 20000);
		return 0;



	}



	return 0;
}
