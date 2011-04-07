/*    */ package botTools;
/*    */ 
/*    */ import backboneInterface.SeedInterface;
/*    */ import bitmapmanip.BitMap;
/*    */ import bitmapmanip.BitMapTools;
/*    */ import bitmapmanip.BitMapTools.BitIterator;
/*    */ import java.util.List;
/*    */ 
/*    */ public class BitMapPlanningGen2
/*    */ {
/*    */   public final BitMap[] currentPlan;
/*    */   final BitMap buff001;
/*    */   final int width;
/*    */   final int height;
/*    */   final SeedInterface rand;
/* 25 */   final int[] dirStack = { 0, 1, 2, 3, 4 };
/* 26 */   int dirSp = 0;
/*    */ 
/*    */   public BitMapPlanningGen2(BitMap model, SeedInterface rand) {
/* 29 */     this.currentPlan = new BitMap[5];
/* 30 */     for (int i = 0; i < 5; i++) {
/* 31 */       this.currentPlan[i] = new BitMap(model);
/*    */     }
/* 33 */     this.buff001 = new BitMap(model);
/*    */ 
/* 35 */     this.width = model.x;
/* 36 */     this.height = model.y;
/* 37 */     this.rand = rand;
/*    */   }
/*    */ 
/*    */   public void newPlanFor(BitMap toPlan) {
/* 41 */     for (int i = 0; i < 4; i++) {
/* 42 */       this.currentPlan[i].xor(this.currentPlan[i]);
/*    */     }
/* 44 */     this.currentPlan[4].copy(toPlan);
/*    */   }
/*    */ 
/*    */   public void spoolPlan(List<Integer> xval, List<Integer> yval, List<Integer> dirval)
/*    */   {
/* 49 */     this.buff001.xor(this.buff001);
/* 50 */     for (int i = 0; i < 5; i++) {
/* 51 */       this.buff001.or(this.currentPlan[i]);
/*    */     }
/*    */ 
/* 54 */     BitMapTools.BitIterator doit = new BitMapTools.BitIterator(xval, yval, dirval) {
/*    */       public void at(int x, int y) {
/* 56 */         boolean orderDone = false;
/* 57 */         BitMapPlanningGen2.this.dirSp = 5;
/* 58 */         while ((!orderDone) && (BitMapPlanningGen2.this.dirSp > 0)) {
/* 59 */           int v = 8;
/* 60 */           while (v >= BitMapPlanningGen2.this.dirSp) {
/* 61 */             v = BitMapPlanningGen2.this.rand.next() & 0x7;
/*    */           }
/* 63 */           int sto = BitMapPlanningGen2.this.dirStack[v];
/* 64 */           BitMapPlanningGen2.this.dirStack[v] = BitMapPlanningGen2.this.dirStack[(--BitMapPlanningGen2.this.dirSp)];
/* 65 */           BitMapPlanningGen2.this.dirStack[BitMapPlanningGen2.this.dirSp] = sto;
/* 66 */           int dir = sto;
/*    */ 
/* 69 */           if (BitMapPlanningGen2.this.currentPlan[dir].get(x, y) != 0L) {
/* 70 */             orderDone = true;
/*    */ 
/* 72 */             if (dir != 4) {
/* 73 */               this.val$xval.add(Integer.valueOf(x));
/* 74 */               this.val$yval.add(Integer.valueOf(y));
/* 75 */               this.val$dirval.add(Integer.valueOf(dir));
/*    */             }
/*    */           }
/*    */         }
/*    */       }
/*    */     };
/* 84 */     BitMapTools.bitIterate(this.buff001, doit);
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     botTools.BitMapPlanningGen2
 * JD-Core Version:    0.6.0
 */