/*    */ package botTools;
/*    */ 
/*    */ import bitmapmanip.BitMap;
/*    */ import bitmapmanip.BitMapInt;
/*    */ import bitmapmanip.BitMapInt_DistMap;
/*    */ 
/*    */ public class TenozanDetector
/*    */ {
/*    */   final BitMapInt distToWater;
/*    */   final BitMapInt_DistMap calc;
/*    */   public final BitMap gateResult;
/*    */   final BitMap b001;
/*    */   final BitMap b002;
/*    */   final BitMap b003;
/*    */   final int width;
/*    */   final int height;
/*    */   static final int distBit = 9;
/*    */ 
/*    */   public TenozanDetector(int width, int height)
/*    */   {
/* 29 */     this.width = width;
/* 30 */     this.height = height;
/*    */ 
/* 32 */     this.distToWater = new BitMapInt(width, height, 9);
/* 33 */     this.calc = new BitMapInt_DistMap(width, height);
/* 34 */     this.gateResult = new BitMap(width, height);
/* 35 */     this.b001 = new BitMap(width, height);
/* 36 */     this.b002 = new BitMap(width, height);
/* 37 */     this.b003 = new BitMap(width, height);
/*    */   }
/*    */ 
/*    */   public void calcGate(BitMap knownWater, BitMap knownGround)
/*    */   {
/* 42 */     this.gateResult.copy(knownWater);
/* 43 */     this.gateResult.scramble(this.b001, this.b002, this.b003, 1);
/* 44 */     this.calc.makeDistMap(this.gateResult, knownGround, this.distToWater);
/*    */ 
/* 48 */     this.gateResult.xor(this.gateResult);
/*    */ 
/* 50 */     int max = 0;
/* 51 */     int xp = 0;
/* 52 */     int yp = 0;
/* 53 */     for (int i = 0; i < this.width; i++) {
/* 54 */       for (int j = 0; j < this.height; j++) {
/* 55 */         int v = (int)this.distToWater.get(i, j);
/* 56 */         if (v > max) {
/* 57 */           max = v;
/* 58 */           xp = i;
/* 59 */           yp = j;
/*    */         }
/*    */       }
/*    */     }
/*    */ 
/* 64 */     this.gateResult.set(xp, yp, 1L);
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botTools.TenozanDetector
 * JD-Core Version:    0.6.0
 */