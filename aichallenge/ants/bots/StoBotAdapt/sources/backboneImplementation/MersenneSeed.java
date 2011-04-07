/*    */ package backboneImplementation;
/*    */ 
/*    */ import backboneInterface.SeedInterface;
/*    */ 
/*    */ public class MersenneSeed
/*    */   implements SeedInterface
/*    */ {
/*    */   final MersenneTwisterFast twist;
/*    */ 
/*    */   public MersenneSeed(long seed)
/*    */   {
/* 19 */     this.twist = new MersenneTwisterFast(seed);
/*    */   }
/*    */ 
/*    */   public int next() {
/* 23 */     return this.twist.nextInt();
/*    */   }
/*    */ 
/*    */   public long nextL() {
/* 27 */     return this.twist.nextLong();
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     backboneImplementation.MersenneSeed
 * JD-Core Version:    0.6.0
 */