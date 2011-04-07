/*    */ package botTools;
/*    */ 
/*    */ import bitmapmanip.BitMap;
/*    */ import bitmapmanip.BitMapInt;
/*    */ import bitmapmanip.BitMapInt_DistMap;
/*    */ 
/*    */ public class GateDetector
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
/*    */   public GateDetector(int width, int height)
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
/* 49 */     for (int i = 0; i < this.width; i++)
/* 50 */       for (int j = 0; j < this.height; j++) {
/* 51 */         long v = this.distToWater.get(i, j);
/* 52 */         boolean localMax = true;
/* 53 */         if ((v > 3L) && (v > 0L))
/*    */         {
/* 58 */           int px = (i + this.width + 1) % this.width;
/* 59 */           int py = (j + this.width + 0) % this.width;
/* 60 */           long hu = this.distToWater.get(px, py);
/* 61 */           if (v < hu) {
/* 62 */             localMax = false;
/*    */           }
/* 64 */           px = (i + this.width + 0) % this.width;
/* 65 */           py = (j + this.width + 1) % this.width;
/* 66 */           if (v < this.distToWater.get(px, py)) {
/* 67 */             localMax = false;
/*    */           }
/* 69 */           px = (i + this.width - 1) % this.width;
/* 70 */           py = (j + this.width + 0) % this.width;
/* 71 */           if (v < this.distToWater.get(px, py)) {
/* 72 */             localMax = false;
/*    */           }
/* 74 */           px = (i + this.width + 0) % this.width;
/* 75 */           py = (j + this.width - 1) % this.width;
/* 76 */           if (v < this.distToWater.get(px, py))
/* 77 */             localMax = false;
/*    */         }
/*    */         else {
/* 80 */           localMax = false;
/*    */         }
/* 82 */         if (localMax)
/* 83 */           this.gateResult.set(i, j, 1L);
/*    */       }
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botTools.GateDetector
 * JD-Core Version:    0.6.0
 */