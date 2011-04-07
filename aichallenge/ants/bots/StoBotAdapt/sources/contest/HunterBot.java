/*    */ package contest;
/*    */ 
/*    */ import java.util.Collections;
/*    */ import java.util.HashSet;
/*    */ import java.util.List;
/*    */ import java.util.Set;
/*    */ 
/*    */ public class HunterBot
/*    */   implements Bot
/*    */ {
/*    */   public static void main(String[] args)
/*    */   {
/*  7 */     Ants.run(new HunterBot());
/*    */   }
/*    */ 
/*    */   public void do_turn(Ants ants) {
/* 11 */     Set destinations = new HashSet();
/* 12 */     Set targets = new HashSet();
/* 13 */     targets.addAll(ants.food());
/* 14 */     targets.addAll(ants.enemyAnts());
/* 15 */     for (Tile location : ants.myAnts()) {
/* 16 */       boolean issued = false;
/* 17 */       Tile closestTarget = null;
/* 18 */       int closestDistance = 999999;
/* 19 */       for (Tile target : targets) {
/* 20 */         int distance = ants.distance(location, target);
/* 21 */         if (distance < closestDistance) {
/* 22 */           closestDistance = distance;
/* 23 */           closestTarget = target;
/*    */         }
/*    */       }
/* 26 */       if (closestTarget != null) {
/* 27 */         List directions = ants.directions(location, closestTarget);
/* 28 */         Collections.shuffle(directions);
/* 29 */         for (Aim direction : directions) {
/* 30 */           Tile destination = ants.tile(location, direction);
/* 31 */           if ((ants.ilk(destination).isUnoccupied()) && (!destinations.contains(destination))) {
/* 32 */             ants.issueOrder(location, direction);
/* 33 */             destinations.add(destination);
/* 34 */             issued = true;
/* 35 */             break;
/*    */           }
/*    */         }
/*    */       }
/* 39 */       if (!issued)
/* 40 */         destinations.add(location);
/*    */     }
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.HunterBot
 * JD-Core Version:    0.6.0
 */