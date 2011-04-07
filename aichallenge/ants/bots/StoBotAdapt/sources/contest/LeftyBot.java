/*    */ package contest;
/*    */ 
/*    */ import java.util.ArrayList;
/*    */ import java.util.HashMap;
/*    */ import java.util.HashSet;
/*    */ import java.util.Iterator;
/*    */ import java.util.List;
/*    */ import java.util.Map;
/*    */ import java.util.Set;
/*    */ 
/*    */ public class LeftyBot
/*    */   implements Bot
/*    */ {
/* 11 */   private Map<Tile, Aim> antStraight = new HashMap();
/* 12 */   private Map<Tile, Aim> antLefty = new HashMap();
/*    */ 
/*    */   public static void main(String[] args)
/*    */   {
/*  7 */     Ants.run(new LeftyBot());
/*    */   }
/*    */ 
/*    */   public void do_turn(Ants ants)
/*    */   {
/* 16 */     Set destinations = new HashSet();
/* 17 */     Map newStraight = new HashMap();
/* 18 */     Map newLefty = new HashMap();
/* 19 */     for (Iterator i$ = ants.myAnts().iterator(); i$.hasNext(); ) { location = (Tile)i$.next();
/*    */ 
/* 21 */       if ((!this.antStraight.containsKey(location)) && (!this.antLefty.containsKey(location)))
/*    */       {
/*    */         Aim direction;
/*    */         Aim direction;
/* 23 */         if (location.row() % 2 == 0)
/*    */         {
/*    */           Aim direction;
/* 24 */           if (location.col() % 2 == 0)
/* 25 */             direction = Aim.NORTH;
/*    */           else
/* 27 */             direction = Aim.SOUTH;
/*    */         }
/*    */         else
/*    */         {
/*    */           Aim direction;
/* 30 */           if (location.col() % 2 == 0)
/* 31 */             direction = Aim.EAST;
/*    */           else {
/* 33 */             direction = Aim.WEST;
/*    */           }
/*    */         }
/* 36 */         this.antStraight.put(location, direction);
/*    */       }
/*    */ 
/* 39 */       if (this.antStraight.containsKey(location)) {
/* 40 */         Aim direction = (Aim)this.antStraight.get(location);
/* 41 */         Tile destination = ants.tile(location, direction);
/* 42 */         if (ants.ilk(destination).isPassable()) {
/* 43 */           if ((ants.ilk(destination).isUnoccupied()) && (!destinations.contains(destination))) {
/* 44 */             ants.issueOrder(location, direction);
/* 45 */             newStraight.put(destination, direction);
/* 46 */             destinations.add(destination);
/*    */           }
/*    */           else {
/* 49 */             newStraight.put(location, direction.left());
/* 50 */             destinations.add(location);
/*    */           }
/*    */         }
/*    */         else {
/* 54 */           this.antLefty.put(location, direction.right());
/*    */         }
/*    */       }
/*    */ 
/* 58 */       if (this.antLefty.containsKey(location)) {
/* 59 */         direction = (Aim)this.antLefty.get(location);
/* 60 */         List directions = new ArrayList();
/* 61 */         directions.add(direction.left());
/* 62 */         directions.add(direction);
/* 63 */         directions.add(direction.right());
/* 64 */         directions.add(direction.behind());
/*    */ 
/* 66 */         for (Aim new_direction : directions) {
/* 67 */           Tile destination = ants.tile(location, new_direction);
/* 68 */           if (ants.ilk(destination).isPassable()) {
/* 69 */             if ((ants.ilk(destination).isUnoccupied()) && (!destinations.contains(destination))) {
/* 70 */               ants.issueOrder(location, new_direction);
/* 71 */               newLefty.put(destination, new_direction);
/* 72 */               destinations.add(destination);
/* 73 */               break;
/*    */             }
/*    */ 
/* 76 */             newStraight.put(location, direction.right());
/* 77 */             destinations.add(location);
/* 78 */             break;
/*    */           }
/*    */         }
/*    */       }
/*    */     }
/*    */     Tile location;
/*    */     Aim direction;
/* 84 */     this.antStraight = newStraight;
/* 85 */     this.antLefty = newLefty;
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.LeftyBot
 * JD-Core Version:    0.6.0
 */