/*     */ package botGeneration2;
/*     */ 
/*     */ import backboneInterface.BotInterface.ServerInitMessage;
/*     */ import backboneInterface.GridViewerModel;
/*     */ import backboneInterface.SeedInterface;
/*     */ import bitmapmanip.BitMap;
/*     */ import bitmapmanip.BitMapTools;
/*     */ import bitmapmanip.BitMapTools.BitIterator;
/*     */ import botTools.BitMapPlanningGen2;
/*     */ import botTools.ConflictSolverPlannerGen2_2;
/*     */ import botTools.GateDetector;
/*     */ 
/*     */ public class HillBotGen2 extends BaseBotGen2_2
/*     */ {
/*     */   BitMap consideredFood;
/*     */   BitMap desiredFood;
/*     */   BitMap freeAnt;
/*     */   BitMap gateZone;
/*     */   BitMap mingateZone;
/*     */   int minNumberZone;
/*     */   BitMap consideredGates;
/*     */   BitMap desiredGates;
/*     */   ConflictSolverPlannerGen2_2 conflictSolver;
/*     */   GateDetector gateDetect;
/*  54 */   boolean oneAntActiv = false;
/*  55 */   boolean emergencyBreak = false;
/*     */ 
/*  57 */   int planNum = 0;
/*     */ 
/* 227 */   public final GridViewerModel viewGateZone = new GridViewerModel() {
/*     */     public int width() {
/* 229 */       return HillBotGen2.this.receivedFood.x;
/*     */     }
/*     */     public int height() {
/* 232 */       return HillBotGen2.this.receivedFood.y;
/*     */     }
/*     */     public int at(int x, int y) {
/* 235 */       if (HillBotGen2.this.gateZone.get(x, y) == 1L) return 6;
/* 236 */       if (HillBotGen2.this.refKnownWater.get(x, y) == 1L) return 2;
/* 237 */       if (HillBotGen2.this.receivedFood.get(x, y) == 1L) return 4;
/* 238 */       if (HillBotGen2.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 239 */       if ((HillBotGen2.this.refKnownGround.get(x, y) == 1L) && (HillBotGen2.this.calcView.get(x, y) == 1L)) return 3;
/* 240 */       if (HillBotGen2.this.refKnownGround.get(x, y) == 1L) return 1;
/* 241 */       return 0;
/*     */     }
/* 227 */   };
/*     */ 
/* 245 */   public final GridViewerModel viewMinGate = new GridViewerModel() {
/*     */     public int width() {
/* 247 */       return HillBotGen2.this.receivedFood.x;
/*     */     }
/*     */     public int height() {
/* 250 */       return HillBotGen2.this.receivedFood.y;
/*     */     }
/*     */     public int at(int x, int y) {
/* 253 */       if (HillBotGen2.this.mingateZone.get(x, y) == 1L) return 6;
/* 254 */       if (HillBotGen2.this.refKnownWater.get(x, y) == 1L) return 2;
/* 255 */       if (HillBotGen2.this.receivedFood.get(x, y) == 1L) return 4;
/* 256 */       if (HillBotGen2.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 257 */       if ((HillBotGen2.this.refKnownGround.get(x, y) == 1L) && (HillBotGen2.this.calcView.get(x, y) == 1L)) return 3;
/* 258 */       if (HillBotGen2.this.refKnownGround.get(x, y) == 1L) return 1;
/* 259 */       return 0;
/*     */     }
/* 245 */   };
/*     */ 
/*     */   public HillBotGen2(SeedInterface rand)
/*     */   {
/*  34 */     super(rand);
/*     */   }
/*     */ 
/*     */   public void init(BotInterface.ServerInitMessage init)
/*     */   {
/*  39 */     super.init(init);
/*     */ 
/*  41 */     this.desiredFood = new BitMap(this.width, this.height);
/*  42 */     this.consideredFood = new BitMap(this.width, this.height);
/*  43 */     this.freeAnt = new BitMap(this.width, this.height);
/*  44 */     this.gateZone = new BitMap(this.width, this.height);
/*  45 */     this.consideredGates = new BitMap(this.width, this.height);
/*  46 */     this.desiredGates = new BitMap(this.width, this.height);
/*  47 */     this.mingateZone = new BitMap(this.width, this.height);
/*  48 */     this.conflictSolver = new ConflictSolverPlannerGen2_2(this.width, this.height, this.rand);
/*  49 */     this.gateDetect = new GateDetector(this.width, this.height);
/*  50 */     this.minNumberZone = 10000;
/*     */   }
/*     */ 
/*     */   public void makeUpInit()
/*     */   {
/*  60 */     for (int i = 0; i < this.planOrderToSend.currentPlan.length; i++) {
/*  61 */       this.planOrderToSend.currentPlan[i].xor(this.planOrderToSend.currentPlan[i]);
/*     */     }
/*     */ 
/*  64 */     this.freeAnt.copy(this.receivedOwnAnt);
/*  65 */     this.desiredFood.copy(this.receivedFood);
/*     */ 
/*  67 */     this.buff01.copy(this.receivedFood);
/*  68 */     this.buff01.rotateDown(1, this.buff02);
/*  69 */     this.buff01.and(this.freeAnt);
/*  70 */     this.buff03.copy(this.buff01);
/*  71 */     this.buff01.copy(this.receivedFood);
/*  72 */     this.buff01.rotateUp(1, this.buff02);
/*  73 */     this.buff01.and(this.freeAnt);
/*  74 */     this.buff03.or(this.buff01);
/*  75 */     this.buff01.copy(this.receivedFood);
/*  76 */     this.buff01.rotateLeft(1);
/*  77 */     this.buff01.and(this.freeAnt);
/*  78 */     this.buff03.or(this.buff01);
/*  79 */     this.buff01.copy(this.receivedFood);
/*  80 */     this.buff01.rotateRight(1);
/*  81 */     this.buff01.and(this.freeAnt);
/*  82 */     this.buff03.or(this.buff01);
/*     */ 
/*  84 */     this.buff03.bitnot();
/*  85 */     this.freeAnt.and(this.buff03);
/*     */ 
/*  88 */     this.planNum = 0;
/*     */   }
/*     */ 
/*     */   public void makeUpFooder() {
/*  92 */     while ((!this.desiredFood.isZero()) && (!this.freeAnt.isZero()) && (!this.emergencyBreak))
/*     */     {
/*  94 */       this.buff01.copy(this.freeAnt);
/*  95 */       this.buff01.nearestUpTo(this.desiredFood, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/*  97 */       this.consideredFood.copy(this.move00);
/*  98 */       this.consideredFood.or(this.move01);
/*  99 */       this.consideredFood.or(this.move02);
/* 100 */       this.consideredFood.or(this.move03);
/*     */ 
/* 102 */       if (this.consideredFood.isZero()) {
/*     */         break;
/*     */       }
/* 105 */       BitMapTools.BitIterator itOnFood = new BitMapTools.BitIterator()
/*     */       {
/*     */         public void at(int px, int py) {
/* 108 */           HillBotGen2.this.buff01.xor(HillBotGen2.this.buff01);
/* 109 */           HillBotGen2.this.buff01.set(px, py, 1L);
/*     */ 
/* 111 */           HillBotGen2.this.buff01.nearestUpTo(HillBotGen2.this.freeAnt, HillBotGen2.this.move00, HillBotGen2.this.move01, HillBotGen2.this.move02, HillBotGen2.this.move03, HillBotGen2.this.refKnownGround, HillBotGen2.this.buff02, HillBotGen2.this.buff03, HillBotGen2.this.buff04);
/*     */ 
/* 115 */           HillBotGen2.this.buff02.xor(HillBotGen2.this.buff02);
/* 116 */           HillBotGen2.this.buff02.or(HillBotGen2.this.move00);
/* 117 */           HillBotGen2.this.buff02.or(HillBotGen2.this.move01);
/* 118 */           HillBotGen2.this.buff02.or(HillBotGen2.this.move02);
/* 119 */           HillBotGen2.this.buff02.or(HillBotGen2.this.move03);
/*     */ 
/* 121 */           HillBotGen2.this.oneAntActiv = false;
/*     */ 
/* 123 */           BitMapTools.BitIterator itOnAntTouched = new BitMapTools.BitIterator(px, py)
/*     */           {
/*     */             public void at(int x, int y) {
/* 126 */               if (HillBotGen2.this.oneAntActiv) return;
/* 127 */               HillBotGen2.this.buff03.xor(HillBotGen2.this.buff03);
/* 128 */               HillBotGen2.this.buff03.set(x, y, 1L);
/* 129 */               HillBotGen2.this.move00.and(HillBotGen2.this.buff03);
/* 130 */               HillBotGen2.this.move01.and(HillBotGen2.this.buff03);
/* 131 */               HillBotGen2.this.move02.and(HillBotGen2.this.buff03);
/* 132 */               HillBotGen2.this.move03.and(HillBotGen2.this.buff03);
/*     */ 
/* 134 */               HillBotGen2.this.desiredFood.set(this.val$px, this.val$py, 0L);
/* 135 */               HillBotGen2.this.freeAnt.set(x, y, 0L);
/* 136 */               HillBotGen2.this.oneAntActiv = true;
/*     */             }
/*     */           };
/* 149 */           BitMapTools.bitIterate(HillBotGen2.this.buff02, itOnAntTouched);
/* 150 */           HillBotGen2.this.planOrderToSend.currentPlan[0].or(HillBotGen2.this.move02);
/* 151 */           HillBotGen2.this.planOrderToSend.currentPlan[1].or(HillBotGen2.this.move03);
/* 152 */           HillBotGen2.this.planOrderToSend.currentPlan[2].or(HillBotGen2.this.move00);
/* 153 */           HillBotGen2.this.planOrderToSend.currentPlan[3].or(HillBotGen2.this.move01);
/*     */         }
/*     */       };
/* 156 */       BitMapTools.bitIterate(this.consideredFood, itOnFood);
/*     */     }
/*     */   }
/*     */ 
/*     */   public void makeUpGater()
/*     */   {
/* 165 */     int sec = BitMapTools.bitCount(this.receivedOwnAnt);
/*     */ 
/* 167 */     while ((sec-- > 0) && (!this.freeAnt.isZero())) {
/* 168 */       this.buff01.copy(this.mingateZone);
/* 169 */       this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 171 */       this.buff02.xor(this.buff02);
/* 172 */       this.buff02.or(this.move00);
/* 173 */       this.buff02.or(this.move01);
/* 174 */       this.buff02.or(this.move02);
/* 175 */       this.buff02.or(this.move03);
/* 176 */       this.freeAnt.xor(this.buff02);
/* 177 */       this.planOrderToSend.currentPlan[0].or(this.move02);
/* 178 */       this.planOrderToSend.currentPlan[1].or(this.move03);
/* 179 */       this.planOrderToSend.currentPlan[2].or(this.move00);
/* 180 */       this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */     }
/*     */   }
/*     */ 
/*     */   public void makeUpExploreUnknown()
/*     */   {
/* 188 */     this.buff03.xor(this.buff03);
/* 189 */     this.buff01.copy(this.refKnownGround);
/* 190 */     this.buff01.or(this.refKnownWater);
/* 191 */     this.buff01.bitnot();
/* 192 */     this.buff01.scramble(this.buff02, this.buff03, this.buff04, 1);
/* 193 */     this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 195 */     this.buff02.xor(this.buff02);
/* 196 */     this.buff02.or(this.move00);
/* 197 */     this.buff02.or(this.move01);
/* 198 */     this.buff02.or(this.move02);
/* 199 */     this.buff02.or(this.move03);
/* 200 */     this.freeAnt.xor(this.buff02);
/* 201 */     this.planOrderToSend.currentPlan[0].or(this.move02);
/* 202 */     this.planOrderToSend.currentPlan[1].or(this.move03);
/* 203 */     this.planOrderToSend.currentPlan[2].or(this.move00);
/* 204 */     this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */   }
/*     */ 
/*     */   public void makeUpExploreControled()
/*     */   {
/* 210 */     this.buff01.copy(this.stateUnrefreshedFriendly);
/* 211 */     this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 213 */     this.buff02.xor(this.buff02);
/* 214 */     this.buff02.or(this.move00);
/* 215 */     this.buff02.or(this.move01);
/* 216 */     this.buff02.or(this.move02);
/* 217 */     this.buff02.or(this.move03);
/* 218 */     this.freeAnt.xor(this.buff02);
/* 219 */     this.planOrderToSend.currentPlan[0].or(this.move02);
/* 220 */     this.planOrderToSend.currentPlan[1].or(this.move03);
/* 221 */     this.planOrderToSend.currentPlan[2].or(this.move00);
/* 222 */     this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */   }
/*     */ 
/*     */   public void makeUpGateControl()
/*     */   {
/* 264 */     this.gateDetect.calcGate(this.refKnownWater, this.refKnownGround);
/* 265 */     this.mingateZone.copy(this.gateDetect.gateResult);
/* 266 */     this.buff01.copy(this.refCalcPotentialEnnemyGround);
/* 267 */     this.buff01.scramble(this.buff04, this.buff02, this.buff03, 1);
/* 268 */     this.buff01.scramble(this.buff04, this.buff02, this.buff03, 1);
/* 269 */     this.buff01.scramble(this.buff04, this.buff02, this.buff03, 1);
/* 270 */     this.buff01.scramble(this.buff04, this.buff02, this.buff03, 1);
/* 271 */     this.buff01.scramble(this.buff04, this.buff02, this.buff03, 1);
/*     */ 
/* 274 */     makeUpGater();
/*     */   }
/*     */ 
/*     */   public void makeUpOrders()
/*     */   {
/* 281 */     makeUpInit();
/* 282 */     makeUpFooder();
/* 283 */     makeUpExploreUnknown();
/* 284 */     makeUpExploreUnknown();
/* 285 */     makeUpExploreUnknown();
/* 286 */     makeUpExploreControled();
/*     */ 
/* 293 */     this.conflictSolver.solveConflictSimple(this.planOrderToSend, this.receivedOwnAnt);
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botGeneration2.HillBotGen2
 * JD-Core Version:    0.6.0
 */