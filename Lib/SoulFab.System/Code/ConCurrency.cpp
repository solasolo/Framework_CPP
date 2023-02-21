#include "Concurrency.hpp"

namespace SoulFab::System
{
    void t() {
        ConcurrencyQueue<int> Test;
        Test.Push(1);
        Test.Pop();
    }
}
