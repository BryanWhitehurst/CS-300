package edu.cs300;

 

public class PlayerToDateStatsReport{

    

    static boolean headerPrinted=false;

 

    synchronized static void printReportData(StatsObj obj){              
            if (!headerPrinted) {
                System.out.printf("%-31s%-10s%-9s%-11s\n", "Name", "At Bats", "On base", "Batting Avg");
                headerPrinted=true;
            }

            
            System.out.printf("%-31s%-10d%-9d%-11.3f\n", obj.name, obj.atBats, obj.onBase, obj.battingAvg);
            

    }
}

