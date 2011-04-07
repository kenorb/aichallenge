/*     */ package bitmapmanip;
/*     */ 
/*     */ public class BitMap
/*     */ {
/*     */   public final int x;
/*     */   public final int y;
/*     */   public final int xnbmap;
/*     */   public final long[][] maps;
/*     */   public final long borderMask;
/*     */   public final int decsup;
/*     */ 
/*     */   public BitMap(BitMap model)
/*     */   {
/*  25 */     this(model.x, model.y);
/*     */   }
/*     */ 
/*     */   public boolean isZero() {
/*  29 */     long bits = 0L;
/*     */ 
/*  31 */     for (int i = 0; i < this.y; i++) {
/*  32 */       for (int k = 0; k < this.xnbmap; k++) {
/*  33 */         bits |= this.maps[i][k];
/*     */       }
/*     */     }
/*     */ 
/*  37 */     return bits == 0L;
/*     */   }
/*     */ 
/*     */   public boolean nearestUpTo(BitMap dest, BitMap d0, BitMap d1, BitMap d2, BitMap d3, BitMap path, BitMap buffsave, BitMap buffAccumul, BitMap buffShift)
/*     */   {
/*  53 */     d0.xor(d0);
/*  54 */     d1.xor(d1);
/*  55 */     d2.xor(d2);
/*  56 */     d3.xor(d3);
/*     */ 
/*  64 */     buffsave.set(0, 0, 1L);
/*  65 */     buffAccumul.copy(this);
/*  66 */     boolean found = false;
/*     */ 
/*  68 */     int secure = dest.x + dest.y << 1;
/*  69 */     while ((!buffsave.isZero()) && (!found) && (secure > 0)) {
/*  70 */       secure--;
/*  71 */       buffsave.copy(this);
/*     */ 
/*  73 */       d0.copy(this);
/*  74 */       d0.rotateUp(1, buffShift);
/*  75 */       buffAccumul.or(d0);
/*  76 */       d0.and(dest);
/*     */ 
/*  78 */       d2.copy(this);
/*  79 */       d2.rotateDown(1, buffShift);
/*  80 */       buffAccumul.or(d2);
/*  81 */       d2.and(dest);
/*     */ 
/*  83 */       d3.copy(this);
/*  84 */       d3.rotateLeft(1);
/*  85 */       buffAccumul.or(d3);
/*  86 */       d3.and(dest);
/*     */ 
/*  88 */       d1.copy(this);
/*  89 */       d1.rotateRight(1);
/*  90 */       buffAccumul.or(d1);
/*  91 */       d1.and(dest);
/*     */ 
/*  93 */       copy(buffAccumul);
/*  94 */       buffAccumul.and(dest);
/*  95 */       and(path);
/*  96 */       buffsave.xor(this);
/*  97 */       found = !buffAccumul.isZero();
/*     */     }
/*     */ 
/* 101 */     return found;
/*     */   }
/*     */ 
/*     */   public void intersectUpTo(BitMap dest, BitMap bmff01, BitMap bmff02, BitMap bmff03)
/*     */   {
/* 106 */     scramble(bmff01, bmff02, bmff03, 1);
/* 107 */     and(dest);
/*     */   }
/*     */ 
/*     */   public void scramble(BitMap buff1, BitMap buff2, BitMap buff3, int l) {
/* 111 */     buff1.copy(this);
/* 112 */     buff1.rotateDown(l, buff2);
/* 113 */     buff3.copy(buff1);
/*     */ 
/* 115 */     buff1.copy(this);
/* 116 */     buff1.rotateUp(l, buff2);
/* 117 */     buff3.or(buff1);
/*     */ 
/* 119 */     buff1.copy(this);
/* 120 */     buff1.rotateLeft(l);
/* 121 */     buff3.or(buff1);
/*     */ 
/* 123 */     buff1.copy(this);
/* 124 */     buff1.rotateRight(l);
/* 125 */     buff3.or(buff1);
/*     */ 
/* 127 */     or(buff3);
/*     */   }
/*     */ 
/*     */   public BitMap(int x, int y) {
/* 131 */     this.x = x;
/* 132 */     this.y = y;
/*     */ 
/* 134 */     int nbmaps = x >>> 6;
/* 135 */     int sup = x & 0x3F;
/* 136 */     if (sup > 0) nbmaps++;
/*     */ 
/* 138 */     this.borderMask = (-1L >>> 64 - sup);
/* 139 */     this.decsup = sup;
/* 140 */     this.xnbmap = nbmaps;
/*     */ 
/* 142 */     this.maps = new long[y][];
/*     */ 
/* 144 */     for (int i = 0; i < y; i++)
/* 145 */       this.maps[i] = new long[this.xnbmap];
/*     */   }
/*     */ 
/*     */   public void shiftLeft(int nb)
/*     */   {
/* 150 */     nb &= 63;
/* 151 */     if (nb == 0) return;
/* 152 */     for (int i = 0; i < this.y; i++) {
/* 153 */       for (int k = 0; k < this.xnbmap - 1; k++) {
/* 154 */         this.maps[i][k] >>>= nb;
/* 155 */         this.maps[i][k] |= this.maps[i][(k + 1)] << 64 - nb;
/*     */       }
/* 157 */       this.maps[i][(this.xnbmap - 1)] >>>= nb;
/* 158 */       this.maps[i][(this.xnbmap - 1)] &= this.borderMask;
/*     */     }
/*     */   }
/*     */ 
/*     */   public void shiftRight(int nb)
/*     */   {
/* 164 */     nb &= 63;
/* 165 */     if (nb == 0) return;
/* 166 */     for (int i = 0; i < this.y; i++) {
/* 167 */       for (int k = this.xnbmap - 1; k > 0; k--) {
/* 168 */         this.maps[i][k] <<= nb;
/* 169 */         this.maps[i][k] |= this.maps[i][(k - 1)] >>> 64 - nb;
/*     */       }
/* 171 */       this.maps[i][0] <<= nb;
/* 172 */       this.maps[i][(this.xnbmap - 1)] &= this.borderMask;
/*     */     }
/*     */   }
/*     */ 
/*     */   public void rotateLeft(int nb) {
/* 177 */     nb &= 63;
/* 178 */     if (nb == 0) return;
/* 179 */     for (int i = 0; i < this.y; i++) {
/* 180 */       long buff = this.maps[i][0];
/* 181 */       for (int k = 0; k < this.xnbmap - 1; k++) {
/* 182 */         this.maps[i][k] >>>= nb;
/* 183 */         this.maps[i][k] |= this.maps[i][(k + 1)] << 64 - nb;
/*     */       }
/* 185 */       this.maps[i][(this.xnbmap - 1)] >>>= nb;
/* 186 */       this.maps[i][(this.xnbmap - 1)] |= buff << 64 - nb + this.decsup;
/* 187 */       this.maps[i][(this.xnbmap - 1)] &= this.borderMask;
/*     */     }
/*     */   }
/*     */ 
/*     */   public void rotateRight(int nb) {
/* 192 */     nb &= 63;
/* 193 */     if (nb == 0) return;
/* 194 */     for (int i = 0; i < this.y; i++) {
/* 195 */       long buff = this.maps[i][(this.xnbmap - 1)];
/* 196 */       for (int k = this.xnbmap - 1; k > 0; k--) {
/* 197 */         this.maps[i][k] <<= nb;
/* 198 */         this.maps[i][k] |= this.maps[i][(k - 1)] >>> 64 - nb;
/*     */       }
/* 200 */       this.maps[i][0] <<= nb;
/* 201 */       this.maps[i][0] |= buff >>> 64 - nb + this.decsup;
/*     */ 
/* 203 */       this.maps[i][(this.xnbmap - 1)] &= this.borderMask;
/*     */     }
/*     */   }
/*     */ 
/*     */   public void rotateUp(int nb, BitMap buff) {
/* 208 */     for (int i = this.y - nb; i < this.y; i++) {
/* 209 */       for (int k = 0; k < this.xnbmap; k++) {
/* 210 */         buff.maps[i][k] = this.maps[(i - this.y + nb)][k];
/*     */       }
/*     */     }
/* 213 */     for (int i = 0; i < this.y - nb; i++) {
/* 214 */       for (int k = 0; k < this.xnbmap; k++) {
/* 215 */         this.maps[i][k] = this.maps[(i + nb)][k];
/*     */       }
/*     */     }
/* 218 */     for (int i = this.y - nb; i < this.y; i++)
/* 219 */       for (int k = 0; k < this.xnbmap; k++)
/* 220 */         this.maps[i][k] = buff.maps[i][k];
/*     */   }
/*     */ 
/*     */   public void rotateDown(int nb, BitMap buff)
/*     */   {
/* 226 */     for (int i = nb - 1; i >= 0; i--) {
/* 227 */       for (int k = 0; k < this.xnbmap; k++) {
/* 228 */         buff.maps[i][k] = this.maps[(i + this.y - nb)][k];
/*     */       }
/*     */     }
/*     */ 
/* 232 */     for (int i = this.y - 1; i >= nb; i--) {
/* 233 */       for (int k = 0; k < this.xnbmap; k++) {
/* 234 */         this.maps[i][k] = this.maps[(i - nb)][k];
/*     */       }
/*     */     }
/* 237 */     for (int i = nb - 1; i >= 0; i--)
/* 238 */       for (int k = 0; k < this.xnbmap; k++)
/* 239 */         this.maps[i][k] = buff.maps[i][k];
/*     */   }
/*     */ 
/*     */   public void shiftUp(int nb)
/*     */   {
/* 245 */     for (int i = 0; i < this.y - nb; i++) {
/* 246 */       for (int k = 0; k < this.xnbmap; k++) {
/* 247 */         this.maps[i][k] = this.maps[(i + nb)][k];
/*     */       }
/*     */     }
/* 250 */     for (int i = this.y - nb; i < this.y; i++)
/* 251 */       for (int k = 0; k < this.xnbmap; k++)
/* 252 */         this.maps[i][k] = 0L;
/*     */   }
/*     */ 
/*     */   public void shiftDown(int nb)
/*     */   {
/* 258 */     for (int i = this.y - 1; i >= nb; i--) {
/* 259 */       for (int k = 0; k < this.xnbmap; k++) {
/* 260 */         this.maps[i][k] = this.maps[(i - nb)][k];
/*     */       }
/*     */     }
/* 263 */     for (int i = nb - 1; i >= 0; i--)
/* 264 */       for (int k = 0; k < this.xnbmap; k++)
/* 265 */         this.maps[i][k] = 0L;
/*     */   }
/*     */ 
/*     */   public void secureSet(int x, int y, long val)
/*     */   {
/* 271 */     if ((x < 0) || (y < 0) || (x >= this.x) || (y >= this.y)) return;
/*     */ 
/* 273 */     set(x, y, val);
/*     */   }
/*     */ 
/*     */   public void set(int x, int y, long val)
/*     */   {
/* 279 */     long[] row = this.maps[y];
/*     */ 
/* 281 */     int num = x >>> 6;
/* 282 */     int bit = x & 0x3F;
/*     */ 
/* 284 */     long bitval = 1L << bit;
/* 285 */     row[num] &= (bitval ^ 0xFFFFFFFF);
/* 286 */     row[num] |= bitval & val << bit;
/*     */   }
/*     */ 
/*     */   public long get(int x, int y) {
/* 290 */     long[] row = this.maps[y];
/*     */ 
/* 292 */     int num = x >>> 6;
/* 293 */     int bit = x & 0x3F;
/*     */ 
/* 295 */     return row[num] >>> bit & 1L;
/*     */   }
/*     */ 
/*     */   public void and(BitMap m) {
/* 299 */     for (int i = 0; i < this.maps.length; i++) {
/* 300 */       long[] tm = this.maps[i];
/* 301 */       long[] om = m.maps[i];
/*     */ 
/* 303 */       for (int j = 0; j < tm.length; j++)
/* 304 */         tm[j] &= om[j];
/*     */     }
/*     */   }
/*     */ 
/*     */   public void or(BitMap m)
/*     */   {
/* 310 */     for (int i = 0; i < this.maps.length; i++) {
/* 311 */       long[] tm = this.maps[i];
/* 312 */       long[] om = m.maps[i];
/*     */ 
/* 314 */       for (int j = 0; j < tm.length; j++)
/* 315 */         tm[j] |= om[j];
/*     */     }
/*     */   }
/*     */ 
/*     */   public void xor(BitMap m)
/*     */   {
/* 321 */     for (int i = 0; i < this.maps.length; i++) {
/* 322 */       long[] tm = this.maps[i];
/* 323 */       long[] om = m.maps[i];
/*     */ 
/* 325 */       for (int j = 0; j < tm.length; j++)
/* 326 */         tm[j] ^= om[j];
/*     */     }
/*     */   }
/*     */ 
/*     */   public void bitnot()
/*     */   {
/* 333 */     for (int j = 0; j < this.y; j++) {
/* 334 */       for (int n = 0; n < this.xnbmap; n++) {
/* 335 */         this.maps[j][n] ^= -1L;
/*     */       }
/* 337 */       this.maps[j][(this.xnbmap - 1)] &= this.borderMask;
/*     */     }
/*     */   }
/*     */ 
/*     */   public void copy(BitMap m)
/*     */   {
/* 343 */     for (int i = 0; i < this.maps.length; i++) {
/* 344 */       long[] tm = this.maps[i];
/* 345 */       long[] om = m.maps[i];
/*     */ 
/* 347 */       for (int j = 0; j < tm.length; j++)
/* 348 */         tm[j] = om[j];
/*     */     }
/*     */   }
/*     */ 
/*     */   public String toString()
/*     */   {
/* 354 */     StringBuilder sb = new StringBuilder();
/* 355 */     for (int j = 0; j < this.y; j++) {
/* 356 */       for (int i = 0; i < this.x; i++)
/*     */       {
/* 359 */         char c = '.';
/*     */ 
/* 361 */         if (get(i, j) != 0L) c = 'X';
/* 362 */         sb.append(c);
/*     */       }
/* 364 */       sb.append("\n");
/*     */     }
/*     */ 
/* 367 */     return sb.toString();
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     bitmapmanip.BitMap
 * JD-Core Version:    0.6.0
 */