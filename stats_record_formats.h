#ifndef LWS_H
#define LWS_H

#define PLAYER_NAME_LENGTH 50
#define GAME_NAME_LENGTH 30
#define MAX_PITCHES_RECORDED 15
#define PLAYER_NAME_ARY_LEN PLAYER_NAME_LENGTH+1
#define GAME_NAME_ARY_LEN GAME_NAME_LENGTH +1
#define MAX_PITCHES_RECORDED_ARY_LEN MAX_PITCHES_RECORDED+1



// Declare the message structure


//Record at bat
typedef struct atbatbuf {
  long mtype;
  int player_id;//player_id is -1 when game is over
  char pitch_results[MAX_PITCHES_RECORDED_ARY_LEN];

} atbat_buf;

//Return stats

typedef struct statsbuf {
	long mtype;
	int index; //index of players to return
	int count; //total players to print
    int player_id;
    char player_name[PLAYER_NAME_ARY_LEN];
	int strike_outs;
	int walks;
	int singles;
    int doubles;
    int triples;
    int home_runs;
	float game_avg;
    float overall_avg;
} stats_buf;
#endif
