/*     */ package botGeneration2;
/*     */ 
/*     */ import backboneImplementation.ObjectivVectorModelImpl;
/*     */ import backboneInterface.BotInterface;
/*     */ import backboneInterface.BotInterface.ServerInitMessage;
/*     */ import backboneInterface.BotInterface.ServerTurnMessage;
/*     */ import backboneInterface.GridViewerModel;
/*     */ import backboneInterface.SeedInterface;
/*     */ import bitmapmanip.BitMap;
/*     */ import bitmapmanip.BitMapTools;
/*     */ import botTools.BitMapPlanningGen2;
/*     */ import java.util.ArrayList;
/*     */ import java.util.List;
/*     */ 
/*     */ public class BaseBotGen2_2
/*     */   implements BotInterface
/*     */ {
/*     */   protected BitMap receivedFood;
/*     */   protected BitMap receivedOwnAnt;
/*     */   protected BitMap refKnownWater;
/*     */   protected BitMap refKnownGround;
/*     */   protected BitMap refSeenMap;
/*     */   protected BitMap calcView;
/*     */   protected BitMap receivedEnnemyAnt;
/*     */   protected BitMap refCalcPotentialEnnemyGround;
/*     */   protected BitMap stateUnrefreshedFriendly;
/*     */   protected BitMap move00;
/*     */   protected BitMap move01;
/*     */   protected BitMap move02;
/*     */   protected BitMap move03;
/*     */   protected BitMap buff01;
/*     */   protected BitMap buff02;
/*     */   protected BitMap buff03;
/*     */   protected BitMap buff04;
/*     */   protected int receiveViewRadius2;
/*     */   protected int width;
/*     */   protected int height;
/*     */   protected BitMapPlanningGen2 planOrderToSend;
/*     */   final List<Integer> xval;
/*     */   final List<Integer> yval;
/*     */   final List<Integer> dirval;
/*     */   public final SeedInterface rand;
/*  68 */   public ObjectivVectorModelImpl objectiv = new ObjectivVectorModelImpl();
/*     */ 
/* 118 */   public final GridViewerModel viewPerceptionServer = new GridViewerModel() {
/*     */     public int width() {
/* 120 */       return BaseBotGen2_2.this.receivedFood.x;
/*     */     }
/*     */ 
/*     */     public int height() {
/* 124 */       return BaseBotGen2_2.this.receivedFood.y;
/*     */     }
/*     */ 
/*     */     public int at(int x, int y)
/*     */     {
/* 132 */       if (BaseBotGen2_2.this.refKnownWater.get(x, y) == 1L) return 2;
/* 133 */       if (BaseBotGen2_2.this.receivedFood.get(x, y) == 1L) return 4;
/* 134 */       if (BaseBotGen2_2.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 135 */       if (BaseBotGen2_2.this.receivedEnnemyAnt.get(x, y) == 1L) return 6;
/* 136 */       if ((BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) && (BaseBotGen2_2.this.calcView.get(x, y) == 1L)) return 3;
/*     */ 
/* 144 */       if (BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) return 1;
/*     */ 
/* 146 */       return 0;
/*     */     }
/* 118 */   };
/*     */ 
/* 150 */   public final GridViewerModel viewOrders = new GridViewerModel() {
/*     */     public int width() {
/* 152 */       return BaseBotGen2_2.this.receivedFood.x;
/*     */     }
/*     */     public int height() {
/* 155 */       return BaseBotGen2_2.this.receivedFood.y;
/*     */     }
/*     */ 
/*     */     public int at(int x, int y)
/*     */     {
/* 163 */       BaseBotGen2_2.this.buff01.copy(BaseBotGen2_2.this.planOrderToSend.currentPlan[0]);
/* 164 */       BaseBotGen2_2.this.buff01.rotateUp(1, BaseBotGen2_2.this.buff02);
/* 165 */       if (BaseBotGen2_2.this.buff01.get(x, y) == 1L) {
/* 166 */         return 7;
/*     */       }
/* 168 */       BaseBotGen2_2.this.buff01.copy(BaseBotGen2_2.this.planOrderToSend.currentPlan[2]);
/* 169 */       BaseBotGen2_2.this.buff01.rotateDown(1, BaseBotGen2_2.this.buff02);
/* 170 */       if (BaseBotGen2_2.this.buff01.get(x, y) == 1L) {
/* 171 */         return 7;
/*     */       }
/* 173 */       BaseBotGen2_2.this.buff01.copy(BaseBotGen2_2.this.planOrderToSend.currentPlan[3]);
/* 174 */       BaseBotGen2_2.this.buff01.rotateLeft(1);
/* 175 */       if (BaseBotGen2_2.this.buff01.get(x, y) == 1L) {
/* 176 */         return 7;
/*     */       }
/* 178 */       BaseBotGen2_2.this.buff01.copy(BaseBotGen2_2.this.planOrderToSend.currentPlan[1]);
/* 179 */       BaseBotGen2_2.this.buff01.rotateRight(1);
/* 180 */       if (BaseBotGen2_2.this.buff01.get(x, y) == 1L) {
/* 181 */         return 7;
/*     */       }
/*     */ 
/* 184 */       if (BaseBotGen2_2.this.refKnownWater.get(x, y) == 1L) return 2;
/* 185 */       if (BaseBotGen2_2.this.receivedFood.get(x, y) == 1L) return 4;
/* 186 */       if (BaseBotGen2_2.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 187 */       if (BaseBotGen2_2.this.receivedEnnemyAnt.get(x, y) == 1L) return 6;
/* 188 */       if ((BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) && (BaseBotGen2_2.this.calcView.get(x, y) == 1L)) return 3;
/* 189 */       if (BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) return 1;
/* 190 */       return 0;
/*     */     }
/* 150 */   };
/*     */ 
/* 195 */   public final GridViewerModel viewPotentialEnnemyZone = new GridViewerModel() {
/*     */     public int width() {
/* 197 */       return BaseBotGen2_2.this.receivedFood.x;
/*     */     }
/*     */     public int height() {
/* 200 */       return BaseBotGen2_2.this.receivedFood.y;
/*     */     }
/*     */     public int at(int x, int y) {
/* 203 */       if (BaseBotGen2_2.this.refCalcPotentialEnnemyGround.get(x, y) == 1L) return 6;
/* 204 */       if (BaseBotGen2_2.this.refKnownWater.get(x, y) == 1L) return 2;
/* 205 */       if (BaseBotGen2_2.this.receivedFood.get(x, y) == 1L) return 4;
/* 206 */       if (BaseBotGen2_2.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 207 */       if ((BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) && (BaseBotGen2_2.this.calcView.get(x, y) == 1L)) return 3;
/* 208 */       if (BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) return 1;
/* 209 */       return 0;
/*     */     }
/* 195 */   };
/*     */ 
/* 213 */   public final GridViewerModel viewUnrefreshedFriendlyZone = new GridViewerModel() {
/*     */     public int width() {
/* 215 */       return BaseBotGen2_2.this.receivedFood.x;
/*     */     }
/*     */     public int height() {
/* 218 */       return BaseBotGen2_2.this.receivedFood.y;
/*     */     }
/*     */     public int at(int x, int y) {
/* 221 */       if (BaseBotGen2_2.this.stateUnrefreshedFriendly.get(x, y) == 1L) return 6;
/* 222 */       if (BaseBotGen2_2.this.refKnownWater.get(x, y) == 1L) return 2;
/* 223 */       if (BaseBotGen2_2.this.receivedFood.get(x, y) == 1L) return 4;
/* 224 */       if (BaseBotGen2_2.this.receivedOwnAnt.get(x, y) == 1L) return 5;
/* 225 */       if ((BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) && (BaseBotGen2_2.this.calcView.get(x, y) == 1L)) return 3;
/* 226 */       if (BaseBotGen2_2.this.refKnownGround.get(x, y) == 1L) return 1;
/* 227 */       return 0;
/*     */     }
/* 213 */   };
/*     */ 
/*     */   public BaseBotGen2_2(SeedInterface rand)
/*     */   {
/*  62 */     this.rand = rand;
/*  63 */     this.xval = new ArrayList();
/*  64 */     this.yval = new ArrayList();
/*  65 */     this.dirval = new ArrayList();
/*     */   }
/*     */ 
/*     */   public int nbAntWithOrderForNextTurn()
/*     */   {
/*  71 */     return this.xval.size();
/*     */   }
/*     */ 
/*     */   public int getXBotOrderForNextTurn(int i) {
/*  75 */     return ((Integer)this.xval.get(i)).intValue();
/*     */   }
/*     */ 
/*     */   public int getYBotOrderForNextTurn(int i) {
/*  79 */     return ((Integer)this.yval.get(i)).intValue();
/*     */   }
/*     */ 
/*     */   public int getDirBotOrderForNextTurn(int i) {
/*  83 */     return ((Integer)this.dirval.get(i)).intValue();
/*     */   }
/*     */ 
/*     */   public void init(BotInterface.ServerInitMessage init) {
/*  87 */     this.width = init.cols();
/*  88 */     this.height = init.rows();
/*  89 */     this.receivedFood = new BitMap(this.width, this.height);
/*  90 */     this.receivedOwnAnt = new BitMap(this.width, this.height);
/*  91 */     this.refKnownWater = new BitMap(this.width, this.height);
/*  92 */     this.refKnownGround = new BitMap(this.width, this.height);
/*  93 */     this.refSeenMap = new BitMap(this.width, this.height);
/*  94 */     this.refCalcPotentialEnnemyGround = new BitMap(this.width, this.height);
/*  95 */     this.receivedEnnemyAnt = new BitMap(this.width, this.height);
/*  96 */     this.stateUnrefreshedFriendly = new BitMap(this.width, this.height);
/*     */ 
/*  98 */     this.buff01 = new BitMap(this.width, this.height);
/*  99 */     this.buff02 = new BitMap(this.width, this.height);
/* 100 */     this.buff03 = new BitMap(this.width, this.height);
/* 101 */     this.buff04 = new BitMap(this.width, this.height);
/*     */ 
/* 103 */     this.move00 = new BitMap(this.width, this.height);
/* 104 */     this.move01 = new BitMap(this.width, this.height);
/* 105 */     this.move02 = new BitMap(this.width, this.height);
/* 106 */     this.move03 = new BitMap(this.width, this.height);
/*     */ 
/* 108 */     this.calcView = new BitMap(this.width, this.height);
/* 109 */     this.planOrderToSend = new BitMapPlanningGen2(this.receivedFood, this.rand);
/*     */ 
/* 111 */     this.receiveViewRadius2 = init.viewradius2();
/*     */ 
/* 113 */     this.refCalcPotentialEnnemyGround.bitnot();
/*     */   }
/*     */ 
/*     */   public void turnStart(BotInterface.ServerTurnMessage mess)
/*     */   {
/* 233 */     this.xval.clear();
/* 234 */     this.yval.clear();
/* 235 */     this.dirval.clear();
/*     */ 
/* 237 */     this.objectiv.clear();
/*     */ 
/* 239 */     this.receivedFood.xor(this.receivedFood);
/* 240 */     this.receivedOwnAnt.xor(this.receivedOwnAnt);
/* 241 */     this.receivedEnnemyAnt.xor(this.receivedEnnemyAnt);
/* 242 */     for (int i = 0; i < mess.getNbYourAnt(); i++) {
/* 243 */       this.receivedOwnAnt.set(mess.getXYourAntPos(i), mess.getYYourAntPos(i), 1L);
/*     */     }
/* 245 */     for (int i = 0; i < mess.getNbViewedFood(); i++) {
/* 246 */       this.receivedFood.set(mess.getXViewedFoodPos(i), mess.getYViewedFoodPos(i), 1L);
/*     */     }
/* 248 */     for (int i = 0; i < mess.getNbFirstViewedWater(); i++) {
/* 249 */       this.refKnownWater.set(mess.getXFirstViewedWater(i), mess.getYFirstViewedWater(i), 1L);
/*     */     }
/* 251 */     for (int i = 0; i < mess.getNbOtherAnt(); i++) {
/* 252 */       this.receivedEnnemyAnt.set(mess.getXViewedAnt(i), mess.getYViewedAnt(i), 1L);
/*     */     }
/*     */ 
/* 255 */     BitMapTools.diskToAllPointRotate(this.receivedOwnAnt, this.buff01, this.calcView, this.buff02, this.receiveViewRadius2);
/* 256 */     this.buff01.copy(this.refKnownWater);
/* 257 */     this.buff01.bitnot();
/* 258 */     this.buff01.and(this.calcView);
/*     */ 
/* 260 */     this.refSeenMap.or(this.calcView);
/* 261 */     this.refKnownGround.or(this.buff01);
/*     */ 
/* 265 */     this.buff01.copy(this.calcView);
/* 266 */     this.buff01.or(this.refKnownWater);
/* 267 */     this.buff01.bitnot();
/* 268 */     this.stateUnrefreshedFriendly.and(this.buff01);
/* 269 */     this.refCalcPotentialEnnemyGround.and(this.buff01);
/* 270 */     this.refCalcPotentialEnnemyGround.or(this.receivedEnnemyAnt);
/* 271 */     this.refCalcPotentialEnnemyGround.scramble(this.buff01, this.buff02, this.buff03, 1);
/* 272 */     this.buff03.copy(this.refKnownWater);
/* 273 */     this.buff03.bitnot();
/* 274 */     this.refCalcPotentialEnnemyGround.and(this.buff03);
/*     */ 
/* 276 */     this.buff02.copy(this.refCalcPotentialEnnemyGround);
/* 277 */     this.buff02.bitnot();
/* 278 */     this.stateUnrefreshedFriendly.and(this.buff02);
/*     */ 
/* 280 */     if (this.stateUnrefreshedFriendly.isZero()) {
/* 281 */       this.stateUnrefreshedFriendly.copy(this.refKnownGround);
/* 282 */       this.buff01.copy(this.refCalcPotentialEnnemyGround);
/* 283 */       this.buff01.bitnot();
/* 284 */       this.buff02.copy(this.calcView);
/* 285 */       this.buff02.bitnot();
/* 286 */       this.stateUnrefreshedFriendly.and(this.buff01);
/* 287 */       this.stateUnrefreshedFriendly.and(this.buff02);
/*     */     }
/*     */ 
/* 292 */     makeUpOrders();
/*     */ 
/* 294 */     this.planOrderToSend.spoolPlan(this.xval, this.yval, this.dirval);
/*     */   }
/*     */ 
/*     */   public void makeUpOrders() {
/* 298 */     for (int i = 0; i < 5; i++)
/* 299 */       this.planOrderToSend.currentPlan[i].copy(this.receivedOwnAnt);
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botGeneration2.BaseBotGen2_2
 * JD-Core Version:    0.6.0
 */