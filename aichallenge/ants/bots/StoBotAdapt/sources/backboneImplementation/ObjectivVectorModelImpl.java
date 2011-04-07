/*    */ package backboneImplementation;
/*    */ 
/*    */ import backboneInterface.ObjectivVectorModel;
/*    */ import java.util.ArrayList;
/*    */ import java.util.List;
/*    */ 
/*    */ public class ObjectivVectorModelImpl
/*    */   implements ObjectivVectorModel
/*    */ {
/*    */   public final List<Integer> osx;
/*    */   public final List<Integer> osy;
/*    */   public final List<Integer> odx;
/*    */   public final List<Integer> ody;
/*    */   public final List<Integer> ocol;
/* 23 */   public String srcLabel = "";
/* 24 */   public String dstLabel = "";
/*    */ 
/*    */   public ObjectivVectorModelImpl()
/*    */   {
/* 28 */     this.osx = new ArrayList();
/* 29 */     this.osy = new ArrayList();
/* 30 */     this.odx = new ArrayList();
/* 31 */     this.ody = new ArrayList();
/* 32 */     this.ocol = new ArrayList();
/*    */   }
/*    */ 
/*    */   public void clear()
/*    */   {
/* 37 */     this.osx.clear();
/* 38 */     this.osy.clear();
/* 39 */     this.odx.clear();
/* 40 */     this.ody.clear();
/* 41 */     this.ocol.clear();
/*    */   }
/*    */ 
/*    */   public int getNbVectors()
/*    */   {
/* 46 */     return this.osx.size();
/*    */   }
/*    */ 
/*    */   public int getsrcX(int i) {
/* 50 */     return ((Integer)this.osx.get(i)).intValue();
/*    */   }
/*    */ 
/*    */   public int getsrcY(int i) {
/* 54 */     return ((Integer)this.osy.get(i)).intValue();
/*    */   }
/*    */ 
/*    */   public int getdstX(int i) {
/* 58 */     return ((Integer)this.odx.get(i)).intValue();
/*    */   }
/*    */ 
/*    */   public int getdstY(int i) {
/* 62 */     return ((Integer)this.ody.get(i)).intValue();
/*    */   }
/*    */ 
/*    */   public int getVectorColor(int i) {
/* 66 */     return ((Integer)this.ocol.get(i)).intValue();
/*    */   }
/*    */ 
/*    */   public String labelSrc(int i) {
/* 70 */     return this.srcLabel;
/*    */   }
/*    */ 
/*    */   public String labelDst(int i) {
/* 74 */     return this.dstLabel;
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     backboneImplementation.ObjectivVectorModelImpl
 * JD-Core Version:    0.6.0
 */