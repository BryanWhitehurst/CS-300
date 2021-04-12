package edu.cs300;

import java.util.concurrent.*;
import java.util.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;

class Worker extends Thread{

  ArrayBlockingQueue<AtBatPitchResults> incomingStats;
  ArrayBlockingQueue outgoingResults;
  Integer id;
  String name; 


  //stats 0 = atBats, 1 = outs, 2 = singles, 3 = doubles, 4 = triples, 5 = home runs, 6 = walks, 7 = strikeouts, 8 = hits by pitch
  // 9 = playerID, 10 = playerName, 11 = battingAvg
  public Worker(Integer id,ArrayBlockingQueue<AtBatPitchResults> incomingStats, ArrayBlockingQueue outgoingResults, String playerName){
    this.incomingStats=incomingStats;
    this.outgoingResults=outgoingResults;
    this.id=id;
    this.name=playerName;//put name of player here

  }

  public void run() {
    //DebugLog.log("Player Tracker-"+this.id+" ("+this.name+") thread started ...");

    int[] stats = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    while (true){
      try {
        AtBatPitchResults atBat = (AtBatPitchResults)this.incomingStats.take();
        if(!atBat.gameOver()){
            //System.out.println("Game not ending...process pitch results");
            
            
            //DebugLog.log("Worker-" + this.id + " " + atBat.pitchResults);
            String last = atBat.pitchResults.substring(atBat.pitchResults.length() - 1);
            stats[0]++;
            if(last.equals("O")) stats[1]++;
            else if(last.equals("1")) stats[2]++;
            else if(last.equals("2")) stats[3]++;
            else if(last.equals("3")) stats[4]++;
            else if(last.equals("H")) stats[5]++;
            else if(last.equals("W")) stats[6]++;
            else if(last.equals("S")) stats[7]++;
            else if(last.equals("P")) stats[8]++;   
        }

        //game is over, calculate battingAvg, write new results to file
        else {
          //DebugLog.log("signal'd game over");
          int onBase = (stats[2] + stats[3] + stats[4] + stats[5] + stats[6] + stats[8]);

          double divisor = (stats[0] - stats[6] - stats[8]);
          double battingAvg = 0;
          if(divisor > 0) battingAvg = ((stats[2] + stats[3] + stats[4] + stats[5]) / divisor);
          else battingAvg = -1;    

            //write new game stats to file
            try{
              if(battingAvg != -1){
                FileWriter myWriter = new FileWriter(this.id + ".txt", true);
                String[] strArray = Arrays.stream(stats).mapToObj(String::valueOf).toArray(String[]::new);
                //strArray[10] = this.name;
                //strArray[11] = Double.toString(battingAvg);
                myWriter.write("\n");
                for(int j = 0; j < 9; j++){
                  myWriter.write(Integer.toString(stats[j]));
                  if(j != 8) myWriter.write(",");
                }
                myWriter.close();
              }
            }
            catch (IOException e){

            }


            //sum up history and calculate overall batting avg
            double total = 0;
            int i = 0;
            double overallBattingAvg = 0;
            int totalAtBats = 0;
            int totalOnBase = 0;
            int totalAvgOnBase = 0;
            int totalAvgDivisor = 0;
            try{
              File file = new File (this.id + ".txt");//finish processing of players and players history here
              Scanner sc = new Scanner(file);
            
              while (sc.hasNextLine()) {
                  i++;
                  double curAvg;
                  String data = sc.nextLine();
                  String[] vals = data.split(",");

                  totalAtBats += Integer.parseInt(vals[0]);
                  totalOnBase += (Integer.parseInt(vals[2]) + Integer.parseInt(vals[3]) + Integer.parseInt(vals[4]) + Integer.parseInt(vals[5]) + Integer.parseInt(vals[6]) + Integer.parseInt(vals[8]));

                  //add up the batting avg from each individual game
                  /*divisor = Integer.parseInt(vals[0]) - Integer.parseInt(vals[6]) - Integer.parseInt(vals[8]);
                  if(divisor > 0) total += ((Integer.parseInt(vals[2]) + Integer.parseInt(vals[3]) + Integer.parseInt(vals[4]) + Integer.parseInt(vals[5])) / divisor);
                  else total += -1;
                  */
                  totalAvgOnBase += Integer.parseInt(vals[2]) + Integer.parseInt(vals[3]) + Integer.parseInt(vals[4]) + Integer.parseInt(vals[5]);
                  totalAvgDivisor += Integer.parseInt(vals[0]) - Integer.parseInt(vals[6]) - Integer.parseInt(vals[8]);
              }
              overallBattingAvg = (double)totalAvgOnBase/(double)totalAvgDivisor;  //overall batting average = sum up batting avg from each game / total num of games
              //stats[12] = overallBattingAvg;

              StatsObj obj = new StatsObj(this.name, totalAtBats, totalOnBase, overallBattingAvg);
              PlayerToDateStatsReport.printReportData(obj);
            }
            catch(FileNotFoundException s){

            } 

            //now send results back to PlayerStatsTracker as string array
            //int playerID, int statRecordID, int statRecordCount, String PlayerName, int strike_outs, int walks, int singles, int doubles, int triples, int homeRuns, double gameBattingAverage,double overallBattingAverage

            String[] sendBack = {Integer.toString(this.id), this.name, Integer.toString(stats[7]), Integer.toString(stats[6]), Integer.toString(stats[2]), Integer.toString(stats[3]), Integer.toString(stats[4]), Integer.toString(stats[5]), Double.toString(battingAvg), Double.toString(overallBattingAvg)};
            this.outgoingResults.put(String.join(",", sendBack));



            //Must be done concurrently from thread; append, don't rewrite
            
            //send game stats back then exit
            return;
        }                                     
          
      } catch(InterruptedException e){
        System.err.println(e.getMessage());
      }
    }
  }
    
}
