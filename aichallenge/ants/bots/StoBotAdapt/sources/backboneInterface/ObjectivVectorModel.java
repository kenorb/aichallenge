package backboneInterface;

public abstract interface ObjectivVectorModel
{
  public abstract int getNbVectors();

  public abstract int getsrcX(int paramInt);

  public abstract int getsrcY(int paramInt);

  public abstract int getdstX(int paramInt);

  public abstract int getdstY(int paramInt);

  public abstract int getVectorColor(int paramInt);

  public abstract String labelSrc(int paramInt);

  public abstract String labelDst(int paramInt);
}

/* Location:           /Users/kenorb/projects/ants/aichallenge/ants/bots/StoBotAdapt/sto003_AdaptToContestbotLk.jar
 * Qualified Name:     backboneInterface.ObjectivVectorModel
 * JD-Core Version:    0.6.0
 */