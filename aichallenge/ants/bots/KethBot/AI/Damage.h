#ifndef DAMAGE_H_INCLUDED
#define DAMAGE_H_INCLUDED

class Ant;

struct AntMovement
{
    Ant* ant;
    int move;
};

struct DamageSolve
{
    std::list<AntMovement> antsMovement;
};


struct Damage
{
    int friendly, enemy;

    int sideDamage[4];
    int countSideDamage() {
        int ret = 0;
        for (int i = 0; i < 4; i++) ret += sideDamage[i];
        return ret;
    }
    void increaseSideDamage(int side) { sideDamage[side]++; }

    bool prediction;
};

#endif // DAMAGE_H_INCLUDED
