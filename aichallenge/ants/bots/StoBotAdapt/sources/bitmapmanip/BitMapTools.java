/*     */ package bitmapmanip;
/*     */ 
/*     */ import backboneInterface.SeedInterface;
/*     */ 
/*     */ public class BitMapTools
/*     */ {
/*     */   public static int numFirstBit(long bits)
/*     */   {
/*  23 */     for (int i = 0; i < 64; i++) {
/*  24 */       if ((bits >> i & 1L) == 1L) return i;
/*     */     }
/*  26 */     return 64;
/*     */   }
/*     */ 
/*     */   public static int bitCount(BitMap src)
/*     */   {
/*  31 */     int res = 0;
/*  32 */     for (int j = 0; j < src.y; j++) {
/*  33 */       int posxoffset = 0;
/*  34 */       for (int ib = 0; ib < src.xnbmap; ib++) {
/*  35 */         long v = src.maps[j][ib];
/*  36 */         while (v != 0L) {
/*  37 */           long k = v;
/*  38 */           v &= v - 1L;
/*  39 */           k ^= v;
/*  40 */           res++;
/*     */         }
/*  42 */         posxoffset += 64;
/*     */       }
/*     */     }
/*     */ 
/*  46 */     return res;
/*     */   }
/*     */ 
/*     */   public static void bitIterate(BitMap src, BitIterator call) {
/*  50 */     for (int j = 0; j < src.y; j++) {
/*  51 */       int posxoffset = 0;
/*  52 */       for (int ib = 0; ib < src.xnbmap; ib++) {
/*  53 */         long v = src.maps[j][ib];
/*  54 */         while (v != 0L) {
/*  55 */           long k = v;
/*  56 */           v &= v - 1L;
/*  57 */           k ^= v;
/*  58 */           call.at(posxoffset + numFirstBit(k), j);
/*     */         }
/*  60 */         posxoffset += 64;
/*     */       }
/*     */     }
/*     */   }
/*     */ 
/*     */   public static void diskToAllPointLimit(BitMap src, BitMap buff, BitMap dest, int sqrad)
/*     */   {
/*  68 */     dest.xor(dest);
/*  69 */     for (int i = 0; i * i <= sqrad; i++)
/*  70 */       for (int j = 0; i * i + j * j <= sqrad; j++)
/*     */       {
/*  72 */         buff.copy(src);
/*  73 */         buff.shiftRight(i);
/*  74 */         buff.shiftDown(j);
/*  75 */         dest.or(buff);
/*     */ 
/*  77 */         buff.copy(src);
/*  78 */         buff.shiftLeft(i);
/*  79 */         buff.shiftDown(j);
/*  80 */         dest.or(buff);
/*     */ 
/*  82 */         buff.copy(src);
/*  83 */         buff.shiftRight(i);
/*  84 */         buff.shiftUp(j);
/*  85 */         dest.or(buff);
/*     */ 
/*  87 */         buff.copy(src);
/*  88 */         buff.shiftLeft(i);
/*  89 */         buff.shiftUp(j);
/*  90 */         dest.or(buff);
/*     */       }
/*     */   }
/*     */ 
/*     */   public static void diskToAllPointRotate(BitMap src, BitMap buff, BitMap dest, BitMap buff2, int sqrad)
/*     */   {
/*  97 */     dest.xor(dest);
/*  98 */     for (int i = 0; i * i <= sqrad; i++)
/*  99 */       for (int j = 0; i * i + j * j <= sqrad; j++)
/*     */       {
/* 101 */         buff.copy(src);
/* 102 */         buff.rotateRight(i);
/* 103 */         buff.rotateDown(j, buff2);
/* 104 */         dest.or(buff);
/*     */ 
/* 106 */         buff.copy(src);
/* 107 */         buff.rotateLeft(i);
/* 108 */         buff.rotateDown(j, buff2);
/* 109 */         dest.or(buff);
/*     */ 
/* 111 */         buff.copy(src);
/* 112 */         buff.rotateRight(i);
/* 113 */         buff.rotateUp(j, buff2);
/* 114 */         dest.or(buff);
/*     */ 
/* 116 */         buff.copy(src);
/* 117 */         buff.rotateLeft(i);
/* 118 */         buff.rotateUp(j, buff2);
/* 119 */         dest.or(buff);
/*     */       }
/*     */   }
/*     */ 
/*     */   public static void plainDisk(BitMap m, int x, int y, int sqrad)
/*     */   {
/* 125 */     m.set(x, y, 1L);
/*     */ 
/* 127 */     for (int i = 0; i * i <= sqrad; i++)
/* 128 */       for (int j = 0; i * i + j * j <= sqrad; j++) {
/* 129 */         m.secureSet(i + x, j + y, 1L);
/* 130 */         m.secureSet(-i + x, j + y, 1L);
/* 131 */         m.secureSet(i + x, -j + y, 1L);
/* 132 */         m.secureSet(-i + x, -j + y, 1L);
/*     */       }
/*     */   }
/*     */ 
/*     */   public static void randomWalkGenerator(BitMap res, int percentG, SeedInterface seed)
/*     */   {
/* 139 */     int width = res.x;
/* 140 */     int height = res.y;
/*     */ 
/* 142 */     int xpos = seed.next() & 0x7FFFFFFF;
/* 143 */     int ypos = seed.next() & 0x7FFFFFFF;
/*     */ 
/* 145 */     res.xor(res);
/*     */ 
/* 147 */     xpos %= width;
/* 148 */     ypos %= height;
/*     */ 
/* 150 */     int vx = 0;
/* 151 */     int vy = 0;
/*     */ 
/* 153 */     int target = width * height * percentG / 100;
/* 154 */     int done = 0;
/*     */ 
/* 156 */     int nbiteration = 0;
/*     */ 
/* 158 */     while (done < target) {
/* 159 */       int dir = seed.next() & 0x3;
/* 160 */       switch (dir) { case 0:
/* 161 */         vx = 0; vy = -1; break;
/*     */       case 1:
/* 162 */         vx = 1; vy = 0; break;
/*     */       case 2:
/* 163 */         vx = 0; vy = 1; break;
/*     */       case 3:
/* 164 */         vx = -1; vy = 0;
/*     */       }
/*     */ 
/* 167 */       xpos += vx;
/* 168 */       ypos += vy;
/*     */ 
/* 170 */       xpos = (xpos + width) % width;
/* 171 */       ypos = (ypos + height) % height;
/*     */ 
/* 173 */       if (res.get(xpos, ypos) == 0L) {
/* 174 */         done++;
/* 175 */         res.set(xpos, ypos, 1L);
/*     */       }
/*     */ 
/* 178 */       nbiteration++;
/*     */ 
/* 180 */       if (nbiteration > 10000000) throw new RuntimeException("Big number of iteration in randomwalk generation");
/*     */     }
/*     */   }
/*     */ 
/*     */   public static abstract interface BitIterator
/*     */   {
/*     */     public abstract void at(int paramInt1, int paramInt2);
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     bitmapmanip.BitMapTools
 * JD-Core Version:    0.6.0
 */