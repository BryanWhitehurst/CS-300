#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
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

int main(int argc, char**argv)
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    atbat_buf sbuf;
    size_t buf_length;

    //printf("ftok\n");
    key = ftok(CRIMSON_ID,QUEUE_NUMBER);
    //printf("open queue\n");
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error msgget: %s\n", strerror( errnum ));
    }
    else
        fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);

    // We'll send message type 1
    sbuf.mtype = 1;
    sbuf.player_id=16;//player_id is -1 when game is over
    memset(sbuf.pitch_results,'\0',MAX_PITCHES_RECORDED_ARY_LEN);
    strlcpy(sbuf.pitch_results,"BKBKFFBO",MAX_PITCHES_RECORDED);
    buf_length = strlen(sbuf.pitch_results) + sizeof(int)+1;//struct size without    
    // Send a message.
    if((msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT)) < 0) {
        int errnum = errno;
        fprintf(stderr,"%d, %ld, %d, %d\n", msqid, sbuf.mtype, sbuf.player_id, (int)buf_length);
        perror("(msgsnd)");
        fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
        exit(1);
    }
    else
        fprintf(stderr,"msgsnd-atbat: player id\"#%d\" Sent (%d bytes)\n", sbuf.player_id,(int)buf_length);

    exit(0);
}

