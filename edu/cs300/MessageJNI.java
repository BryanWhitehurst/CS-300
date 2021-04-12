package edu.cs300;

import java.util.concurrent.*;


//statements only relevant on Mac; Use statements on readme instead
//javac -h . MessageJNI.java
//gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/darwin system5_msg.c -o edu_cs300_MessageJNI.o
//gcc -dynamiclib -o libsystem5msg.dylib edu_cs300_MessageJNI.o -lc
//java -cp . -Djava.library.path=. edu.cs300.MessageJNI

public class MessageJNI {

    static {
        System.loadLibrary("system5msg");
    }

    public static void main(String[] args) {
        try{
        //System.out.println(new MessageJNI().readStringMsg("anderson",65));
        System.out.println("Running MessageJNI test routine");
        System.out.println(new MessageJNI().readAtBatPitchResultsMsg());

        new MessageJNI().writePlayerFinalStatsMsg( 16, 1, 1, "Bailey Hemphill", 1, 3, 1, 0, 0, 3,0.5,0.25);
        } catch (Exception e){
            System.err.println("Error:"+e);
        }
    }
    

    //when you read in players.txt you have access to every player on team. no way for C side to know how many players on team
    //if 16 players, send 16 messages
    //stat-record-id is num
    //statrecordcount = total num of statistics
    //statrecordID = which record /16 you are referring
    public static native void writePlayerFinalStatsMsg(int playerID, int statRecordID, int statRecordCount, String PlayerName, int strike_outs, int walks, int singles, int doubles, int triples, int homeRuns, double gameBattingAverage,double overallBattingAverage);

    public static native AtBatPitchResults readAtBatPitchResultsMsg();

    // Declare a native method sayHello() that receives no arguments and returns void
    private static native String readStringMsg();

}
