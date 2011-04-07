/*    */ package main;
/*    */ 
/*    */ import backboneImplementation.MersenneSeed;
/*    */ import botGeneration2.FoodHunter2_2;
/*    */ import contest.Ants;
/*    */ import contest.StoBotAdapt;
/*    */ 
/*    */ public class Main
/*    */ {
/*    */   public static void main(String[] args)
/*    */   {
/* 25 */     FoodHunter2_2 tbot = new FoodHunter2_2(new MersenneSeed(897766L));
/* 26 */     Ants.run(new StoBotAdapt(tbot));
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     main.Main
 * JD-Core Version:    0.6.0
 */