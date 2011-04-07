/*     */ package contest;
/*     */ 
/*     */ import java.io.InputStream;
/*     */ import java.io.PrintStream;
/*     */ import java.util.ArrayList;
/*     */ import java.util.Arrays;
/*     */ import java.util.HashMap;
/*     */ import java.util.HashSet;
/*     */ import java.util.List;
/*     */ import java.util.Map;
/*     */ import java.util.Map.Entry;
/*     */ import java.util.Set;
/*     */ 
/*     */ public class Ants
/*     */ {
/*   8 */   private int turn = 0;
/*   9 */   private int turns = 0;
/*  10 */   private int rows = 0;
/*  11 */   private int cols = 0;
/*  12 */   private int loadtime = 0;
/*  13 */   private int turntime = 0;
/*  14 */   private int viewradius2 = 0;
/*  15 */   private int attackradius2 = 0;
/*  16 */   private int spawnradius2 = 0;
/*     */   private Ilk[][] map;
/*  18 */   public Map<Tile, Ilk> antList = new HashMap();
/*  19 */   public List<Tile> myAnt = new ArrayList();
/*  20 */   public List<Tile> ennemyAnt = new ArrayList();
/*  21 */   private Set<Tile> foodList = new HashSet();
/*  22 */   public List<Tile> deadList = new ArrayList();
/*  23 */   public List<Tile> waterList = new ArrayList();
/*     */ 
/*     */   public int turn() {
/*  26 */     return this.turn;
/*     */   }
/*     */ 
/*     */   public int turns() {
/*  30 */     return this.turns;
/*     */   }
/*     */ 
/*     */   public int rows() {
/*  34 */     return this.rows;
/*     */   }
/*     */ 
/*     */   public int cols() {
/*  38 */     return this.cols;
/*     */   }
/*     */ 
/*     */   public int loadtime() {
/*  42 */     return this.loadtime;
/*     */   }
/*     */ 
/*     */   public int turntime() {
/*  46 */     return this.turntime;
/*     */   }
/*     */ 
/*     */   public int viewradius2() {
/*  50 */     return this.viewradius2;
/*     */   }
/*     */ 
/*     */   public int attackradius2() {
/*  54 */     return this.attackradius2;
/*     */   }
/*     */ 
/*     */   public int spawnradius2() {
/*  58 */     return this.spawnradius2;
/*     */   }
/*     */ 
/*     */   public boolean setup(List<String> data) {
/*     */     try {
/*  63 */       for (String line : data) {
/*  64 */         String[] tokens = line.toLowerCase().split(" ");
/*  65 */         if (tokens[0].equals("cols"))
/*  66 */           this.cols = Integer.parseInt(tokens[1]);
/*  67 */         else if (tokens[0].equals("rows"))
/*  68 */           this.rows = Integer.parseInt(tokens[1]);
/*  69 */         else if (tokens[0].equals("turns"))
/*  70 */           this.turns = Integer.parseInt(tokens[1]);
/*  71 */         else if (tokens[0].equals("loadtime"))
/*  72 */           this.loadtime = Integer.parseInt(tokens[1]);
/*  73 */         else if (tokens[0].equals("turntime"))
/*  74 */           this.turntime = Integer.parseInt(tokens[1]);
/*  75 */         else if (tokens[0].equals("viewradius2"))
/*  76 */           this.viewradius2 = Integer.parseInt(tokens[1]);
/*  77 */         else if (tokens[0].equals("attackradius2"))
/*  78 */           this.attackradius2 = Integer.parseInt(tokens[1]);
/*  79 */         else if (tokens[0].equals("spawnradius2")) {
/*  80 */           this.spawnradius2 = Integer.parseInt(tokens[1]);
/*     */         }
/*     */       }
/*  83 */       this.map = new Ilk[this.rows][this.cols];
/*  84 */       for (Ilk[] row : this.map) {
/*  85 */         Arrays.fill(row, Ilk.LAND);
/*     */       }
/*  87 */       return true; } catch (Exception ex) {
/*     */     }
/*  89 */     return false;
/*     */   }
/*     */ 
/*     */   private boolean update(List<String> data)
/*     */   {
/*  95 */     for (Tile ant : this.antList.keySet()) {
/*  96 */       this.map[ant.row()][ant.col()] = Ilk.LAND;
/*     */     }
/*  98 */     this.antList.clear();
/*  99 */     for (Tile food : this.foodList) {
/* 100 */       this.map[food.row()][food.col()] = Ilk.LAND;
/*     */     }
/* 102 */     this.foodList.clear();
/* 103 */     for (Tile dead : this.deadList) {
/* 104 */       this.map[dead.row()][dead.col()] = Ilk.LAND;
/*     */     }
/* 106 */     this.deadList.clear();
/* 107 */     this.myAnt.clear();
/* 108 */     this.ennemyAnt.clear();
/* 109 */     this.waterList.clear();
/*     */ 
/* 111 */     for (String line : data) {
/* 112 */       String[] tokens = line.split(" ");
/* 113 */       if (tokens.length > 2) {
/* 114 */         int row = Integer.parseInt(tokens[1]);
/* 115 */         int col = Integer.parseInt(tokens[2]);
/* 116 */         if (tokens[0].equals("w")) {
/* 117 */           this.map[row][col] = Ilk.WATER;
/* 118 */           this.waterList.add(new Tile(row, col));
/* 119 */           System.out.println("Water received " + row + " " + col);
/*     */         }
/* 125 */         else if (tokens[0].equals("a")) {
/* 126 */           Ilk ilk = Ilk.fromId(Integer.parseInt(tokens[3]));
/* 127 */           this.map[row][col] = ilk;
/* 128 */           this.antList.put(new Tile(row, col), ilk);
/* 129 */           if (ilk == Ilk.MY_ANT) {
/* 130 */             this.myAnt.add(new Tile(row, col));
/*     */           }
/*     */           else
/* 133 */             this.ennemyAnt.add(new Tile(row, col));
/*     */         }
/* 135 */         else if (tokens[0].equals("f")) {
/* 136 */           this.map[row][col] = Ilk.FOOD;
/* 137 */           this.foodList.add(new Tile(row, col));
/* 138 */         } else if (tokens[0].equals("d")) {
/* 139 */           this.map[row][col] = Ilk.DEAD;
/* 140 */           this.deadList.add(new Tile(row, col));
/*     */         }
/*     */       }
/*     */     }
/* 144 */     return true;
/*     */   }
/*     */ 
/*     */   public void issueOrder(int row, int col, Aim direction) {
/* 148 */     System.out.println("o " + col + " " + row + " " + direction.symbol);
/* 149 */     System.out.flush();
/*     */   }
/*     */ 
/*     */   public void issueOrder(Tile ant, Aim direction) {
/* 153 */     System.out.println("o " + ant.col() + " " + ant.row() + " " + direction.symbol);
/* 154 */     System.out.flush();
/*     */   }
/*     */ 
/*     */   public void finishTurn() {
/* 158 */     System.out.println("go");
/* 159 */     System.out.flush();
/* 160 */     this.turn += 1;
/*     */   }
/*     */ 
/*     */   public Set<Tile> myAnts() {
/* 164 */     Set myAnts = new HashSet();
/* 165 */     for (Map.Entry ant : this.antList.entrySet()) {
/* 166 */       if (ant.getValue() == Ilk.MY_ANT) {
/* 167 */         myAnts.add(ant.getKey());
/*     */       }
/*     */     }
/* 170 */     return myAnts;
/*     */   }
/*     */ 
/*     */   public Set<Tile> enemyAnts() {
/* 174 */     Set enemyAnts = new HashSet();
/* 175 */     for (Map.Entry ant : this.antList.entrySet()) {
/* 176 */       if (((Ilk)ant.getValue()).isEnemy()) {
/* 177 */         enemyAnts.add(ant.getKey());
/*     */       }
/*     */     }
/* 180 */     return enemyAnts;
/*     */   }
/*     */ 
/*     */   public Set<Tile> food() {
/* 184 */     return new HashSet(this.foodList);
/*     */   }
/*     */ 
/*     */   public int distance(Tile t1, Tile t2) {
/* 188 */     int dRow = Math.abs(t1.row() - t2.row());
/* 189 */     int dCol = Math.abs(t1.col() - t2.col());
/*     */ 
/* 191 */     dRow = Math.min(dRow, this.rows - dRow);
/* 192 */     dCol = Math.min(dCol, this.cols - dCol);
/*     */ 
/* 194 */     return dRow * dRow + dCol * dCol;
/*     */   }
/*     */ 
/*     */   public List<Aim> directions(Tile t1, Tile t2) {
/* 198 */     List directions = new ArrayList();
/*     */ 
/* 200 */     if (t1.row() < t2.row()) {
/* 201 */       if (t2.row() - t1.row() >= this.rows / 2)
/* 202 */         directions.add(Aim.NORTH);
/*     */       else
/* 204 */         directions.add(Aim.SOUTH);
/*     */     }
/* 206 */     else if (t1.row() > t2.row()) {
/* 207 */       if (t1.row() - t2.row() >= this.rows / 2)
/* 208 */         directions.add(Aim.SOUTH);
/*     */       else {
/* 210 */         directions.add(Aim.NORTH);
/*     */       }
/*     */     }
/*     */ 
/* 214 */     if (t1.col() < t2.col()) {
/* 215 */       if (t2.col() - t1.col() >= this.cols / 2)
/* 216 */         directions.add(Aim.WEST);
/*     */       else
/* 218 */         directions.add(Aim.EAST);
/*     */     }
/* 220 */     else if (t1.col() > t2.col()) {
/* 221 */       if (t1.col() - t2.col() >= this.cols / 2)
/* 222 */         directions.add(Aim.EAST);
/*     */       else {
/* 224 */         directions.add(Aim.WEST);
/*     */       }
/*     */     }
/*     */ 
/* 228 */     return directions;
/*     */   }
/*     */ 
/*     */   public Ilk ilk(Tile location, Aim direction) {
/* 232 */     Tile new_location = tile(location, direction);
/* 233 */     return this.map[new_location.row()][new_location.col()];
/*     */   }
/*     */ 
/*     */   public Ilk ilk(Tile location) {
/* 237 */     return this.map[location.row()][location.col()];
/*     */   }
/*     */ 
/*     */   public Tile tile(Tile location, Aim direction) {
/* 241 */     int nRow = (location.row() + direction.dRow) % this.rows;
/* 242 */     if (nRow < 0) {
/* 243 */       nRow += this.rows;
/*     */     }
/* 245 */     int nCol = (location.col() + direction.dCol) % this.cols;
/* 246 */     if (nCol < 0) {
/* 247 */       nCol += this.cols;
/*     */     }
/* 249 */     return new Tile(nRow, nCol);
/*     */   }
/*     */ 
/*     */   public static void run(Bot bot) {
/* 253 */     Ants ants = new Ants();
/* 254 */     StringBuffer line = new StringBuffer();
/* 255 */     ArrayList data = new ArrayList();
/*     */     try
/*     */     {
/*     */       int c;
/* 258 */       while ((c = System.in.read()) >= 0)
/* 259 */         switch (c) {
/*     */         case 10:
/*     */         case 13:
/* 262 */           if (line.length() <= 0) continue;
/* 263 */           String full_line = line.toString();
/* 264 */           if (full_line.equals("ready")) {
/* 265 */             ants.setup(data);
/* 266 */             ants.finishTurn();
/* 267 */             data.clear();
/* 268 */           } else if (full_line.equals("go")) {
/* 269 */             ants.update(data);
/* 270 */             bot.do_turn(ants);
/* 271 */             ants.finishTurn();
/* 272 */             data.clear();
/*     */           }
/* 274 */           else if (line.length() > 0) {
/* 275 */             data.add(full_line);
/*     */           }
/*     */ 
/* 278 */           line = new StringBuffer();
/* 279 */           break;
/*     */         default:
/* 282 */           line.append((char)c);
/*     */         }
/*     */     }
/*     */     catch (Exception e)
/*     */     {
/* 287 */       e.printStackTrace(System.err);
/*     */     }
/*     */   }
/*     */ }

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     contest.Ants
 * JD-Core Version:    0.6.0
 */