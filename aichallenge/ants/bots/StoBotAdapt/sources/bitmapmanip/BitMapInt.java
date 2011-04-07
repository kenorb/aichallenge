/*     */ package bitmapmanip;
/*     */ 
/*     */ public class BitMapInt
/*     */ {
/*     */   final BitMap[] values;
/*     */   final int width;
/*     */   final int height;
/*     */ 
/*     */   public BitMapInt(int w, int h, int nbBits)
/*     */   {
/*  18 */     this.values = new BitMap[nbBits];
/*  19 */     for (int i = 0; i < nbBits; i++) {
/*  20 */       this.values[i] = new BitMap(w, h);
/*     */     }
/*  22 */     this.width = w;
/*  23 */     this.height = h;
/*     */   }
/*     */ 
/*     */   public void clear() {
/*  27 */     for (int i = this.values.length - 1; i >= 0; i--)
/*  28 */       this.values[i].xor(this.values[i]);
/*     */   }
/*     */ 
/*     */   public void setForBitMap(BitMap mask, int v, BitMap invMask)
/*     */   {
/*  33 */     int bitflux = v;
/*  34 */     invMask.copy(mask);
/*  35 */     invMask.bitnot();
/*  36 */     for (int i = 0; i < this.values.length; i++) {
/*  37 */       int t = bitflux & 0x1;
/*  38 */       bitflux >>>= 1;
/*  39 */       this.values[i].and(invMask);
/*  40 */       if (t == 1)
/*  41 */         this.values[i].or(mask);
/*     */     }
/*     */   }
/*     */ 
/*     */   public void getForBitMap(BitMap result, int v, BitMap b001)
/*     */   {
/*  62 */     result.xor(result);
/*  63 */     result.bitnot();
/*     */ 
/*  66 */     int bitflux = v;
/*  67 */     for (int i = 0; i < this.values.length; i++) {
/*  68 */       int t = bitflux & 0x1;
/*  69 */       bitflux >>>= 1;
/*  70 */       if (t == 0) {
/*  71 */         b001.xor(b001);
/*  72 */         b001.bitnot();
/*  73 */         b001.xor(this.values[i]);
/*     */       } else {
/*  75 */         b001.xor(b001);
/*  76 */         b001.bitnot();
/*  77 */         b001.and(this.values[i]);
/*     */       }
/*  79 */       result.and(b001);
/*     */     }
/*     */   }
/*     */ 
/*     */   public long get(int x, int y) {
/*  84 */     long res = 0L;
/*     */ 
/*  86 */     for (int i = this.values.length - 1; i >= 0; i--) {
/*  87 */       res <<= 1;
/*  88 */       res |= this.values[i].get(x, y);
/*     */     }
/*     */ 
/*  91 */     return res;
/*     */   }
/*     */ 
/*     */   public void put(int x, int y, long v) {
/*  95 */     long l = v;
/*  96 */     for (int i = 0; i < this.values.length; i++) {
/*  97 */       long t = l & 1L;
/*  98 */       l >>>= 1;
/*  99 */       this.values[i].set(x, y, t);
/*     */     }
/*     */   }
/*     */ 
/*     */   public String toString() {
/* 104 */     String res = "";
/*     */ 
/* 106 */     for (int i = 0; i < this.width; i++) {
/* 107 */       for (int j = 0; j < this.height; j++) {
/* 108 */         if (get(i, j) != 0L)
/* 109 */           res = res + String.format("%02d|", new Object[] { Long.valueOf(get(i, j)) });
/*     */         else
/* 111 */           res = res + "  |";
/*     */       }
/* 113 */       res = res + "\n";
/*     */     }
/* 115 */     return res;
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     bitmapmanip.BitMapInt
 * JD-Core Version:    0.6.0
 */