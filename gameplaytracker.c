#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stats_record_formats.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "queue_ids.h"
#include <errno.h>

int main(int argc, char *argv[]){
	char gameName[GAME_NAME_LENGTH];
	strcpy(gameName, argv[1]);

	//receive player number input
	//receive at bat results, each atbat terminated by (O, S, W, 1, 2, 3, H, P)
	//game end terminated by -1
	int playerNum;

	while(1){
		scanf("%d", &playerNum);


		if(playerNum == -1) {
			//send at_bat with id of -1
			atbat_buf bat;
			bat.player_id = -1;
			int msqid;
    		int msgflg = IPC_CREAT | 0666;
    		key_t key;
    		size_t buf_length;
			key = ftok(CRIMSON_ID,QUEUE_NUMBER);
			msqid = msgget(key, msgflg);
			buf_length = strlen(bat.pitch_results) + sizeof(int)+1;//struct size without
			msgsnd(msqid, &bat, buf_length, IPC_NOWAIT);
			break;
		}
		char cur[3] = "";
	
		atbat_buf at_bat;
		at_bat.mtype = 1;
		at_bat.player_id = playerNum;
		strcpy(at_bat.pitch_results, "");
		while(1){

			scanf("%s", cur);
			if(!strcmp(cur, "W") || !strcmp(cur, "O") || !strcmp(cur, "H") || !strcmp(cur, "P") || 
			   !strcmp(cur, "S") || !strcmp(cur, "1") || !strcmp(cur, "2") || !strcmp(cur, "3")) {
				strcat(at_bat.pitch_results, cur);
				break;
			}
			 strcat(at_bat.pitch_results, cur);

			
		}
			
			//send to player stats tracker va system V queues
			int msqid;
    		int msgflg = IPC_CREAT | 0666;
    		key_t key;
    		size_t buf_length;
			key = ftok(CRIMSON_ID,QUEUE_NUMBER);
			msqid = msgget(key, msgflg);
			buf_length = strlen(at_bat.pitch_results) + sizeof(int)+1;//struct size without
			msgsnd(msqid, &at_bat, buf_length, IPC_NOWAIT);
			
	}

	//wait to receive stats messages
	int cnt = 0;
	while(1){
		int msqid;
		int msgflg = IPC_CREAT | 0666;
		key_t key;
		stats_buf rbuf;
		size_t buf_length;

		key = ftok(CRIMSON_ID,QUEUE_NUMBER);
		msqid = msgget(key, msgflg);
		int ret;
		ret = msgrcv(msqid, &rbuf, sizeof(stats_buf), 2, 0);//receive type 2 message
		if(rbuf.index > 0) cnt++;  
		if(cnt == 1){
			printf("%s: Game Stats\n", gameName);
			printf("Player                    Singles Doubles Triples Homerun Walks   Strike-Outs Batting Avg\n");
			printf("                                                                              Game  Overall\n");
		}
		char name[GAME_NAME_LENGTH + 5]; 
		if(rbuf.game_avg != -1){
			sprintf(name, "%s(#%d)",rbuf.player_name, rbuf.player_id);
			printf("%-26s%-8d%-8d%-8d%-8d%-8d%-12d%-6.3f%-6.3f\n", name, rbuf.singles, rbuf.doubles, rbuf.triples, rbuf.home_runs, rbuf.walks, rbuf.strike_outs, rbuf.game_avg, rbuf.overall_avg);
		}
		//this means that player did not play in the current game
		//if(rbuf.game_avg = -1) continue; 

		if(rbuf.index == rbuf.count) break;
	}
	//print game report

	//if batting avg = -1, dont print them
	//printf("game play tracker\n");

}
