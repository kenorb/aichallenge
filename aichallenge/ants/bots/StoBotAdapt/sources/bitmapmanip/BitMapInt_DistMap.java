/*    */ package bitmapmanip;
/*    */ 
/*    */ public class BitMapInt_DistMap
/*    */ {
/*    */   final BitMap buff001;
/*    */   final BitMap buff002;
/*    */   final BitMap buff003;
/*    */   final BitMap next;
/*    */   final BitMap prev;
/*    */   final BitMap onde;
/*    */   final int width;
/*    */   final int height;
/*    */ 
/*    */   public BitMapInt_DistMap(int width, int height)
/*    */   {
/* 24 */     this.width = width;
/* 25 */     this.height = height;
/* 26 */     this.buff001 = new BitMap(width, height);
/* 27 */     this.buff002 = new BitMap(width, height);
/* 28 */     this.buff003 = new BitMap(width, height);
/* 29 */     this.next = new BitMap(width, height);
/* 30 */     this.prev = new BitMap(width, height);
/* 31 */     this.onde = new BitMap(width, height);
/*    */   }
/*    */ 
/*    */   public final void makeDistMap(BitMap src, BitMap path, BitMapInt res) {
/* 35 */     this.prev.copy(src);
/* 36 */     this.prev.and(path);
/* 37 */     res.clear();
/* 38 */     int dist = 1;
/* 39 */     this.buff001.copy(this.prev);
/* 40 */     this.buff001.bitnot();
/* 41 */     res.setForBitMap(this.prev, dist++, this.buff001);
/*    */ 
/* 43 */     this.next.copy(this.prev);
/*    */     while (true)
/*    */     {
/* 48 */       this.next.scramble(this.buff001, this.buff002, this.buff003, 1);
/* 49 */       this.next.and(path);
/* 50 */       this.prev.xor(this.next);
/* 51 */       if (this.prev.isZero())
/*    */         break;
/* 53 */       this.buff001.copy(this.prev);
/* 54 */       this.buff001.bitnot();
/* 55 */       res.setForBitMap(this.prev, dist++, this.buff001);
/*    */ 
/* 57 */       this.prev.copy(this.next);
/*    */     }
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     bitmapmanip.BitMapInt_DistMap
 * JD-Core Version:    0.6.0
 */