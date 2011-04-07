package backboneInterface;

public abstract interface BotInterface
{
  public abstract int nbAntWithOrderForNextTurn();

  public abstract int getXBotOrderForNextTurn(int paramInt);

  public abstract int getYBotOrderForNextTurn(int paramInt);

  public abstract int getDirBotOrderForNextTurn(int paramInt);

  public abstract void init(ServerInitMessage paramServerInitMessage);

  public abstract void turnStart(ServerTurnMessage paramServerTurnMessage);

  public static abstract interface ServerTurnMessage
  {
    public abstract int getNbViewedFood();

    public abstract int getXViewedFoodPos(int paramInt);

    public abstract int getYViewedFoodPos(int paramInt);

    public abstract int getNbFirstViewedWater();

    public abstract int getXFirstViewedWater(int paramInt);

    public abstract int getYFirstViewedWater(int paramInt);

    public abstract int getNbYourAnt();

    public abstract int getXYourAntPos(int paramInt);

    public abstract int getYYourAntPos(int paramInt);

    public abstract int getNbOtherAnt();

    public abstract int getXViewedAnt(int paramInt);

    public abstract int getYViewedAnt(int paramInt);

    public abstract int getOwnerViewedAnt(int paramInt);

    public abstract boolean gameEnd();

    public abstract int turnNumberIfNotEnd();

    public abstract int[] scoreForEachPlayerIfEnd();

    public abstract int nbPlayerIfEnd();
  }

  public static abstract interface ServerInitMessage
  {
    public abstract int loadtime();

    public abstract int turntime();

    public abstract int rows();

    public abstract int cols();

    public abstract int turns();

    public abstract int viewradius2();

    public abstract int attackradius2();

    public abstract int spawnradius2();
  }
}

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     backboneInterface.BotInterface
 * JD-Core Version:    0.6.0
 */