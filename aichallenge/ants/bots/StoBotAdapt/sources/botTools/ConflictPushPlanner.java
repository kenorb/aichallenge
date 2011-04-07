/*    */ package botTools;
/*    */ 
/*    */ import backboneInterface.SeedInterface;
/*    */ import bitmapmanip.BitMap;
/*    */ 
/*    */ public class ConflictPushPlanner
/*    */ {
/*    */   final BitMap potentialFuturPos;
/*    */   final BitMap potentialNextOnDir;
/*    */   final BitMap buff01;
/*    */   final BitMap buff02;
/*    */   final BitMap currStatique;
/*    */   final BitMap expectedDeath;
/* 25 */   final int[] dirStack = { 0, 1, 2, 3 };
/* 26 */   int dirSp = 0;
/*    */   final SeedInterface rand;
/*    */ 
/*    */   public ConflictPushPlanner(int width, int height, SeedInterface rand)
/*    */   {
/* 30 */     this.potentialFuturPos = new BitMap(width, height);
/* 31 */     this.buff01 = new BitMap(width, height);
/* 32 */     this.buff02 = new BitMap(width, height);
/*    */ 
/* 34 */     this.potentialNextOnDir = new BitMap(width, height);
/* 35 */     this.expectedDeath = new BitMap(width, height);
/* 36 */     this.currStatique = new BitMap(width, height);
/*    */ 
/* 38 */     this.rand = rand;
/*    */   }
/*    */ 
/*    */   public void solveConflictSimple(BitMapPlanningGen2 plan, BitMap antpos)
/*    */   {
/* 45 */     this.expectedDeath.set(0, 0, 1L);
/*    */ 
/* 48 */     this.buff01.copy(plan.currentPlan[0]);
/* 49 */     this.buff01.or(plan.currentPlan[1]);
/* 50 */     this.buff01.or(plan.currentPlan[2]);
/* 51 */     this.buff01.or(plan.currentPlan[3]);
/* 52 */     this.buff01.bitnot();
/* 53 */     this.buff01.and(antpos);
/* 54 */     plan.currentPlan[4].copy(this.buff01);
/*    */ 
/* 56 */     int sec = 20;
/* 57 */     while ((!this.expectedDeath.isZero()) && (sec > 0)) {
/* 58 */       sec--;
/*    */ 
/* 60 */       this.potentialFuturPos.copy(plan.currentPlan[4]);
/* 61 */       this.expectedDeath.xor(this.expectedDeath);
/* 62 */       for (int dir = 0; dir < 4; dir++) {
/* 63 */         this.potentialNextOnDir.copy(plan.currentPlan[dir]);
/* 64 */         switch (dir) {
/*    */         case 0:
/* 66 */           this.potentialNextOnDir.rotateUp(1, this.buff01); break;
/*    */         case 1:
/* 68 */           this.potentialNextOnDir.rotateRight(1); break;
/*    */         case 2:
/* 70 */           this.potentialNextOnDir.rotateDown(1, this.buff01); break;
/*    */         case 3:
/* 72 */           this.potentialNextOnDir.rotateLeft(1); break;
/*    */         }
/*    */ 
/* 76 */         this.buff01.copy(plan.currentPlan[4]);
/* 77 */         this.buff01.and(this.potentialNextOnDir);
/* 78 */         if (!this.buff01.isZero()) {
/* 79 */           plan.currentPlan[4].xor(this.buff01);
/* 80 */           this.potentialNextOnDir.xor(this.buff01);
/* 81 */           plan.currentPlan[dir].or(this.buff01);
/*    */         }
/*    */ 
/* 84 */         this.buff01.copy(this.potentialFuturPos);
/* 85 */         this.buff01.and(this.potentialNextOnDir);
/*    */ 
/* 87 */         this.expectedDeath.or(this.buff01);
/* 88 */         this.potentialFuturPos.or(this.potentialNextOnDir);
/*    */ 
/* 90 */         this.buff01.bitnot();
/*    */ 
/* 96 */         switch (dir) {
/*    */         case 2:
/* 98 */           this.buff01.rotateUp(1, this.buff02); break;
/*    */         case 3:
/* 100 */           this.buff01.rotateRight(1); break;
/*    */         case 0:
/* 102 */           this.buff01.rotateDown(1, this.buff02); break;
/*    */         case 1:
/* 104 */           this.buff01.rotateLeft(1); break;
/*    */         }
/*    */ 
/* 107 */         plan.currentPlan[dir].and(this.buff01);
/*    */       }
/*    */ 
/* 112 */       this.currStatique.copy(plan.currentPlan[0]);
/* 113 */       this.currStatique.or(plan.currentPlan[1]);
/* 114 */       this.currStatique.or(plan.currentPlan[2]);
/* 115 */       this.currStatique.or(plan.currentPlan[3]);
/* 116 */       this.currStatique.bitnot();
/* 117 */       this.currStatique.and(antpos);
/* 118 */       plan.currentPlan[4].copy(this.currStatique);
/*    */     }
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botTools.ConflictPushPlanner
 * JD-Core Version:    0.6.0
 */