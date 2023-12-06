# C++ Pair

## Purpose




```c++

#include <iostream>
#include <format>
#include "pair.hxx"


int main()
{




    pair p(58,62);

    [[maybe_unused]] auto& [a, b] = p;

    std::cout 
        << std::format("p.first: {}, a: {}") <<
    std::endl;

    a = 845;

    std::cout 
        << std::format("p.first: {}, a: {}") <<      
    std::endl;

    struct point {

    public:
        constexpr explicit point(float x, float y) noexcept : m_x(x), m_y(y) {}

        void repr() const noexcept
        {
            std::cout 
                << std::format("point <m_x: {}, m_y: {}> \n") <<
            std::endl;
        }
    
    private:
            float m_x, m_y;
    };


    [[maybe_unused]] auto const&& [p1, p2] = 
                            pair<point, point>(std::piecewise_construct,
                                                    std::forward_as_tuple(1.25f, 6.54f),
                                                    std::forward_as_tuple(2.31f, 7.89f) );
   

    p1.repr();
    p2.repr();
}
```