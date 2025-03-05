
#include <cstdlib>  // 包含rand()和srand()
#include <ctime>    // 包含time()

int random(){
    // 设置随机数种子
    srand(static_cast<unsigned int>(time(0)));
 
    // 生成一个[0, RAND_MAX]范围内的随机数
    int randomNumber = rand();
    std::cout << "Random Number: " << randomNumber << std::endl;
 
    // 生成一个[min, max]范围内的随机数
    int min = 1, max = 6;
    int range = max - min + 1;
    int randomNumberInRange = min + (rand() % range);
    std::cout << "Random Number in Range [" << min << ", " << max << "]: " << randomNumberInRange << std::endl;
 
    return 0;
}