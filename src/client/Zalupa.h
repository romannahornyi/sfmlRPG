#ifndef ZALUPA_H
#define ZALUPA_H
#include <SFML/Graphics.hpp>
class Zalupa {
    public:
        Zalupa(int x);
        int getZalupa() const;
    private:
        int zalupa;
};
#endif