#include "utility.hpp"
#include <iostream>
#include <chrono>


int main()
{
    std::unordered_map<Point, int> p2i = {{Point(1,3), 1}, {Point(4,2), 9}};
    
    p2i.insert(std::pair<Point, int>(Point(1,2), 12));
    p2i.insert(std::pair<Point, int>(Point(1,2), 12));

    if(p2i.find(Point(1,2)) != p2i.end())
    {
    	std::cout << "found (1,2) in map, value is: " << p2i.find(Point(1,2))->second << std::endl;
    }

    for (auto& x: p2i)
    {
        std::cout << "(" << x.first.x << ", " << x.first.y << ") -> " << x.second << std::endl;
    }

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    float d_valid = distance(1,2,4,4);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    Point p1(1,2);
    Point p2(4,4);
    Point p3(1,2);
    Point p4(4,4);

    std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
    float d_test1 = distanceV2(p1,p2);
    std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    float d_test2 = distanceV2(p3,p4);
    std::chrono::high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();
    float d_test3 = distanceV2(p2,p4);
    std::chrono::high_resolution_clock::time_point t6 = std::chrono::high_resolution_clock::now();

    std::cout << "expected distance is: " << d_valid << "\ttook: " << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() << std::endl;
    std::cout << "p1, p2 distance is: " << d_test1 << "\ttook: "<< std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count() << std::endl;
    std::cout << "p3, p4 distance is: " << d_test2 << "\ttook: "<< std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count() << std::endl;
    std::cout << "p2, p4 distance is: " << d_test3 << "\ttook: "<< std::chrono::duration_cast<std::chrono::microseconds>(t6-t5).count() << std::endl;

    return 0;
}
