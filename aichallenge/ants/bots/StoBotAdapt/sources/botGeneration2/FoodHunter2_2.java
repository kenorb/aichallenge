/*     */ package botGeneration2;
/*     */ 
/*     */ import backboneImplementation.ObjectivVectorModelImpl;
/*     */ import backboneInterface.BotInterface.ServerInitMessage;
/*     */ import backboneInterface.SeedInterface;
/*     */ import bitmapmanip.BitMap;
/*     */ import bitmapmanip.BitMapTools;
/*     */ import bitmapmanip.BitMapTools.BitIterator;
/*     */ import botTools.BitMapPlanningGen2;
/*     */ import botTools.ConflictSolverPlannerGen2_2;
/*     */ import java.util.List;
/*     */ 
/*     */ public class FoodHunter2_2 extends BaseBotGen2_2
/*     */ {
/*     */   BitMap consideredFood;
/*     */   BitMap desiredFood;
/*     */   BitMap freeAnt;
/*     */   ConflictSolverPlannerGen2_2 conflictSolver;
/*  40 */   boolean oneAntActiv = false;
/*  41 */   boolean emergencyBreak = false;
/*     */ 
/*  43 */   int planNum = 0;
/*     */ 
/*     */   public FoodHunter2_2(SeedInterface rand)
/*     */   {
/*  26 */     super(rand);
/*     */   }
/*     */ 
/*     */   public void init(BotInterface.ServerInitMessage init)
/*     */   {
/*  31 */     super.init(init);
/*     */ 
/*  33 */     this.desiredFood = new BitMap(this.width, this.height);
/*  34 */     this.consideredFood = new BitMap(this.width, this.height);
/*  35 */     this.freeAnt = new BitMap(this.width, this.height);
/*  36 */     this.conflictSolver = new ConflictSolverPlannerGen2_2(this.width, this.height, this.rand);
/*     */   }
/*     */ 
/*     */   public void makeUpOrders()
/*     */   {
/*  47 */     for (int i = 0; i < this.planOrderToSend.currentPlan.length; i++) {
/*  48 */       this.planOrderToSend.currentPlan[i].xor(this.planOrderToSend.currentPlan[i]);
/*     */     }
/*  50 */     this.freeAnt.copy(this.receivedOwnAnt);
/*  51 */     this.desiredFood.copy(this.receivedFood);
/*     */ 
/*  53 */     this.buff01.copy(this.receivedFood);
/*  54 */     this.buff01.rotateDown(1, this.buff02);
/*  55 */     this.buff01.and(this.freeAnt);
/*  56 */     this.buff03.copy(this.buff01);
/*  57 */     this.buff01.copy(this.receivedFood);
/*  58 */     this.buff01.rotateUp(1, this.buff02);
/*  59 */     this.buff01.and(this.freeAnt);
/*  60 */     this.buff03.or(this.buff01);
/*  61 */     this.buff01.copy(this.receivedFood);
/*  62 */     this.buff01.rotateLeft(1);
/*  63 */     this.buff01.and(this.freeAnt);
/*  64 */     this.buff03.or(this.buff01);
/*  65 */     this.buff01.copy(this.receivedFood);
/*  66 */     this.buff01.rotateRight(1);
/*  67 */     this.buff01.and(this.freeAnt);
/*  68 */     this.buff03.or(this.buff01);
/*     */ 
/*  70 */     this.buff03.bitnot();
/*  71 */     this.freeAnt.and(this.buff03);
/*     */ 
/*  74 */     this.planNum = 0;
/*     */ 
/*  76 */     while ((!this.desiredFood.isZero()) && (!this.freeAnt.isZero()) && (!this.emergencyBreak))
/*     */     {
/*  78 */       this.buff01.copy(this.freeAnt);
/*  79 */       this.buff01.nearestUpTo(this.desiredFood, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/*  81 */       this.consideredFood.copy(this.move00);
/*  82 */       this.consideredFood.or(this.move01);
/*  83 */       this.consideredFood.or(this.move02);
/*  84 */       this.consideredFood.or(this.move03);
/*     */ 
/*  86 */       if (this.consideredFood.isZero()) {
/*     */         break;
/*     */       }
/*  89 */       BitMapTools.BitIterator itOnFood = new BitMapTools.BitIterator()
/*     */       {
/*     */         public void at(int px, int py) {
/*  92 */           FoodHunter2_2.this.buff01.xor(FoodHunter2_2.this.buff01);
/*  93 */           FoodHunter2_2.this.buff01.set(px, py, 1L);
/*     */ 
/*  95 */           FoodHunter2_2.this.buff01.nearestUpTo(FoodHunter2_2.this.freeAnt, FoodHunter2_2.this.move00, FoodHunter2_2.this.move01, FoodHunter2_2.this.move02, FoodHunter2_2.this.move03, FoodHunter2_2.this.refKnownGround, FoodHunter2_2.this.buff02, FoodHunter2_2.this.buff03, FoodHunter2_2.this.buff04);
/*     */ 
/*  99 */           FoodHunter2_2.this.buff02.xor(FoodHunter2_2.this.buff02);
/* 100 */           FoodHunter2_2.this.buff02.or(FoodHunter2_2.this.move00);
/* 101 */           FoodHunter2_2.this.buff02.or(FoodHunter2_2.this.move01);
/* 102 */           FoodHunter2_2.this.buff02.or(FoodHunter2_2.this.move02);
/* 103 */           FoodHunter2_2.this.buff02.or(FoodHunter2_2.this.move03);
/*     */ 
/* 105 */           FoodHunter2_2.this.oneAntActiv = false;
/*     */ 
/* 107 */           BitMapTools.BitIterator itOnAntTouched = new BitMapTools.BitIterator(px, py)
/*     */           {
/*     */             public void at(int x, int y) {
/* 110 */               if (FoodHunter2_2.this.oneAntActiv) return;
/* 111 */               FoodHunter2_2.this.buff03.xor(FoodHunter2_2.this.buff03);
/* 112 */               FoodHunter2_2.this.buff03.set(x, y, 1L);
/* 113 */               FoodHunter2_2.this.move00.and(FoodHunter2_2.this.buff03);
/* 114 */               FoodHunter2_2.this.move01.and(FoodHunter2_2.this.buff03);
/* 115 */               FoodHunter2_2.this.move02.and(FoodHunter2_2.this.buff03);
/* 116 */               FoodHunter2_2.this.move03.and(FoodHunter2_2.this.buff03);
/*     */ 
/* 118 */               FoodHunter2_2.this.desiredFood.set(this.val$px, this.val$py, 0L);
/* 119 */               FoodHunter2_2.this.freeAnt.set(x, y, 0L);
/* 120 */               FoodHunter2_2.this.oneAntActiv = true;
/*     */ 
/* 122 */               FoodHunter2_2.this.objectiv.osx.add(Integer.valueOf(x));
/* 123 */               FoodHunter2_2.this.objectiv.osy.add(Integer.valueOf(y));
/* 124 */               FoodHunter2_2.this.objectiv.odx.add(Integer.valueOf(this.val$px));
/* 125 */               FoodHunter2_2.this.objectiv.ody.add(Integer.valueOf(this.val$py));
/* 126 */               FoodHunter2_2.this.objectiv.ocol.add(Integer.valueOf(FoodHunter2_2.this.planNum++));
/*     */             }
/*     */           };
/* 133 */           BitMapTools.bitIterate(FoodHunter2_2.this.buff02, itOnAntTouched);
/* 134 */           FoodHunter2_2.this.planOrderToSend.currentPlan[0].or(FoodHunter2_2.this.move02);
/* 135 */           FoodHunter2_2.this.planOrderToSend.currentPlan[1].or(FoodHunter2_2.this.move03);
/* 136 */           FoodHunter2_2.this.planOrderToSend.currentPlan[2].or(FoodHunter2_2.this.move00);
/* 137 */           FoodHunter2_2.this.planOrderToSend.currentPlan[3].or(FoodHunter2_2.this.move01);
/*     */         }
/*     */       };
/* 140 */       BitMapTools.bitIterate(this.consideredFood, itOnFood);
/*     */     }
/*     */ 
/* 145 */     this.buff03.xor(this.buff03);
/* 146 */     this.buff01.copy(this.refKnownGround);
/* 147 */     this.buff01.or(this.refKnownWater);
/* 148 */     this.buff01.bitnot();
/* 149 */     this.buff01.scramble(this.buff02, this.buff03, this.buff04, 1);
/* 150 */     this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 152 */     this.buff02.xor(this.buff02);
/* 153 */     this.buff02.or(this.move00);
/* 154 */     this.buff02.or(this.move01);
/* 155 */     this.buff02.or(this.move02);
/* 156 */     this.buff02.or(this.move03);
/* 157 */     this.freeAnt.xor(this.buff02);
/* 158 */     this.planOrderToSend.currentPlan[0].or(this.move02);
/* 159 */     this.planOrderToSend.currentPlan[1].or(this.move03);
/* 160 */     this.planOrderToSend.currentPlan[2].or(this.move00);
/* 161 */     this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */ 
/* 165 */     this.buff01.copy(this.stateUnrefreshedFriendly);
/* 166 */     this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 168 */     this.buff02.xor(this.buff02);
/* 169 */     this.buff02.or(this.move00);
/* 170 */     this.buff02.or(this.move01);
/* 171 */     this.buff02.or(this.move02);
/* 172 */     this.buff02.or(this.move03);
/* 173 */     this.freeAnt.xor(this.buff02);
/* 174 */     this.planOrderToSend.currentPlan[0].or(this.move02);
/* 175 */     this.planOrderToSend.currentPlan[1].or(this.move03);
/* 176 */     this.planOrderToSend.currentPlan[2].or(this.move00);
/* 177 */     this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */ 
/* 180 */     int secure = this.freeAnt.x + this.freeAnt.y << 1;
/* 181 */     while ((!this.freeAnt.isZero()) && (secure > 0)) {
/* 182 */       secure--;
/*     */ 
/* 184 */       this.buff01.copy(this.refCalcPotentialEnnemyGround);
/* 185 */       this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 187 */       this.buff02.xor(this.buff02);
/* 188 */       this.buff02.or(this.move00);
/* 189 */       this.buff02.or(this.move01);
/* 190 */       this.buff02.or(this.move02);
/* 191 */       this.buff02.or(this.move03);
/* 192 */       this.freeAnt.xor(this.buff02);
/* 193 */       this.planOrderToSend.currentPlan[0].or(this.move02);
/* 194 */       this.planOrderToSend.currentPlan[1].or(this.move03);
/* 195 */       this.planOrderToSend.currentPlan[2].or(this.move00);
/* 196 */       this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */     }
/*     */ 
/* 202 */     this.conflictSolver.solveConflictSimple(this.planOrderToSend, this.receivedOwnAnt);
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botGeneration2.FoodHunter2_2
 * JD-Core Version:    0.6.0
 */