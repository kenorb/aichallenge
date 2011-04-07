/*     */ package botGeneration2;
/*     */ 
/*     */ import backboneInterface.BotInterface.ServerInitMessage;
/*     */ import backboneInterface.GridViewerModel;
/*     */ import backboneInterface.SeedInterface;
/*     */ import bitmapmanip.BitMap;
/*     */ import bitmapmanip.BitMapTools;
/*     */ import bitmapmanip.BitMapTools.BitIterator;
/*     */ import botTools.BitMapPlanningGen2;
/*     */ import botTools.ConflictPushPlanner;
/*     */ import botTools.TenozanDetector;
/*     */ 
/*     */ public class BlobBot extends BaseBotGen2_2
/*     */ {
/*     */   BitMap consideredFood;
/*     */   BitMap desiredFood;
/*     */   BitMap freeAnt;
/*     */   BitMap gateZone;
/*     */   BitMap mingateZone;
/*     */   int minNumberZone;
/*     */   BitMap consideredGates;
/*     */   BitMap desiredGates;
/*     */   ConflictPushPlanner conflictSolver;
/*     */   TenozanDetector tenozanDetect;
/*  35 */   boolean stopExplo = false;
/*     */ 
/*  59 */   boolean oneAntActiv = false;
/*  60 */   boolean emergencyBreak = false;
/*     */ 
/*  62 */   int planNum = 0;
/*     */ 
/* 237 */   public final GridViewerModel viewGateZone = new GridViewerModel() {
/*     */     public int width() {
/* 239 */       return BlobBot.this.receivedFood.x;
/*     */     }
/*     */     public int height() {
/* 242 */       return BlobBot.this.receivedFood.y;
/*     */     }
/*     */     public int at(int x, int y) {
/* 245 */       if (BlobBot.this.gateZone.get(x, y) == 1L) return 6;
/* 246 */       if (BlobBot.this.refKnownWater.get(x, y) == 1L) return 2;
/* 247 */       if (BlobBot.this.receivedFood.get(x, y) == 1L) return 4;
/* 248 */       if (BlobBot.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 249 */       if ((BlobBot.this.refKnownGround.get(x, y) == 1L) && (BlobBot.this.calcView.get(x, y) == 1L)) return 3;
/* 250 */       if (BlobBot.this.refKnownGround.get(x, y) == 1L) return 1;
/* 251 */       return 0;
/*     */     }
/* 237 */   };
/*     */ 
/* 255 */   public final GridViewerModel viewMinGate = new GridViewerModel() {
/*     */     public int width() {
/* 257 */       return BlobBot.this.receivedFood.x;
/*     */     }
/*     */     public int height() {
/* 260 */       return BlobBot.this.receivedFood.y;
/*     */     }
/*     */     public int at(int x, int y) {
/* 263 */       if (BlobBot.this.mingateZone.get(x, y) == 1L) return 6;
/* 264 */       if (BlobBot.this.refKnownWater.get(x, y) == 1L) return 2;
/* 265 */       if (BlobBot.this.receivedFood.get(x, y) == 1L) return 4;
/* 266 */       if (BlobBot.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 267 */       if ((BlobBot.this.refKnownGround.get(x, y) == 1L) && (BlobBot.this.calcView.get(x, y) == 1L)) return 3;
/* 268 */       if (BlobBot.this.refKnownGround.get(x, y) == 1L) return 1;
/* 269 */       return 0;
/*     */     }
/* 255 */   };
/*     */ 
/*     */   public BlobBot(SeedInterface rand)
/*     */   {
/*  38 */     super(rand);
/*     */   }
/*     */ 
/*     */   public void init(BotInterface.ServerInitMessage init)
/*     */   {
/*  43 */     super.init(init);
/*     */ 
/*  45 */     this.desiredFood = new BitMap(this.width, this.height);
/*  46 */     this.consideredFood = new BitMap(this.width, this.height);
/*  47 */     this.freeAnt = new BitMap(this.width, this.height);
/*  48 */     this.gateZone = new BitMap(this.width, this.height);
/*  49 */     this.consideredGates = new BitMap(this.width, this.height);
/*  50 */     this.desiredGates = new BitMap(this.width, this.height);
/*  51 */     this.mingateZone = new BitMap(this.width, this.height);
/*     */ 
/*  53 */     this.conflictSolver = new ConflictPushPlanner(this.width, this.height, this.rand);
/*  54 */     this.tenozanDetect = new TenozanDetector(this.width, this.height);
/*  55 */     this.minNumberZone = 10000;
/*     */   }
/*     */ 
/*     */   public void makeUpInit()
/*     */   {
/*  65 */     for (int i = 0; i < this.planOrderToSend.currentPlan.length; i++) {
/*  66 */       this.planOrderToSend.currentPlan[i].xor(this.planOrderToSend.currentPlan[i]);
/*     */     }
/*     */ 
/*  69 */     this.freeAnt.copy(this.receivedOwnAnt);
/*  70 */     this.desiredFood.copy(this.receivedFood);
/*     */ 
/*  72 */     this.buff01.copy(this.receivedFood);
/*  73 */     this.buff01.rotateDown(1, this.buff02);
/*  74 */     this.buff01.and(this.freeAnt);
/*  75 */     this.buff03.copy(this.buff01);
/*  76 */     this.buff01.copy(this.receivedFood);
/*  77 */     this.buff01.rotateUp(1, this.buff02);
/*  78 */     this.buff01.and(this.freeAnt);
/*  79 */     this.buff03.or(this.buff01);
/*  80 */     this.buff01.copy(this.receivedFood);
/*  81 */     this.buff01.rotateLeft(1);
/*  82 */     this.buff01.and(this.freeAnt);
/*  83 */     this.buff03.or(this.buff01);
/*  84 */     this.buff01.copy(this.receivedFood);
/*  85 */     this.buff01.rotateRight(1);
/*  86 */     this.buff01.and(this.freeAnt);
/*  87 */     this.buff03.or(this.buff01);
/*     */ 
/*  89 */     this.buff03.bitnot();
/*  90 */     this.freeAnt.and(this.buff03);
/*     */ 
/*  93 */     this.planNum = 0;
/*     */   }
/*     */ 
/*     */   public void makeUpFooder() {
/*  97 */     while ((!this.desiredFood.isZero()) && (!this.freeAnt.isZero()) && (!this.emergencyBreak))
/*     */     {
/*  99 */       this.buff01.copy(this.freeAnt);
/* 100 */       this.buff01.nearestUpTo(this.desiredFood, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 102 */       this.consideredFood.copy(this.move00);
/* 103 */       this.consideredFood.or(this.move01);
/* 104 */       this.consideredFood.or(this.move02);
/* 105 */       this.consideredFood.or(this.move03);
/*     */ 
/* 107 */       if (this.consideredFood.isZero()) {
/*     */         break;
/*     */       }
/* 110 */       BitMapTools.BitIterator itOnFood = new BitMapTools.BitIterator()
/*     */       {
/*     */         public void at(int px, int py) {
/* 113 */           BlobBot.this.buff01.xor(BlobBot.this.buff01);
/* 114 */           BlobBot.this.buff01.set(px, py, 1L);
/*     */ 
/* 116 */           BlobBot.this.buff01.nearestUpTo(BlobBot.this.freeAnt, BlobBot.this.move00, BlobBot.this.move01, BlobBot.this.move02, BlobBot.this.move03, BlobBot.this.refKnownGround, BlobBot.this.buff02, BlobBot.this.buff03, BlobBot.this.buff04);
/*     */ 
/* 120 */           BlobBot.this.buff02.xor(BlobBot.this.buff02);
/* 121 */           BlobBot.this.buff02.or(BlobBot.this.move00);
/* 122 */           BlobBot.this.buff02.or(BlobBot.this.move01);
/* 123 */           BlobBot.this.buff02.or(BlobBot.this.move02);
/* 124 */           BlobBot.this.buff02.or(BlobBot.this.move03);
/*     */ 
/* 126 */           BlobBot.this.oneAntActiv = false;
/*     */ 
/* 128 */           BitMapTools.BitIterator itOnAntTouched = new BitMapTools.BitIterator(px, py)
/*     */           {
/*     */             public void at(int x, int y) {
/* 131 */               if (BlobBot.this.oneAntActiv) return;
/* 132 */               BlobBot.this.buff03.xor(BlobBot.this.buff03);
/* 133 */               BlobBot.this.buff03.set(x, y, 1L);
/* 134 */               BlobBot.this.move00.and(BlobBot.this.buff03);
/* 135 */               BlobBot.this.move01.and(BlobBot.this.buff03);
/* 136 */               BlobBot.this.move02.and(BlobBot.this.buff03);
/* 137 */               BlobBot.this.move03.and(BlobBot.this.buff03);
/*     */ 
/* 139 */               BlobBot.this.desiredFood.set(this.val$px, this.val$py, 0L);
/* 140 */               BlobBot.this.freeAnt.set(x, y, 0L);
/* 141 */               BlobBot.this.oneAntActiv = true;
/*     */             }
/*     */           };
/* 154 */           BitMapTools.bitIterate(BlobBot.this.buff02, itOnAntTouched);
/* 155 */           BlobBot.this.planOrderToSend.currentPlan[0].or(BlobBot.this.move02);
/* 156 */           BlobBot.this.planOrderToSend.currentPlan[1].or(BlobBot.this.move03);
/* 157 */           BlobBot.this.planOrderToSend.currentPlan[2].or(BlobBot.this.move00);
/* 158 */           BlobBot.this.planOrderToSend.currentPlan[3].or(BlobBot.this.move01);
/*     */         }
/*     */       };
/* 161 */       BitMapTools.bitIterate(this.consideredFood, itOnFood);
/*     */     }
/*     */   }
/*     */ 
/*     */   public void makeUpGater()
/*     */   {
/* 170 */     int sec = BitMapTools.bitCount(this.receivedOwnAnt);
/*     */ 
/* 172 */     this.buff01.copy(this.mingateZone);
/* 173 */     this.buff01.and(this.freeAnt);
/* 174 */     this.freeAnt.xor(this.buff01);
/* 175 */     this.mingateZone.xor(this.buff01);
/*     */ 
/* 177 */     while ((sec-- > 0) && (!this.freeAnt.isZero())) {
/* 178 */       this.buff01.copy(this.mingateZone);
/* 179 */       this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 181 */       this.buff02.xor(this.buff02);
/* 182 */       this.buff02.or(this.move00);
/* 183 */       this.buff02.or(this.move01);
/* 184 */       this.buff02.or(this.move02);
/* 185 */       this.buff02.or(this.move03);
/* 186 */       this.freeAnt.xor(this.buff02);
/* 187 */       this.planOrderToSend.currentPlan[0].or(this.move02);
/* 188 */       this.planOrderToSend.currentPlan[1].or(this.move03);
/* 189 */       this.planOrderToSend.currentPlan[2].or(this.move00);
/* 190 */       this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */     }
/*     */   }
/*     */ 
/*     */   public void makeUpExploreUnknown()
/*     */   {
/* 198 */     this.buff03.xor(this.buff03);
/* 199 */     this.buff01.copy(this.refKnownGround);
/* 200 */     this.buff01.or(this.refKnownWater);
/* 201 */     this.buff01.bitnot();
/* 202 */     this.buff01.scramble(this.buff02, this.buff03, this.buff04, 1);
/* 203 */     this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 205 */     this.buff02.xor(this.buff02);
/* 206 */     this.buff02.or(this.move00);
/* 207 */     this.buff02.or(this.move01);
/* 208 */     this.buff02.or(this.move02);
/* 209 */     this.buff02.or(this.move03);
/* 210 */     this.freeAnt.xor(this.buff02);
/* 211 */     this.planOrderToSend.currentPlan[0].or(this.move02);
/* 212 */     this.planOrderToSend.currentPlan[1].or(this.move03);
/* 213 */     this.planOrderToSend.currentPlan[2].or(this.move00);
/* 214 */     this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */   }
/*     */ 
/*     */   public void makeUpExploreControled()
/*     */   {
/* 220 */     this.buff01.copy(this.stateUnrefreshedFriendly);
/* 221 */     this.buff01.nearestUpTo(this.freeAnt, this.move00, this.move01, this.move02, this.move03, this.refKnownGround, this.buff02, this.buff03, this.buff04);
/*     */ 
/* 223 */     this.buff02.xor(this.buff02);
/* 224 */     this.buff02.or(this.move00);
/* 225 */     this.buff02.or(this.move01);
/* 226 */     this.buff02.or(this.move02);
/* 227 */     this.buff02.or(this.move03);
/* 228 */     this.freeAnt.xor(this.buff02);
/* 229 */     this.planOrderToSend.currentPlan[0].or(this.move02);
/* 230 */     this.planOrderToSend.currentPlan[1].or(this.move03);
/* 231 */     this.planOrderToSend.currentPlan[2].or(this.move00);
/* 232 */     this.planOrderToSend.currentPlan[3].or(this.move01);
/*     */   }
/*     */ 
/*     */   public void makeUpGateControl()
/*     */   {
/* 275 */     if (!this.freeAnt.isZero()) {
/* 276 */       this.stopExplo = true;
/*     */     }
/* 278 */     this.buff01.copy(this.refKnownGround);
/* 279 */     this.buff01.bitnot();
/* 280 */     this.tenozanDetect.calcGate(this.buff01, this.refKnownGround);
/* 281 */     this.mingateZone.copy(this.tenozanDetect.gateResult);
/*     */ 
/* 283 */     this.consideredGates.copy(this.mingateZone);
/* 284 */     this.buff01.copy(this.mingateZone);
/* 285 */     this.buff01.rotateDown(1, this.buff02);
/* 286 */     this.buff01.rotateLeft(1);
/* 287 */     this.consideredGates.or(this.buff01);
/* 288 */     this.buff01.copy(this.mingateZone);
/* 289 */     this.buff01.rotateDown(1, this.buff02);
/* 290 */     this.buff01.rotateRight(1);
/* 291 */     this.consideredGates.or(this.buff01);
/* 292 */     this.buff01.copy(this.mingateZone);
/* 293 */     this.buff01.rotateUp(1, this.buff02);
/* 294 */     this.buff01.rotateLeft(1);
/* 295 */     this.consideredGates.or(this.buff01);
/* 296 */     this.buff01.copy(this.mingateZone);
/* 297 */     this.buff01.rotateUp(1, this.buff02);
/* 298 */     this.buff01.rotateRight(1);
/* 299 */     this.consideredGates.or(this.buff01);
/* 300 */     this.buff01.copy(this.mingateZone);
/* 301 */     this.buff01.scramble(this.buff04, this.buff02, this.buff03, 1);
/* 302 */     this.consideredGates.or(this.buff01);
/*     */ 
/* 304 */     this.mingateZone.xor(this.consideredGates);
/* 305 */     int maxSec = 10;
/*     */ 
/* 308 */     while ((!this.consideredGates.isZero()) && (BitMapTools.bitCount(this.consideredGates) < BitMapTools.bitCount(this.freeAnt)) && (maxSec > 0)) {
/* 309 */       this.consideredGates.copy(this.mingateZone);
/* 310 */       this.buff01.copy(this.mingateZone);
/* 311 */       this.buff01.rotateDown(1, this.buff02);
/* 312 */       this.buff01.rotateLeft(1);
/* 313 */       this.consideredGates.or(this.buff01);
/* 314 */       this.buff01.copy(this.mingateZone);
/* 315 */       this.buff01.rotateDown(1, this.buff02);
/* 316 */       this.buff01.rotateRight(1);
/* 317 */       this.consideredGates.or(this.buff01);
/* 318 */       this.buff01.copy(this.mingateZone);
/* 319 */       this.buff01.rotateUp(1, this.buff02);
/* 320 */       this.buff01.rotateLeft(1);
/* 321 */       this.consideredGates.or(this.buff01);
/* 322 */       this.buff01.copy(this.mingateZone);
/* 323 */       this.buff01.rotateUp(1, this.buff02);
/* 324 */       this.buff01.rotateRight(1);
/* 325 */       this.consideredGates.or(this.buff01);
/* 326 */       this.buff01.copy(this.mingateZone);
/* 327 */       this.buff01.scramble(this.buff04, this.buff02, this.buff03, 1);
/* 328 */       this.consideredGates.or(this.buff01);
/*     */ 
/* 330 */       this.mingateZone.xor(this.consideredGates);
/*     */ 
/* 332 */       maxSec--;
/*     */     }
/*     */ 
/* 353 */     this.mingateZone.and(this.refKnownGround);
/*     */ 
/* 362 */     makeUpGater();
/*     */   }
/*     */ 
/*     */   public void makeUpOrders()
/*     */   {
/* 369 */     makeUpInit();
/*     */ 
/* 373 */     makeUpFooder();
/*     */ 
/* 376 */     if (!this.stopExplo)
/*     */     {
/* 378 */       makeUpExploreUnknown();
/* 379 */       makeUpExploreUnknown();
/* 380 */       makeUpExploreUnknown();
/* 381 */       makeUpExploreControled();
/*     */     }
/*     */ 
/* 384 */     makeUpGateControl();
/*     */ 
/* 386 */     if (this.stopExplo);
/* 394 */     this.conflictSolver.solveConflictSimple(this.planOrderToSend, this.receivedOwnAnt);
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botGeneration2.BlobBot
 * JD-Core Version:    0.6.0
 */