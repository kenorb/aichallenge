/*    */ package botTools;
/*    */ 
/*    */ import bitmapmanip.BitMap;
/*    */ import bitmapmanip.BitMapInt;
/*    */ import bitmapmanip.BitMapTools;
/*    */ import bitmapmanip.BitMapTools.BitIterator;
/*    */ 
/*    */ public class FoodTakePlannerGen2
/*    */ {
/*    */   final BitMapPlanningGen2 commonPlanning;
/*    */   final BitMap cfarthestFood;
/*    */   final BitMap pfoodPropagation;
/*    */   final BitMap pfoodLastPropagation;
/*    */   final BitMap paggentPos;
/*    */   final BitMap buff01;
/*    */   final BitMap buff02;
/*    */   final BitMap buff03;
/*    */   final BitMap ondeAgent;
/*    */   final BitMapInt ra_id;
/*    */   final BitMapInt ra_t;
/*    */   final BitMapInt ra_n;
/*    */   final int[] xstack;
/*    */   final int[] ystack;
/*    */   int xysp;
/*    */   static final int mxnbBit = 9;
/*    */   static final int mxsz = 1024;
/* 40 */   int sprebonant = 0;
/* 41 */   final int maxDistPlan = 80;
/*    */ 
/*    */   public FoodTakePlannerGen2(BitMapPlanningGen2 planning) {
/* 44 */     this.commonPlanning = planning;
/*    */ 
/* 46 */     this.cfarthestFood = new BitMap(planning.width, planning.height);
/* 47 */     this.pfoodPropagation = new BitMap(planning.width, planning.height);
/* 48 */     this.pfoodLastPropagation = new BitMap(planning.width, planning.height);
/* 49 */     this.paggentPos = new BitMap(planning.width, planning.height);
/*    */ 
/* 51 */     this.ondeAgent = new BitMap(planning.width, planning.height);
/*    */ 
/* 53 */     this.buff01 = new BitMap(planning.width, planning.height);
/* 54 */     this.buff02 = new BitMap(planning.width, planning.height);
/* 55 */     this.buff03 = new BitMap(planning.width, planning.height);
/*    */ 
/* 57 */     this.ra_id = new BitMapInt(planning.width, planning.height, 9);
/* 58 */     this.ra_t = new BitMapInt(planning.width, planning.height, 9);
/* 59 */     this.ra_n = new BitMapInt(planning.width, planning.height, 9);
/*    */ 
/* 61 */     this.xstack = new int[1024];
/* 62 */     this.ystack = new int[1024];
/*    */   }
/*    */ 
/*    */   private void goToClosestFoodMultiPlanning(BitMap foodTakePoint, BitMap path)
/*    */   {
/* 67 */     this.pfoodPropagation.copy(foodTakePoint);
/* 68 */     this.paggentPos.copy(this.commonPlanning.currentPlan[4]);
/*    */ 
/* 70 */     this.ra_id.clear();
/* 71 */     this.ra_t.clear();
/* 72 */     this.ra_n.clear();
/*    */   }
/*    */ 
/*    */   public void goToFoodTakePoint(BitMap foodTakePoint, BitMap path)
/*    */   {
/* 88 */     BitMapTools.BitIterator agentDirect = new BitMapTools.BitIterator()
/*    */     {
/*    */       public void at(int x, int y) {
/* 91 */         FoodTakePlannerGen2.this.ondeAgent.xor(FoodTakePlannerGen2.this.ondeAgent);
/* 92 */         FoodTakePlannerGen2.this.ondeAgent.set(x, y, 1L);
/*    */ 
/* 94 */         for (int i = 0; i < 80; i++);
/*    */       }
/*    */     };
/* 99 */     BitMapTools.bitIterate(this.paggentPos, agentDirect);
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botTools.FoodTakePlannerGen2
 * JD-Core Version:    0.6.0
 */