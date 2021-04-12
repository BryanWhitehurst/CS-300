#include "edu_cs300_MessageJNI.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <jni.h>
#include "stats_record_formats.h"
#include "queue_ids.h"


#ifndef darwin
size_t                  /* O - Length of string */
strlcpy(char       *dst,        /* O - Destination string */
        const char *src,      /* I - Source string */
        size_t      size)     /* I - Size of destination string buffer */
{
    size_t    srclen;         /* Length of source string */


    /*
     * Figure out how much room is needed...
     */

    size --;

    srclen = strlen(src);

    /*
     * Copy the appropriate amount...
     */

    if (srclen > size)
        srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}

#endif

/*
 * Class:     edu_cs300_MessageJNI
 * Method:    writePlayerFinalStatsMsg
 * Signature: (IIILjava/lang/String;IIIIIIDD)V
 */
JNIEXPORT void JNICALL Java_edu_cs300_MessageJNI_writePlayerFinalStatsMsg
  (JNIEnv *env, jclass obj, jint player_id, jint player_idx, jint player_cnt, jstring nameStr, jint strike_outs, jint walks, jint singles, jint doubles, jint triples, jint home_runs, jdouble gavg,jdouble oavg){


    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    stats_buf rbuf;

    key = ftok(CRIMSON_ID,QUEUE_NUMBER);
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
    }
    //else
        //fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);

    const char *player_name = (*env)->GetStringUTFChars(env,nameStr, NULL);
    int name_len = strlen(player_name);
    int buffer_length=sizeof(stats_buf)-sizeof(long); //int


    //fprintf(dev/null,"JNI msgsnd-stats: Record %d of %d: %s(#%d) strikeouts=%d, walks=%d, singles=%d, doubles=%d, triples=%d, home runs=%d, game avg=%lf, overall avg=%lf",player_idx,player_cnt,player_name,player_id, strike_outs,  walks,  singles,  doubles,  triples,  home_runs,  gavg,oavg);


    // // We'll send message type 2
    rbuf.mtype = 2;
    rbuf.index=player_idx; //index of response
    rbuf.count=player_cnt; //total excerpts available
    int index; //index of players to return
    int count; //total players to print
    rbuf.player_id=player_id;
    strlcpy(rbuf.player_name,player_name,PLAYER_NAME_ARY_LEN);
    rbuf.strike_outs=strike_outs;
    rbuf.walks=walks;
    rbuf.singles=singles;
    rbuf.doubles=doubles;
    rbuf.triples=triples;
    rbuf.home_runs=home_runs;
    rbuf.game_avg=gavg;
    rbuf.overall_avg=oavg;


    //
    // Send a message.
    if((msgsnd(msqid, &rbuf, buffer_length, IPC_NOWAIT)) < 0) {
        int errnum = errno;

        perror("(msgsnd)");
        fprintf(stderr, " resulted in error sending msg: %s\n", strerror( errnum ));
        exit(1);
    }
    else
        //fprintf(stderr," successfully sent\n");


    (*env)->ReleaseStringUTFChars(env, nameStr, player_name);

}

/*
 * Class:     edu_cs300_MessageJNI
 * Method:    readAtBatPitchResultsMsg
 * Signature: ()Ledu/cs300/AtBatPitchResults;
 */
JNIEXPORT jobject JNICALL Java_edu_cs300_MessageJNI_readAtBatPitchResultsMsg
  (JNIEnv *env, jclass obj) {
    

    key_t key;
    int msqid;
    atbat_buf atbat;


    // ftok to generate unique key
    //key = ftok(CRIMSON_ID, QUEUE_NUMBER);
    key = ftok(CRIMSON_ID, QUEUE_NUMBER);

    // msgget creates a message queue
    // and returns identifier
    msqid = msgget(key, 0666 | IPC_CREAT);

    // msgrcv to receive message
    int ret = msgrcv(msqid, &atbat, sizeof(atbat_buf), 1, 0);//TODO what is the correct length here
    if (ret < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
        
    }
    //fprintf(stderr,"JNI msgrcv-atbat: Player #%d pitch results=%s ret bytes rcv'd %d\n",atbat.player_id,atbat.pitch_results,ret);

    // Create the object of the class UserData
    jclass AtBatPitchResultsClass = (*env)->FindClass(env,"edu/cs300/AtBatPitchResults");

    jstring result = (*env)->NewStringUTF(env,atbat.pitch_results);
    jmethodID constructor = (*env)->GetMethodID(env, AtBatPitchResultsClass, "<init>", "(ILjava/lang/String;)V");
    jobject secondObject = (*env)->NewObject(env, AtBatPitchResultsClass, constructor,atbat.player_id, result);

    return secondObject;

}

JNIEXPORT jstring JNICALL Java_edu_cs300_MessageJNI_readStringMsg
(JNIEnv *env, jobject obj) {

    key_t key;
    int msqid;
    char rbuf[100];
    int msgflg = IPC_CREAT | 0666;

    // ftok to generate unique key
    key = ftok(CRIMSON_ID, QUEUE_NUMBER);

    // msgget creates a message queue
    // and returns identifier
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error msgget: %s\n", strerror( errnum ));
    }
    else
        fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);

    // msgrcv to receive message
    int ret = msgrcv(msqid, &rbuf, 100, 1, 0);//TODO what is the correct length here
    if (ret < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
        strcpy(rbuf,"error");//return error to java program
    }

    jstring result;

    puts(rbuf);
    result = (*env)->NewStringUTF(env,rbuf);
    return result;
}


