/*    */ package contest;
/*    */ 
/*    */ public class Tile
/*    */ {
/*    */   private int row;
/*    */   private int col;
/*    */ 
/*    */   Tile(int row, int col)
/*    */   {
/*  5 */     this.row = row;
/*  6 */     this.col = col;
/*    */   }
/*    */ 
/*    */   public int row()
/*    */   {
/* 13 */     return this.row;
/*    */   }
/*    */ 
/*    */   public int col() {
/* 17 */     return this.col;
/*    */   }
/*    */ 
/*    */   public int hashCode() {
/* 21 */     return this.row * 65536 + this.col;
/*    */   }
/*    */ 
/*    */   public boolean equals(Object o) {
/* 25 */     if (o.getClass() == Tile.class) {
/* 26 */       return (this.row == ((Tile)o).row()) && (this.col == ((Tile)o).col());
/*    */     }
/* 28 */     return false;
/*    */   }
/*    */ 
/*    */   public String toString()
/*    */   {
/* 33 */     return "(" + this.row + "," + this.col + ")";
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.Tile
 * JD-Core Version:    0.6.0
 */