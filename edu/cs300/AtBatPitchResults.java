package edu.cs300;

public class AtBatPitchResults {

    int playerID;
    String pitchResults;


  public AtBatPitchResults(int playerID, String pitchResults){
    this.playerID=playerID;
    this.pitchResults=pitchResults;
  }
    
  public AtBatPitchResults(){
    this.playerID=-1;
    this.pitchResults="";
  }
  public boolean gameOver(){
      if (this.playerID==-1) return true;
      return false;
  }
  public String toString(){
    return this.playerID+":"+this.pitchResults;
  }

}