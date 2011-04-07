/*      */ package backboneImplementation;
/*      */ 
/*      */ import I;
/*      */ import java.io.DataInputStream;
/*      */ import java.io.DataOutputStream;
/*      */ import java.io.IOException;
/*      */ import java.io.PrintStream;
/*      */ import java.io.Serializable;
/*      */ import java.util.Random;
/*      */ 
/*      */ public class MersenneTwisterFast
/*      */   implements Serializable, Cloneable
/*      */ {
/*      */   private static final int N = 624;
/*      */   private static final int M = 397;
/*      */   private static final int MATRIX_A = -1727483681;
/*      */   private static final int UPPER_MASK = -2147483648;
/*      */   private static final int LOWER_MASK = 2147483647;
/*      */   private static final int TEMPERING_MASK_B = -1658038656;
/*      */   private static final int TEMPERING_MASK_C = -272236544;
/*      */   private int[] mt;
/*      */   private int mti;
/*      */   private int[] mag01;
/*      */   private double __nextNextGaussian;
/*      */   private boolean __haveNextNextGaussian;
/*      */ 
/*      */   public Object clone()
/*      */     throws CloneNotSupportedException
/*      */   {
/*  179 */     MersenneTwisterFast f = (MersenneTwisterFast)(MersenneTwisterFast)super.clone();
/*  180 */     f.mt = ((int[])(int[])this.mt.clone());
/*  181 */     f.mag01 = ((int[])(int[])this.mag01.clone());
/*  182 */     return f;
/*      */   }
/*      */ 
/*      */   public boolean stateEquals(Object o)
/*      */   {
/*  187 */     if (o == this) return true;
/*  188 */     if ((o == null) || (!(o instanceof MersenneTwisterFast)))
/*  189 */       return false;
/*  190 */     MersenneTwisterFast other = (MersenneTwisterFast)o;
/*  191 */     if (this.mti != other.mti) return false;
/*  192 */     for (int x = 0; x < this.mag01.length; x++)
/*  193 */       if (this.mag01[x] != other.mag01[x]) return false;
/*  194 */     for (int x = 0; x < this.mt.length; x++)
/*  195 */       if (this.mt[x] != other.mt[x]) return false;
/*  196 */     return true;
/*      */   }
/*      */ 
/*      */   public void readState(DataInputStream stream)
/*      */     throws IOException
/*      */   {
/*  202 */     int len = this.mt.length;
/*  203 */     for (int x = 0; x < len; x++) this.mt[x] = stream.readInt();
/*      */ 
/*  205 */     len = this.mag01.length;
/*  206 */     for (int x = 0; x < len; x++) this.mag01[x] = stream.readInt();
/*      */ 
/*  208 */     this.mti = stream.readInt();
/*  209 */     this.__nextNextGaussian = stream.readDouble();
/*  210 */     this.__haveNextNextGaussian = stream.readBoolean();
/*      */   }
/*      */ 
/*      */   public void writeState(DataOutputStream stream)
/*      */     throws IOException
/*      */   {
/*  216 */     int len = this.mt.length;
/*  217 */     for (int x = 0; x < len; x++) stream.writeInt(this.mt[x]);
/*      */ 
/*  219 */     len = this.mag01.length;
/*  220 */     for (int x = 0; x < len; x++) stream.writeInt(this.mag01[x]);
/*      */ 
/*  222 */     stream.writeInt(this.mti);
/*  223 */     stream.writeDouble(this.__nextNextGaussian);
/*  224 */     stream.writeBoolean(this.__haveNextNextGaussian);
/*      */   }
/*      */ 
/*      */   public MersenneTwisterFast()
/*      */   {
/*  232 */     this(System.currentTimeMillis());
/*      */   }
/*      */ 
/*      */   public MersenneTwisterFast(long seed)
/*      */   {
/*  242 */     setSeed(seed);
/*      */   }
/*      */ 
/*      */   public MersenneTwisterFast(int[] array)
/*      */   {
/*  254 */     setSeed(array);
/*      */   }
/*      */ 
/*      */   public synchronized void setSeed(long seed)
/*      */   {
/*  268 */     this.__haveNextNextGaussian = false;
/*      */ 
/*  270 */     this.mt = new int[624];
/*      */ 
/*  272 */     this.mag01 = new int[2];
/*  273 */     this.mag01[0] = 0;
/*  274 */     this.mag01[1] = -1727483681;
/*      */ 
/*  276 */     this.mt[0] = (int)(seed & 0xFFFFFFFF);
/*  277 */     for (this.mti = 1; this.mti < 624; this.mti += 1)
/*      */     {
/*  279 */       this.mt[this.mti] = (1812433253 * (this.mt[(this.mti - 1)] ^ this.mt[(this.mti - 1)] >>> 30) + this.mti);
/*      */ 
/*  285 */       this.mt[this.mti] &= -1;
/*      */     }
/*      */   }
/*      */ 
/*      */   public synchronized void setSeed(int[] array)
/*      */   {
/*  300 */     if (array.length == 0) {
/*  301 */       throw new IllegalArgumentException("Array length must be greater than zero");
/*      */     }
/*  303 */     setSeed(19650218L);
/*  304 */     int i = 1; int j = 0;
/*  305 */     int k = 624 > array.length ? 624 : array.length;
/*  306 */     for (; k != 0; k--)
/*      */     {
/*  308 */       this.mt[i] = ((this.mt[i] ^ (this.mt[(i - 1)] ^ this.mt[(i - 1)] >>> 30) * 1664525) + array[j] + j);
/*  309 */       this.mt[i] &= -1;
/*  310 */       i++;
/*  311 */       j++;
/*  312 */       if (i >= 624) { this.mt[0] = this.mt[623]; i = 1; }
/*  313 */       if (j < array.length) continue; j = 0;
/*      */     }
/*  315 */     for (k = 623; k != 0; k--)
/*      */     {
/*  317 */       this.mt[i] = ((this.mt[i] ^ (this.mt[(i - 1)] ^ this.mt[(i - 1)] >>> 30) * 1566083941) - i);
/*  318 */       this.mt[i] &= -1;
/*  319 */       i++;
/*  320 */       if (i < 624)
/*      */         continue;
/*  322 */       this.mt[0] = this.mt[623]; i = 1;
/*      */     }
/*      */ 
/*  325 */     this.mt[0] = -2147483648;
/*      */   }
/*      */ 
/*      */   public final int nextInt()
/*      */   {
/*  333 */     if (this.mti >= 624)
/*      */     {
/*  336 */       int[] mt = this.mt;
/*  337 */       int[] mag01 = this.mag01;
/*      */ 
/*  339 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  341 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  342 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  344 */       for (; kk < 623; kk++)
/*      */       {
/*  346 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  347 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  349 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  350 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  352 */       this.mti = 0;
/*      */     }
/*      */ 
/*  355 */     int y = this.mt[(this.mti++)];
/*  356 */     y ^= y >>> 11;
/*  357 */     y ^= y << 7 & 0x9D2C5680;
/*  358 */     y ^= y << 15 & 0xEFC60000;
/*  359 */     y ^= y >>> 18;
/*      */ 
/*  361 */     return y;
/*      */   }
/*      */ 
/*      */   public final short nextShort()
/*      */   {
/*  370 */     if (this.mti >= 624)
/*      */     {
/*  373 */       int[] mt = this.mt;
/*  374 */       int[] mag01 = this.mag01;
/*      */ 
/*  376 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  378 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  379 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  381 */       for (; kk < 623; kk++)
/*      */       {
/*  383 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  384 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  386 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  387 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  389 */       this.mti = 0;
/*      */     }
/*      */ 
/*  392 */     int y = this.mt[(this.mti++)];
/*  393 */     y ^= y >>> 11;
/*  394 */     y ^= y << 7 & 0x9D2C5680;
/*  395 */     y ^= y << 15 & 0xEFC60000;
/*  396 */     y ^= y >>> 18;
/*      */ 
/*  398 */     return (short)(y >>> 16);
/*      */   }
/*      */ 
/*      */   public final char nextChar()
/*      */   {
/*  407 */     if (this.mti >= 624)
/*      */     {
/*  410 */       int[] mt = this.mt;
/*  411 */       int[] mag01 = this.mag01;
/*      */ 
/*  413 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  415 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  416 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  418 */       for (; kk < 623; kk++)
/*      */       {
/*  420 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  421 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  423 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  424 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  426 */       this.mti = 0;
/*      */     }
/*      */ 
/*  429 */     int y = this.mt[(this.mti++)];
/*  430 */     y ^= y >>> 11;
/*  431 */     y ^= y << 7 & 0x9D2C5680;
/*  432 */     y ^= y << 15 & 0xEFC60000;
/*  433 */     y ^= y >>> 18;
/*      */ 
/*  435 */     return (char)(y >>> 16);
/*      */   }
/*      */ 
/*      */   public final boolean nextBoolean()
/*      */   {
/*  443 */     if (this.mti >= 624)
/*      */     {
/*  446 */       int[] mt = this.mt;
/*  447 */       int[] mag01 = this.mag01;
/*      */ 
/*  449 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  451 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  452 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  454 */       for (; kk < 623; kk++)
/*      */       {
/*  456 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  457 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  459 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  460 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  462 */       this.mti = 0;
/*      */     }
/*      */ 
/*  465 */     int y = this.mt[(this.mti++)];
/*  466 */     y ^= y >>> 11;
/*  467 */     y ^= y << 7 & 0x9D2C5680;
/*  468 */     y ^= y << 15 & 0xEFC60000;
/*  469 */     y ^= y >>> 18;
/*      */ 
/*  471 */     return y >>> 31 != 0;
/*      */   }
/*      */ 
/*      */   public final boolean nextBoolean(float probability)
/*      */   {
/*  486 */     if ((probability < 0.0F) || (probability > 1.0F))
/*  487 */       throw new IllegalArgumentException("probability must be between 0.0 and 1.0 inclusive.");
/*  488 */     if (probability == 0.0F) return false;
/*  489 */     if (probability == 1.0F) return true;
/*  490 */     if (this.mti >= 624)
/*      */     {
/*  493 */       int[] mt = this.mt;
/*  494 */       int[] mag01 = this.mag01;
/*      */ 
/*  496 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  498 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  499 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  501 */       for (; kk < 623; kk++)
/*      */       {
/*  503 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  504 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  506 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  507 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  509 */       this.mti = 0;
/*      */     }
/*      */ 
/*  512 */     int y = this.mt[(this.mti++)];
/*  513 */     y ^= y >>> 11;
/*  514 */     y ^= y << 7 & 0x9D2C5680;
/*  515 */     y ^= y << 15 & 0xEFC60000;
/*  516 */     y ^= y >>> 18;
/*      */ 
/*  518 */     return (y >>> 8) / 16777216.0F < probability;
/*      */   }
/*      */ 
/*      */   public final boolean nextBoolean(double probability)
/*      */   {
/*  531 */     if ((probability < 0.0D) || (probability > 1.0D))
/*  532 */       throw new IllegalArgumentException("probability must be between 0.0 and 1.0 inclusive.");
/*  533 */     if (probability == 0.0D) return false;
/*  534 */     if (probability == 1.0D) return true;
/*  535 */     if (this.mti >= 624)
/*      */     {
/*  538 */       int[] mt = this.mt;
/*  539 */       int[] mag01 = this.mag01;
/*      */ 
/*  541 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  543 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  544 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  546 */       for (; kk < 623; kk++)
/*      */       {
/*  548 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  549 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  551 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  552 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  554 */       this.mti = 0;
/*      */     }
/*      */ 
/*  557 */     int y = this.mt[(this.mti++)];
/*  558 */     y ^= y >>> 11;
/*  559 */     y ^= y << 7 & 0x9D2C5680;
/*  560 */     y ^= y << 15 & 0xEFC60000;
/*  561 */     y ^= y >>> 18;
/*      */ 
/*  563 */     if (this.mti >= 624)
/*      */     {
/*  566 */       int[] mt = this.mt;
/*  567 */       int[] mag01 = this.mag01;
/*      */ 
/*  569 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  571 */         int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  572 */         mt[kk] = (mt[(kk + 397)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */       }
/*  574 */       for (; kk < 623; kk++)
/*      */       {
/*  576 */         int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  577 */         mt[kk] = (mt[(kk + -227)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */       }
/*  579 */       int z = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  580 */       mt[623] = (mt[396] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */ 
/*  582 */       this.mti = 0;
/*      */     }
/*      */ 
/*  585 */     int z = this.mt[(this.mti++)];
/*  586 */     z ^= z >>> 11;
/*  587 */     z ^= z << 7 & 0x9D2C5680;
/*  588 */     z ^= z << 15 & 0xEFC60000;
/*  589 */     z ^= z >>> 18;
/*      */ 
/*  592 */     return ((y >>> 6 << 27) + (z >>> 5)) / 9007199254740992.0D < probability;
/*      */   }
/*      */ 
/*      */   public final byte nextByte()
/*      */   {
/*  600 */     if (this.mti >= 624)
/*      */     {
/*  603 */       int[] mt = this.mt;
/*  604 */       int[] mag01 = this.mag01;
/*      */ 
/*  606 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  608 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  609 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  611 */       for (; kk < 623; kk++)
/*      */       {
/*  613 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  614 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  616 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  617 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  619 */       this.mti = 0;
/*      */     }
/*      */ 
/*  622 */     int y = this.mt[(this.mti++)];
/*  623 */     y ^= y >>> 11;
/*  624 */     y ^= y << 7 & 0x9D2C5680;
/*  625 */     y ^= y << 15 & 0xEFC60000;
/*  626 */     y ^= y >>> 18;
/*      */ 
/*  628 */     return (byte)(y >>> 24);
/*      */   }
/*      */ 
/*      */   public final void nextBytes(byte[] bytes)
/*      */   {
/*  636 */     for (int x = 0; x < bytes.length; x++)
/*      */     {
/*  638 */       if (this.mti >= 624)
/*      */       {
/*  641 */         int[] mt = this.mt;
/*  642 */         int[] mag01 = this.mag01;
/*      */ 
/*  644 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/*  646 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  647 */           mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/*  649 */         for (; kk < 623; kk++)
/*      */         {
/*  651 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  652 */           mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/*  654 */         int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  655 */         mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  657 */         this.mti = 0;
/*      */       }
/*      */ 
/*  660 */       int y = this.mt[(this.mti++)];
/*  661 */       y ^= y >>> 11;
/*  662 */       y ^= y << 7 & 0x9D2C5680;
/*  663 */       y ^= y << 15 & 0xEFC60000;
/*  664 */       y ^= y >>> 18;
/*      */ 
/*  666 */       bytes[x] = (byte)(y >>> 24);
/*      */     }
/*      */   }
/*      */ 
/*      */   public final long nextLong()
/*      */   {
/*  676 */     if (this.mti >= 624)
/*      */     {
/*  679 */       int[] mt = this.mt;
/*  680 */       int[] mag01 = this.mag01;
/*      */ 
/*  682 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  684 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  685 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  687 */       for (; kk < 623; kk++)
/*      */       {
/*  689 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  690 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  692 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  693 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  695 */       this.mti = 0;
/*      */     }
/*      */ 
/*  698 */     int y = this.mt[(this.mti++)];
/*  699 */     y ^= y >>> 11;
/*  700 */     y ^= y << 7 & 0x9D2C5680;
/*  701 */     y ^= y << 15 & 0xEFC60000;
/*  702 */     y ^= y >>> 18;
/*      */ 
/*  704 */     if (this.mti >= 624)
/*      */     {
/*  707 */       int[] mt = this.mt;
/*  708 */       int[] mag01 = this.mag01;
/*      */ 
/*  710 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  712 */         int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  713 */         mt[kk] = (mt[(kk + 397)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */       }
/*  715 */       for (; kk < 623; kk++)
/*      */       {
/*  717 */         int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  718 */         mt[kk] = (mt[(kk + -227)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */       }
/*  720 */       int z = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  721 */       mt[623] = (mt[396] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */ 
/*  723 */       this.mti = 0;
/*      */     }
/*      */ 
/*  726 */     int z = this.mt[(this.mti++)];
/*  727 */     z ^= z >>> 11;
/*  728 */     z ^= z << 7 & 0x9D2C5680;
/*  729 */     z ^= z << 15 & 0xEFC60000;
/*  730 */     z ^= z >>> 18;
/*      */ 
/*  732 */     return (y << 32) + z;
/*      */   }
/*      */ 
/*      */   public final long nextLong(long n)
/*      */   {
/*  741 */     if (n <= 0L) {
/*  742 */       throw new IllegalArgumentException("n must be > 0");
/*      */     }
/*      */     long bits;
/*      */     long val;
/*      */     do
/*      */     {
/*  750 */       if (this.mti >= 624)
/*      */       {
/*  753 */         int[] mt = this.mt;
/*  754 */         int[] mag01 = this.mag01;
/*      */ 
/*  756 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/*  758 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  759 */           mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/*  761 */         for (; kk < 623; kk++)
/*      */         {
/*  763 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  764 */           mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/*  766 */         int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  767 */         mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  769 */         this.mti = 0;
/*      */       }
/*      */ 
/*  772 */       int y = this.mt[(this.mti++)];
/*  773 */       y ^= y >>> 11;
/*  774 */       y ^= y << 7 & 0x9D2C5680;
/*  775 */       y ^= y << 15 & 0xEFC60000;
/*  776 */       y ^= y >>> 18;
/*      */ 
/*  778 */       if (this.mti >= 624)
/*      */       {
/*  781 */         int[] mt = this.mt;
/*  782 */         int[] mag01 = this.mag01;
/*      */ 
/*  784 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/*  786 */           int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  787 */           mt[kk] = (mt[(kk + 397)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */         }
/*  789 */         for (; kk < 623; kk++)
/*      */         {
/*  791 */           int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  792 */           mt[kk] = (mt[(kk + -227)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */         }
/*  794 */         int z = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  795 */         mt[623] = (mt[396] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */ 
/*  797 */         this.mti = 0;
/*      */       }
/*      */ 
/*  800 */       int z = this.mt[(this.mti++)];
/*  801 */       z ^= z >>> 11;
/*  802 */       z ^= z << 7 & 0x9D2C5680;
/*  803 */       z ^= z << 15 & 0xEFC60000;
/*  804 */       z ^= z >>> 18;
/*      */ 
/*  806 */       bits = (y << 32) + z >>> 1;
/*  807 */       val = bits % n;
/*  808 */     }while (bits - val + (n - 1L) < 0L);
/*  809 */     return val;
/*      */   }
/*      */ 
/*      */   public final double nextDouble()
/*      */   {
/*  819 */     if (this.mti >= 624)
/*      */     {
/*  822 */       int[] mt = this.mt;
/*  823 */       int[] mag01 = this.mag01;
/*      */ 
/*  825 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  827 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  828 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  830 */       for (; kk < 623; kk++)
/*      */       {
/*  832 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  833 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/*  835 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  836 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  838 */       this.mti = 0;
/*      */     }
/*      */ 
/*  841 */     int y = this.mt[(this.mti++)];
/*  842 */     y ^= y >>> 11;
/*  843 */     y ^= y << 7 & 0x9D2C5680;
/*  844 */     y ^= y << 15 & 0xEFC60000;
/*  845 */     y ^= y >>> 18;
/*      */ 
/*  847 */     if (this.mti >= 624)
/*      */     {
/*  850 */       int[] mt = this.mt;
/*  851 */       int[] mag01 = this.mag01;
/*      */ 
/*  853 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/*  855 */         int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  856 */         mt[kk] = (mt[(kk + 397)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */       }
/*  858 */       for (; kk < 623; kk++)
/*      */       {
/*  860 */         int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  861 */         mt[kk] = (mt[(kk + -227)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */       }
/*  863 */       int z = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  864 */       mt[623] = (mt[396] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */ 
/*  866 */       this.mti = 0;
/*      */     }
/*      */ 
/*  869 */     int z = this.mt[(this.mti++)];
/*  870 */     z ^= z >>> 11;
/*  871 */     z ^= z << 7 & 0x9D2C5680;
/*  872 */     z ^= z << 15 & 0xEFC60000;
/*  873 */     z ^= z >>> 18;
/*      */ 
/*  876 */     return ((y >>> 6 << 27) + (z >>> 5)) / 9007199254740992.0D;
/*      */   }
/*      */ 
/*      */   public final double nextGaussian()
/*      */   {
/*  885 */     if (this.__haveNextNextGaussian)
/*      */     {
/*  887 */       this.__haveNextNextGaussian = false;
/*  888 */       return this.__nextNextGaussian;
/*      */     }
/*      */ 
/*      */     double v1;
/*      */     double v2;
/*      */     double s;
/*      */     do
/*      */     {
/*  900 */       if (this.mti >= 624)
/*      */       {
/*  903 */         int[] mt = this.mt;
/*  904 */         int[] mag01 = this.mag01;
/*      */ 
/*  906 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/*  908 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  909 */           mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/*  911 */         for (; kk < 623; kk++)
/*      */         {
/*  913 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  914 */           mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/*  916 */         int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  917 */         mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/*  919 */         this.mti = 0;
/*      */       }
/*      */ 
/*  922 */       int y = this.mt[(this.mti++)];
/*  923 */       y ^= y >>> 11;
/*  924 */       y ^= y << 7 & 0x9D2C5680;
/*  925 */       y ^= y << 15 & 0xEFC60000;
/*  926 */       y ^= y >>> 18;
/*      */ 
/*  928 */       if (this.mti >= 624)
/*      */       {
/*  931 */         int[] mt = this.mt;
/*  932 */         int[] mag01 = this.mag01;
/*      */ 
/*  934 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/*  936 */           int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  937 */           mt[kk] = (mt[(kk + 397)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */         }
/*  939 */         for (; kk < 623; kk++)
/*      */         {
/*  941 */           int z = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  942 */           mt[kk] = (mt[(kk + -227)] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */         }
/*  944 */         int z = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  945 */         mt[623] = (mt[396] ^ z >>> 1 ^ mag01[(z & 0x1)]);
/*      */ 
/*  947 */         this.mti = 0;
/*      */       }
/*      */ 
/*  950 */       int z = this.mt[(this.mti++)];
/*  951 */       z ^= z >>> 11;
/*  952 */       z ^= z << 7 & 0x9D2C5680;
/*  953 */       z ^= z << 15 & 0xEFC60000;
/*  954 */       z ^= z >>> 18;
/*      */ 
/*  956 */       if (this.mti >= 624)
/*      */       {
/*  959 */         int[] mt = this.mt;
/*  960 */         int[] mag01 = this.mag01;
/*      */ 
/*  962 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/*  964 */           int a = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  965 */           mt[kk] = (mt[(kk + 397)] ^ a >>> 1 ^ mag01[(a & 0x1)]);
/*      */         }
/*  967 */         for (; kk < 623; kk++)
/*      */         {
/*  969 */           int a = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  970 */           mt[kk] = (mt[(kk + -227)] ^ a >>> 1 ^ mag01[(a & 0x1)]);
/*      */         }
/*  972 */         int a = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/*  973 */         mt[623] = (mt[396] ^ a >>> 1 ^ mag01[(a & 0x1)]);
/*      */ 
/*  975 */         this.mti = 0;
/*      */       }
/*      */ 
/*  978 */       int a = this.mt[(this.mti++)];
/*  979 */       a ^= a >>> 11;
/*  980 */       a ^= a << 7 & 0x9D2C5680;
/*  981 */       a ^= a << 15 & 0xEFC60000;
/*  982 */       a ^= a >>> 18;
/*      */ 
/*  984 */       if (this.mti >= 624)
/*      */       {
/*  987 */         int[] mt = this.mt;
/*  988 */         int[] mag01 = this.mag01;
/*      */ 
/*  990 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/*  992 */           int b = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  993 */           mt[kk] = (mt[(kk + 397)] ^ b >>> 1 ^ mag01[(b & 0x1)]);
/*      */         }
/*  995 */         for (; kk < 623; kk++)
/*      */         {
/*  997 */           int b = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/*  998 */           mt[kk] = (mt[(kk + -227)] ^ b >>> 1 ^ mag01[(b & 0x1)]);
/*      */         }
/* 1000 */         int b = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/* 1001 */         mt[623] = (mt[396] ^ b >>> 1 ^ mag01[(b & 0x1)]);
/*      */ 
/* 1003 */         this.mti = 0;
/*      */       }
/*      */ 
/* 1006 */       int b = this.mt[(this.mti++)];
/* 1007 */       b ^= b >>> 11;
/* 1008 */       b ^= b << 7 & 0x9D2C5680;
/* 1009 */       b ^= b << 15 & 0xEFC60000;
/* 1010 */       b ^= b >>> 18;
/*      */ 
/* 1013 */       v1 = 2.0D * (((y >>> 6 << 27) + (z >>> 5)) / 9007199254740992.0D) - 1.0D;
/*      */ 
/* 1016 */       v2 = 2.0D * (((a >>> 6 << 27) + (b >>> 5)) / 9007199254740992.0D) - 1.0D;
/*      */ 
/* 1018 */       s = v1 * v1 + v2 * v2;
/* 1019 */     }while ((s >= 1.0D) || (s == 0.0D));
/* 1020 */     double multiplier = Math.sqrt(-2.0D * Math.log(s) / s);
/* 1021 */     this.__nextNextGaussian = (v2 * multiplier);
/* 1022 */     this.__haveNextNextGaussian = true;
/* 1023 */     return v1 * multiplier;
/*      */   }
/*      */ 
/*      */   public final float nextFloat()
/*      */   {
/* 1037 */     if (this.mti >= 624)
/*      */     {
/* 1040 */       int[] mt = this.mt;
/* 1041 */       int[] mag01 = this.mag01;
/*      */ 
/* 1043 */       for (int kk = 0; kk < 227; kk++)
/*      */       {
/* 1045 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/* 1046 */         mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/* 1048 */       for (; kk < 623; kk++)
/*      */       {
/* 1050 */         int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/* 1051 */         mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */       }
/* 1053 */       int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/* 1054 */       mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/* 1056 */       this.mti = 0;
/*      */     }
/*      */ 
/* 1059 */     int y = this.mt[(this.mti++)];
/* 1060 */     y ^= y >>> 11;
/* 1061 */     y ^= y << 7 & 0x9D2C5680;
/* 1062 */     y ^= y << 15 & 0xEFC60000;
/* 1063 */     y ^= y >>> 18;
/*      */ 
/* 1065 */     return (y >>> 8) / 16777216.0F;
/*      */   }
/*      */ 
/*      */   public final int nextInt(int n)
/*      */   {
/* 1074 */     if (n <= 0) {
/* 1075 */       throw new IllegalArgumentException("n must be > 0");
/*      */     }
/* 1077 */     if ((n & -n) == n)
/*      */     {
/* 1081 */       if (this.mti >= 624)
/*      */       {
/* 1084 */         int[] mt = this.mt;
/* 1085 */         int[] mag01 = this.mag01;
/*      */ 
/* 1087 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/* 1089 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/* 1090 */           mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/* 1092 */         for (; kk < 623; kk++)
/*      */         {
/* 1094 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/* 1095 */           mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/* 1097 */         int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/* 1098 */         mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/* 1100 */         this.mti = 0;
/*      */       }
/*      */ 
/* 1103 */       int y = this.mt[(this.mti++)];
/* 1104 */       y ^= y >>> 11;
/* 1105 */       y ^= y << 7 & 0x9D2C5680;
/* 1106 */       y ^= y << 15 & 0xEFC60000;
/* 1107 */       y ^= y >>> 18;
/*      */ 
/* 1109 */       return (int)(n * (y >>> 1) >> 31);
/*      */     }
/*      */     int bits;
/*      */     int val;
/*      */     do
/*      */     {
/* 1117 */       if (this.mti >= 624)
/*      */       {
/* 1120 */         int[] mt = this.mt;
/* 1121 */         int[] mag01 = this.mag01;
/*      */ 
/* 1123 */         for (int kk = 0; kk < 227; kk++)
/*      */         {
/* 1125 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/* 1126 */           mt[kk] = (mt[(kk + 397)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/* 1128 */         for (; kk < 623; kk++)
/*      */         {
/* 1130 */           int y = mt[kk] & 0x80000000 | mt[(kk + 1)] & 0x7FFFFFFF;
/* 1131 */           mt[kk] = (mt[(kk + -227)] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */         }
/* 1133 */         int y = mt[623] & 0x80000000 | mt[0] & 0x7FFFFFFF;
/* 1134 */         mt[623] = (mt[396] ^ y >>> 1 ^ mag01[(y & 0x1)]);
/*      */ 
/* 1136 */         this.mti = 0;
/*      */       }
/*      */ 
/* 1139 */       int y = this.mt[(this.mti++)];
/* 1140 */       y ^= y >>> 11;
/* 1141 */       y ^= y << 7 & 0x9D2C5680;
/* 1142 */       y ^= y << 15 & 0xEFC60000;
/* 1143 */       y ^= y >>> 18;
/*      */ 
/* 1145 */       bits = y >>> 1;
/* 1146 */       val = bits % n;
/* 1147 */     }while (bits - val + (n - 1) < 0);
/* 1148 */     return val;
/*      */   }
/*      */ 
/*      */   public static void main(String[] args)
/*      */   {
/* 1164 */     MersenneTwisterFast r = new MersenneTwisterFast(new int[] { 291, 564, 837, 1110 });
/* 1165 */     System.out.println("Output of MersenneTwisterFast with new (2002/1/26) seeding mechanism");
/* 1166 */     for (int j = 0; j < 1000; j++)
/*      */     {
/* 1169 */       long l = r.nextInt();
/* 1170 */       if (l < 0L) l += 4294967296L;
/* 1171 */       String s = String.valueOf(l);
/* 1172 */       while (s.length() < 10) s = " " + s;
/* 1173 */       System.out.print(s + " ");
/* 1174 */       if (j % 5 != 4) continue; System.out.println();
/*      */     }
/*      */ 
/* 1179 */     long SEED = 4357L;
/*      */ 
/* 1182 */     System.out.println("\nTime to test grabbing 100000000 ints");
/*      */ 
/* 1184 */     Random rr = new Random(4357L);
/* 1185 */     int xx = 0;
/* 1186 */     long ms = System.currentTimeMillis();
/* 1187 */     for (j = 0; j < 100000000; j++)
/* 1188 */       xx += rr.nextInt();
/* 1189 */     System.out.println("java.util.Random: " + (System.currentTimeMillis() - ms) + "          Ignore this: " + xx);
/*      */ 
/* 1191 */     r = new MersenneTwisterFast(4357L);
/* 1192 */     ms = System.currentTimeMillis();
/* 1193 */     xx = 0;
/* 1194 */     for (j = 0; j < 100000000; j++)
/* 1195 */       xx += r.nextInt();
/* 1196 */     System.out.println("Mersenne Twister Fast: " + (System.currentTimeMillis() - ms) + "          Ignore this: " + xx);
/*      */ 
/* 1201 */     System.out.println("\nGrab the first 1000 booleans");
/* 1202 */     r = new MersenneTwisterFast(4357L);
/* 1203 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1205 */       System.out.print(r.nextBoolean() + " ");
/* 1206 */       if (j % 8 != 7) continue; System.out.println();
/*      */     }
/* 1208 */     if (j % 8 != 7) System.out.println();
/*      */ 
/* 1210 */     System.out.println("\nGrab 1000 booleans of increasing probability using nextBoolean(double)");
/* 1211 */     r = new MersenneTwisterFast(4357L);
/* 1212 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1214 */       System.out.print(r.nextBoolean(j / 999.0D) + " ");
/* 1215 */       if (j % 8 != 7) continue; System.out.println();
/*      */     }
/* 1217 */     if (j % 8 != 7) System.out.println();
/*      */ 
/* 1219 */     System.out.println("\nGrab 1000 booleans of increasing probability using nextBoolean(float)");
/* 1220 */     r = new MersenneTwisterFast(4357L);
/* 1221 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1223 */       System.out.print(r.nextBoolean(j / 999.0F) + " ");
/* 1224 */       if (j % 8 != 7) continue; System.out.println();
/*      */     }
/* 1226 */     if (j % 8 != 7) System.out.println();
/*      */ 
/* 1228 */     byte[] bytes = new byte[1000];
/* 1229 */     System.out.println("\nGrab the first 1000 bytes using nextBytes");
/* 1230 */     r = new MersenneTwisterFast(4357L);
/* 1231 */     r.nextBytes(bytes);
/* 1232 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1234 */       System.out.print(bytes[j] + " ");
/* 1235 */       if (j % 16 != 15) continue; System.out.println();
/*      */     }
/* 1237 */     if (j % 16 != 15) System.out.println();
/*      */ 
/* 1240 */     System.out.println("\nGrab the first 1000 bytes -- must be same as nextBytes");
/* 1241 */     r = new MersenneTwisterFast(4357L);
/* 1242 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1245 */       byte b;
/* 1244 */       System.out.print((b = r.nextByte()) + " ");
/* 1245 */       if (b != bytes[j]) System.out.print("BAD ");
/* 1246 */       if (j % 16 != 15) continue; System.out.println();
/*      */     }
/* 1248 */     if (j % 16 != 15) System.out.println();
/*      */ 
/* 1250 */     System.out.println("\nGrab the first 1000 shorts");
/* 1251 */     r = new MersenneTwisterFast(4357L);
/* 1252 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1254 */       System.out.print(r.nextShort() + " ");
/* 1255 */       if (j % 8 != 7) continue; System.out.println();
/*      */     }
/* 1257 */     if (j % 8 != 7) System.out.println();
/*      */ 
/* 1259 */     System.out.println("\nGrab the first 1000 ints");
/* 1260 */     r = new MersenneTwisterFast(4357L);
/* 1261 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1263 */       System.out.print(r.nextInt() + " ");
/* 1264 */       if (j % 4 != 3) continue; System.out.println();
/*      */     }
/* 1266 */     if (j % 4 != 3) System.out.println();
/*      */ 
/* 1268 */     System.out.println("\nGrab the first 1000 ints of different sizes");
/* 1269 */     r = new MersenneTwisterFast(4357L);
/* 1270 */     int max = 1;
/* 1271 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1273 */       System.out.print(r.nextInt(max) + " ");
/* 1274 */       max *= 2;
/* 1275 */       if (max <= 0) max = 1;
/* 1276 */       if (j % 4 != 3) continue; System.out.println();
/*      */     }
/* 1278 */     if (j % 4 != 3) System.out.println();
/*      */ 
/* 1280 */     System.out.println("\nGrab the first 1000 longs");
/* 1281 */     r = new MersenneTwisterFast(4357L);
/* 1282 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1284 */       System.out.print(r.nextLong() + " ");
/* 1285 */       if (j % 3 != 2) continue; System.out.println();
/*      */     }
/* 1287 */     if (j % 3 != 2) System.out.println();
/*      */ 
/* 1289 */     System.out.println("\nGrab the first 1000 longs of different sizes");
/* 1290 */     r = new MersenneTwisterFast(4357L);
/* 1291 */     long max2 = 1L;
/* 1292 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1294 */       System.out.print(r.nextLong(max2) + " ");
/* 1295 */       max2 *= 2L;
/* 1296 */       if (max2 <= 0L) max2 = 1L;
/* 1297 */       if (j % 4 != 3) continue; System.out.println();
/*      */     }
/* 1299 */     if (j % 4 != 3) System.out.println();
/*      */ 
/* 1301 */     System.out.println("\nGrab the first 1000 floats");
/* 1302 */     r = new MersenneTwisterFast(4357L);
/* 1303 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1305 */       System.out.print(r.nextFloat() + " ");
/* 1306 */       if (j % 4 != 3) continue; System.out.println();
/*      */     }
/* 1308 */     if (j % 4 != 3) System.out.println();
/*      */ 
/* 1310 */     System.out.println("\nGrab the first 1000 doubles");
/* 1311 */     r = new MersenneTwisterFast(4357L);
/* 1312 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1314 */       System.out.print(r.nextDouble() + " ");
/* 1315 */       if (j % 3 != 2) continue; System.out.println();
/*      */     }
/* 1317 */     if (j % 3 != 2) System.out.println();
/*      */ 
/* 1319 */     System.out.println("\nGrab the first 1000 gaussian doubles");
/* 1320 */     r = new MersenneTwisterFast(4357L);
/* 1321 */     for (j = 0; j < 1000; j++)
/*      */     {
/* 1323 */       System.out.print(r.nextGaussian() + " ");
/* 1324 */       if (j % 3 != 2) continue; System.out.println();
/*      */     }
/* 1326 */     if (j % 3 != 2) System.out.println();
/*      */   }
/*      */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     backboneImplementation.MersenneTwisterFast
 * JD-Core Version:    0.6.0
 */