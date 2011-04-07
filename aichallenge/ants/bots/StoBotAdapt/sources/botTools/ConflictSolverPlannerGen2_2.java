/*    */ package botTools;
/*    */ 
/*    */ import backboneInterface.SeedInterface;
/*    */ import bitmapmanip.BitMap;
/*    */ 
/*    */ public class ConflictSolverPlannerGen2_2
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
/*    */   public ConflictSolverPlannerGen2_2(int width, int height, SeedInterface rand)
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
/* 56 */     while (!this.expectedDeath.isZero()) {
/* 57 */       this.potentialFuturPos.copy(plan.currentPlan[4]);
/* 58 */       this.expectedDeath.xor(this.expectedDeath);
/* 59 */       for (int dir = 0; dir < 4; dir++) {
/* 60 */         this.potentialNextOnDir.copy(plan.currentPlan[dir]);
/* 61 */         switch (dir) {
/*    */         case 0:
/* 63 */           this.potentialNextOnDir.rotateUp(1, this.buff01); break;
/*    */         case 1:
/* 65 */           this.potentialNextOnDir.rotateRight(1); break;
/*    */         case 2:
/* 67 */           this.potentialNextOnDir.rotateDown(1, this.buff01); break;
/*    */         case 3:
/* 69 */           this.potentialNextOnDir.rotateLeft(1); break;
/*    */         }
/*    */ 
/* 72 */         this.buff01.copy(this.potentialFuturPos);
/* 73 */         this.buff01.and(this.potentialNextOnDir);
/* 74 */         this.expectedDeath.or(this.buff01);
/* 75 */         this.potentialFuturPos.or(this.potentialNextOnDir);
/*    */ 
/* 77 */         this.buff01.bitnot();
/*    */ 
/* 79 */         switch (dir) {
/*    */         case 2:
/* 81 */           this.buff01.rotateUp(1, this.buff02); break;
/*    */         case 3:
/* 83 */           this.buff01.rotateRight(1); break;
/*    */         case 0:
/* 85 */           this.buff01.rotateDown(1, this.buff02); break;
/*    */         case 1:
/* 87 */           this.buff01.rotateLeft(1); break;
/*    */         }
/*    */ 
/* 90 */         plan.currentPlan[dir].and(this.buff01);
/*    */       }
/*    */ 
/* 95 */       this.currStatique.copy(plan.currentPlan[0]);
/* 96 */       this.currStatique.or(plan.currentPlan[1]);
/* 97 */       this.currStatique.or(plan.currentPlan[2]);
/* 98 */       this.currStatique.or(plan.currentPlan[3]);
/* 99 */       this.currStatique.bitnot();
/* 100 */       this.currStatique.and(antpos);
/* 101 */       plan.currentPlan[4].copy(this.currStatique);
/*    */     }
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botTools.ConflictSolverPlannerGen2_2
 * JD-Core Version:    0.6.0
 */