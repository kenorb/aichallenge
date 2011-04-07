/*    */ package contest;
/*    */ 
/*    */ import java.util.EnumMap;
/*    */ import java.util.HashMap;
/*    */ import java.util.Map;
/*    */ 
/*    */ public enum Aim
/*    */ {
/*  9 */   NORTH(0, -1, 'n'), 
/* 10 */   EAST(1, 0, 'e'), 
/* 11 */   SOUTH(0, 1, 's'), 
/* 12 */   WEST(-1, 0, 'w');
/*    */ 
/*    */   private static final Map<Aim, Aim> rightLookup;
/*    */   private static final Map<Aim, Aim> leftLookup;
/*    */   private static final Map<Aim, Aim> behindLookup;
/*    */   private static final Map<Character, Aim> symbolLookup;
/*    */   public final int dCol;
/*    */   public final int dRow;
/*    */   public final char symbol;
/*    */ 
/*    */   private Aim(int dRow, int dCol, char symbol)
/*    */   {
/* 43 */     this.dRow = dRow;
/* 44 */     this.dCol = dCol;
/* 45 */     this.symbol = symbol;
/*    */   }
/*    */ 
/*    */   public Aim left() {
/* 49 */     return (Aim)leftLookup.get(this);
/*    */   }
/*    */ 
/*    */   public Aim right() {
/* 53 */     return (Aim)rightLookup.get(this);
/*    */   }
/*    */ 
/*    */   public Aim behind() {
/* 57 */     return (Aim)behindLookup.get(this);
/*    */   }
/*    */ 
/*    */   public static Aim fromSymbol(char symbol) {
/* 61 */     return (Aim)symbolLookup.get(Character.valueOf(symbol));
/*    */   }
/*    */ 
/*    */   static
/*    */   {
/* 14 */     rightLookup = new EnumMap(Aim.class);
/* 15 */     leftLookup = new EnumMap(Aim.class);
/* 16 */     behindLookup = new EnumMap(Aim.class);
/* 17 */     symbolLookup = new HashMap();
/*    */ 
/* 20 */     rightLookup.put(NORTH, EAST);
/* 21 */     rightLookup.put(EAST, SOUTH);
/* 22 */     rightLookup.put(SOUTH, WEST);
/* 23 */     rightLookup.put(WEST, NORTH);
/* 24 */     leftLookup.put(NORTH, WEST);
/* 25 */     leftLookup.put(WEST, SOUTH);
/* 26 */     leftLookup.put(SOUTH, EAST);
/* 27 */     leftLookup.put(EAST, NORTH);
/* 28 */     behindLookup.put(NORTH, SOUTH);
/* 29 */     behindLookup.put(SOUTH, NORTH);
/* 30 */     behindLookup.put(EAST, WEST);
/* 31 */     behindLookup.put(WEST, EAST);
/* 32 */     symbolLookup.put(Character.valueOf('n'), NORTH);
/* 33 */     symbolLookup.put(Character.valueOf('e'), EAST);
/* 34 */     symbolLookup.put(Character.valueOf('s'), SOUTH);
/* 35 */     symbolLookup.put(Character.valueOf('w'), WEST);
/*    */   }
/*    */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.Aim
 * JD-Core Version:    0.6.0
 */