package edu.cs300;

import java.util.concurrent.*;
import java.util.*;
import java.io.File;
import java.io.FileNotFoundException;

public class PlayerStatsTracker {
    
    HashMap<Integer,ArrayBlockingQueue<AtBatPitchResults>> players;
    ArrayBlockingQueue<String> resultsOutputArray;
    ArrayBlockingQueue<AtBatPitchResults> queue16;
    ArrayBlockingQueue<AtBatPitchResults> queue33;
    int playerCount=0;
    
    
    public PlayerStatsTracker(){
        this.players = new HashMap<Integer,ArrayBlockingQueue<AtBatPitchResults>>();
        this.resultsOutputArray = new ArrayBlockingQueue<String>(30);

        try { 
            File file = new File ("players.txt");//finish processing of players and players history here
            Scanner sc = new Scanner(file);
            while (sc.hasNextLine()) {
                String data = sc.nextLine();
                String pname = data.split(",")[0];
                int pnum = Integer.parseInt(data.split(",")[1]);
                this.players.put(pnum, new  ArrayBlockingQueue<AtBatPitchResults>(10));
                new Worker(pnum, this.players.get(pnum), this.resultsOutputArray, pname).start();
                playerCount++;
            }

        } catch (Exception ex) {
          System.err.println("FileNotFoundException triggered:"+ex);
        }
    }
    
  public static void main(String[] args) throws FileNotFoundException {
      
      
      PlayerStatsTracker tracker=new PlayerStatsTracker();
      try {
        tracker.run();//Not a thread; run() is just convenient naming
      } catch   (InterruptedException e){
          System.err.println("InterruptedException:"+e);
      }
  }
    
 void run() throws InterruptedException {

        while (true) {
            //process incoming pitch results
            AtBatPitchResults result = MessageJNI.readAtBatPitchResultsMsg();
            if(result.gameOver()) break;
            this.players.get(result.playerID).put(result);
        }

        AtBatPitchResults sentinel=new AtBatPitchResults();
        for (ArrayBlockingQueue<AtBatPitchResults> queue : this.players.values()) {
            queue.put(sentinel);
        }

        //wait for all threads to complete
        int count=1;
        while (count <=playerCount ){
            String gameStatsMsg= resultsOutputArray.take();
            
            
            String[] stats = gameStatsMsg.split(",");
        
                MessageJNI.writePlayerFinalStatsMsg(Integer.parseInt(stats[0]), count, playerCount, stats[1], Integer.parseInt(stats[2]), Integer.parseInt(stats[3]), Integer.parseInt(stats[4]), Integer.parseInt(stats[5]), Integer.parseInt(stats[6]), Integer.parseInt(stats[7]), Double.parseDouble(stats[8]), Double.parseDouble(stats[9]));
              
            //test message; map fields from resultsOutputArray to this message

            count++;     
        }               
    }
  
}
