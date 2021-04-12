#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "stats_record_formats.h"
#include "queue_ids.h"

int main(int argc, char**argv)
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    stats_buf rbuf;
    size_t buf_length;

    key = ftok(CRIMSON_ID,QUEUE_NUMBER);
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error msgget: %s\n", strerror( errnum ));
    }
    else
        fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);


    // msgrcv to receive message
    int ret;
    do {
      ret = msgrcv(msqid, &rbuf, sizeof(stats_buf), 2, 0);//receive type 2 message
      int errnum = errno;
      if (ret < 0 && errno !=EINTR){
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
      }
    } while ((ret < 0 ) && (errno == 4));
    //fprintf(stderr,"msgrcv error return code --%d:$d--",ret,errno);

    fprintf(stderr,"msgrcv-stats: msg type-%ld, Player stats %d of %d for %s(#%d) strikeouts=%d, walks=%d, singles=%d, doubles=%d, triples=%d, home runs=%d, game avg=%lf overall avg=%lf ret/bytes rcv'd=%d\n", rbuf.mtype, rbuf.index,rbuf.count,rbuf.player_name,rbuf.player_id, rbuf.strike_outs,  rbuf.walks,  rbuf.singles,  rbuf.doubles,  rbuf.triples,  rbuf.home_runs,  rbuf.game_avg, rbuf.overall_avg,ret);

    exit(0);
}
