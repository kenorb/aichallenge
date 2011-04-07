/*    */ package contest;
/*    */ 
/*    */ import java.util.ArrayList;
/*    */ import java.util.Collections;
/*    */ import java.util.EnumSet;
/*    */ import java.util.HashSet;
/*    */ import java.util.List;
/*    */ import java.util.Set;
/*    */ 
/*    */ public class RandomBot
/*    */   implements Bot
/*    */ {
/*    */   public static void main(String[] args)
/*    */   {
/*  7 */     Ants.run(new RandomBot());
/*    */   }
/*    */ 
/*    */   public void do_turn(Ants ants) {
/* 11 */     Set destinations = new HashSet();
/* 12 */     for (Tile location : ants.myAnts()) {
/* 13 */       List directions = new ArrayList(EnumSet.allOf(Aim.class));
/* 14 */       Collections.shuffle(directions);
/* 15 */       boolean issued = false;
/* 16 */       for (Aim direction : directions) {
/* 17 */         Tile destination = ants.tile(location, direction);
/* 18 */         if ((ants.ilk(destination).isUnoccupied()) && (!destinations.contains(destination))) {
/* 19 */           ants.issueOrder(location, direction);
/* 20 */           destinations.add(destination);
/* 21 */           issued = true;
/* 22 */           break;
/*    */         }
/*    */       }
/* 25 */       if (!issued)
/* 26 */         destinations.add(location);
/*    */     }
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.RandomBot
 * JD-Core Version:    0.6.0
 */