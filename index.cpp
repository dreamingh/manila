#include <iostream>
#include <string>
#include "include/random.h"
#include <QApplication>
#include "checkerboard.h"
struct socket
{
   int black=5;
   int green=5;
   int grey=5;
   int blue=5;
};
enum player
{
    
};


int main(int argc, char *argv[])
{

   std::cout<<"hello world"<<std::endl;
   myrandom();
   QApplication app(argc, argv);
   CheckerBoard board;
   board.show();
   return app.exec();
}