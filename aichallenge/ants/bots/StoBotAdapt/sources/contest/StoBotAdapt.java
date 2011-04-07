/*     */ package contest;
/*     */ 
/*     */ import backboneInterface.BotInterface;
/*     */ import backboneInterface.BotInterface.ServerInitMessage;
/*     */ import backboneInterface.BotInterface.ServerTurnMessage;
/*     */ import java.util.ArrayList;
/*     */ import java.util.List;
/*     */ 
/*     */ public class StoBotAdapt
/*     */   implements Bot
/*     */ {
/*     */   final BotInterface stobot;
/*  21 */   int t = -1;
/*     */ 
/*     */   public StoBotAdapt(BotInterface stobot) {
/*  24 */     this.stobot = stobot;
/*     */   }
/*     */ 
/*     */   public void do_turn(Ants ants)
/*     */   {
/*  30 */     this.t += 1;
/*  31 */     if (this.t == 0) {
/*  32 */       BotInterface.ServerInitMessage mess = new BotInterface.ServerInitMessage(ants) {
/*     */         public int loadtime() {
/*  34 */           return this.val$ants.loadtime();
/*     */         }
/*     */ 
/*     */         public int turntime() {
/*  38 */           return this.val$ants.turntime();
/*     */         }
/*     */ 
/*     */         public int rows() {
/*  42 */           return this.val$ants.rows();
/*     */         }
/*     */ 
/*     */         public int cols() {
/*  46 */           return this.val$ants.cols();
/*     */         }
/*     */ 
/*     */         public int turns() {
/*  50 */           return this.val$ants.turns();
/*     */         }
/*     */ 
/*     */         public int viewradius2() {
/*  54 */           return this.val$ants.viewradius2();
/*     */         }
/*     */ 
/*     */         public int attackradius2() {
/*  58 */           return attackradius2();
/*     */         }
/*     */ 
/*     */         public int spawnradius2() {
/*  62 */           return spawnradius2();
/*     */         }
/*     */       };
/*  66 */       this.stobot.init(mess);
/*     */     }
/*     */ 
/*  69 */     List food = new ArrayList();
/*  70 */     food.addAll(ants.food());
/*     */ 
/*  72 */     BotInterface.ServerTurnMessage turn = new BotInterface.ServerTurnMessage(food, ants)
/*     */     {
/*     */       public int getNbViewedFood() {
/*  75 */         return this.val$food.size();
/*     */       }
/*     */ 
/*     */       public int getXViewedFoodPos(int i) {
/*  79 */         return ((Tile)this.val$food.get(i)).col();
/*     */       }
/*     */ 
/*     */       public int getYViewedFoodPos(int i) {
/*  83 */         return ((Tile)this.val$food.get(i)).row();
/*     */       }
/*     */ 
/*     */       public int getNbFirstViewedWater() {
/*  87 */         return this.val$ants.waterList.size();
/*     */       }
/*     */ 
/*     */       public int getXFirstViewedWater(int i) {
/*  91 */         return ((Tile)this.val$ants.waterList.get(i)).col();
/*     */       }
/*     */ 
/*     */       public int getYFirstViewedWater(int i) {
/*  95 */         return ((Tile)this.val$ants.waterList.get(i)).row();
/*     */       }
/*     */ 
/*     */       public int getNbYourAnt() {
/*  99 */         return this.val$ants.myAnt.size();
/*     */       }
/*     */ 
/*     */       public int getXYourAntPos(int i) {
/* 103 */         return ((Tile)this.val$ants.myAnt.get(i)).col();
/*     */       }
/*     */ 
/*     */       public int getYYourAntPos(int i) {
/* 107 */         return ((Tile)this.val$ants.myAnt.get(i)).row();
/*     */       }
/*     */ 
/*     */       public int getNbOtherAnt() {
/* 111 */         return this.val$ants.ennemyAnt.size();
/*     */       }
/*     */ 
/*     */       public int getXViewedAnt(int i) {
/* 115 */         return ((Tile)this.val$ants.ennemyAnt.get(i)).col();
/*     */       }
/*     */ 
/*     */       public int getYViewedAnt(int i) {
/* 119 */         return ((Tile)this.val$ants.ennemyAnt.get(i)).row();
/*     */       }
/*     */ 
/*     */       public int getOwnerViewedAnt(int i) {
/* 123 */         return 1;
/*     */       }
/*     */ 
/*     */       public boolean gameEnd() {
/* 127 */         return false;
/*     */       }
/*     */ 
/*     */       public int turnNumberIfNotEnd() {
/* 131 */         return 0;
/*     */       }
/*     */ 
/*     */       public int[] scoreForEachPlayerIfEnd() {
/* 135 */         return null;
/*     */       }
/*     */ 
/*     */       public int nbPlayerIfEnd() {
/* 139 */         return 2;
/*     */       }
/*     */     };
/* 142 */     this.stobot.turnStart(turn);
/*     */ 
/* 144 */     for (int i = 0; i < this.stobot.nbAntWithOrderForNextTurn(); i++)
/*     */     {
/* 146 */       int dir = this.stobot.getDirBotOrderForNextTurn(i);
/* 147 */       int x = this.stobot.getXBotOrderForNextTurn(i);
/* 148 */       int y = this.stobot.getYBotOrderForNextTurn(i);
/*     */ 
/* 150 */       ants.issueOrder(x, y, Aim.values()[dir]);
/*     */     }
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.StoBotAdapt
 * JD-Core Version:    0.6.0
 */