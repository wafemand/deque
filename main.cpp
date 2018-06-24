#include <iostream>
#include "my_deque.h"

using namespace std;

struct tester {
    int kek;

    tester() {
        cout << "default\n";
        kek = 0;
    }

    tester(tester const &oth) {
        cout << "copy\n";
        kek = oth.kek;
    }

    tester(tester &&oth) noexcept {
        cout << "move\n";
        swap(kek, oth.kek);
    }

    tester &operator=(tester const &oth) {
        kek = oth.kek;
    }

    tester(int kek) : kek(kek) {
        cout << "from int\n";
    }

    ~tester(){
        cout << "dtor\n";
    }

    friend ostream &operator<<(ostream &s, tester const &q) {
        s << q.kek;
    }
};

template<class T>
void print(my_deque<T> const &lol) {
    for (auto const &cur : lol) {
        cout << cur << ' ';
    }
    cout << '\n';
}

int main() {


    my_deque<tester> lol(10, 10);
    print(lol);
    lol[0] = 0;
   // print(lol);
    lol.pop_front();
    //print(lol);
    lol.push_back(200);
    //print(lol);
    lol.push_back(300);
    print(lol);

    return 0;
}