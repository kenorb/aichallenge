/*    */ package contest;
/*    */ 
/*    */ import java.util.EnumSet;
/*    */ import java.util.HashMap;
/*    */ import java.util.Map;
/*    */ 
/*    */ public enum Ilk
/*    */ {
/*  8 */   UNSEEN(-5, '?'), 
/*  9 */   WATER(-4, '%'), 
/* 10 */   FOOD(-3, '*'), 
/* 11 */   LAND(-2, '.'), 
/* 12 */   DEAD(-1, '!'), 
/* 13 */   MY_ANT(0, 'A'), 
/* 14 */   PLAYER1(1, 'B'), 
/* 15 */   PLAYER2(2, 'C'), 
/* 16 */   PLAYER3(3, 'D'), 
/* 17 */   PLAYER4(4, 'E'), 
/* 18 */   PLAYER5(5, 'F'), 
/* 19 */   PLAYER6(6, 'G'), 
/* 20 */   PLAYER7(7, 'H'), 
/* 21 */   PLAYER8(8, 'I'), 
/* 22 */   PLAYER9(9, 'J'), 
/* 23 */   PLAYER10(10, 'K'), 
/* 24 */   PLAYER11(11, 'L'), 
/* 25 */   PLAYER12(12, 'M'), 
/* 26 */   PLAYER13(13, 'N'), 
/* 27 */   PLAYER14(14, 'O'), 
/* 28 */   PLAYER15(15, 'P'), 
/* 29 */   PLAYER16(16, 'Q'), 
/* 30 */   PLAYER17(17, 'R'), 
/* 31 */   PLAYER18(18, 'S'), 
/* 32 */   PLAYER19(19, 'T'), 
/* 33 */   PLAYER20(20, 'U'), 
/* 34 */   PLAYER21(21, 'V'), 
/* 35 */   PLAYER22(22, 'W'), 
/* 36 */   PLAYER23(23, 'X'), 
/* 37 */   PLAYER24(24, 'Y'), 
/* 38 */   PLAYER25(25, 'Z');
/*    */ 
/*    */   private static final Map<Integer, Ilk> idLookup;
/*    */   private static final Map<Character, Ilk> symbolLookup;
/*    */   public final int id;
/*    */   public final char symbol;
/*    */ 
/*    */   private Ilk(int id, char symbol)
/*    */   {
/* 54 */     this.id = id;
/* 55 */     this.symbol = symbol;
/*    */   }
/*    */ 
/*    */   public boolean isAnt() {
/* 59 */     return this.id >= MY_ANT.id;
/*    */   }
/*    */ 
/*    */   public boolean isEnemy() {
/* 63 */     return this.id > MY_ANT.id;
/*    */   }
/*    */ 
/*    */   public boolean isPassable() {
/* 67 */     return this.id > WATER.id;
/*    */   }
/*    */ 
/*    */   public boolean isUnoccupied() {
/* 71 */     return (this.id == LAND.id) || (this.id == DEAD.id);
/*    */   }
/*    */ 
/*    */   public boolean isEnemyOf(Ilk ant) {
/* 75 */     return (this.id >= MY_ANT.id) && (this.id != ant.id);
/*    */   }
/*    */ 
/*    */   public static Ilk fromId(int id) {
/* 79 */     return (Ilk)idLookup.get(Integer.valueOf(id));
/*    */   }
/*    */ 
/*    */   public static Ilk fromSymbol(char symbol) {
/* 83 */     return (Ilk)symbolLookup.get(Character.valueOf(symbol));
/*    */   }
/*    */ 
/*    */   static
/*    */   {
/* 40 */     idLookup = new HashMap();
/* 41 */     symbolLookup = new HashMap();
/*    */ 
/* 44 */     for (Ilk i : EnumSet.allOf(Ilk.class)) {
/* 45 */       idLookup.put(Integer.valueOf(i.id), i);
/* 46 */       symbolLookup.put(Character.valueOf(i.symbol), i);
/*    */     }
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.Ilk
 * JD-Core Version:    0.6.0
 */